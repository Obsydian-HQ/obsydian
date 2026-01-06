/**
 * ComponentViewRegistry Implementation
 * 
 * Follows React Native's RCTComponentViewRegistry pattern
 */

#include "component_view_registry.h"
#include <stdexcept>
#include <iostream>

namespace obsidian::mounting {

// Static empty descriptor for failed lookups
const ComponentViewDescriptor ComponentViewRegistry::kEmptyDescriptor{};

ComponentViewRegistry& ComponentViewRegistry::getInstance() {
    static ComponentViewRegistry instance;
    return instance;
}

ComponentViewRegistry::~ComponentViewRegistry() {
    // Destroy all active views
    for (auto& [tag, descriptor] : registry_) {
        destroyComponentView(descriptor);
    }
    registry_.clear();
    
    // Destroy all recycled views
    for (auto& [handle, pool] : recyclePool_) {
        for (auto& descriptor : pool) {
            destroyComponentView(descriptor);
        }
    }
    recyclePool_.clear();
}

void ComponentViewRegistry::registerViewFactory(
    const ComponentHandle& handle,
    ViewFactory factory,
    ViewDestructor destructor,
    ViewRecycler recycler) {
    
    factories_[handle] = std::move(factory);
    destructors_[handle] = std::move(destructor);
    if (recycler) {
        recyclers_[handle] = std::move(recycler);
    }
}

ComponentViewDescriptor ComponentViewRegistry::dequeueComponentView(
    const ComponentHandle& handle, 
    Tag tag) {
    
    // Check if tag is already registered (error)
    if (registry_.find(tag) != registry_.end()) {
        std::cerr << "[ComponentViewRegistry] ERROR: Attempt to dequeue already registered tag: " 
                  << tag << std::endl;
        return kEmptyDescriptor;
    }
    
    // Try to get from recycle pool first
    auto& pool = recyclePool_[handle];
    ComponentViewDescriptor descriptor;
    
    if (!pool.empty()) {
        descriptor = pool.back();
        pool.pop_back();
    } else {
        // Create new view
        descriptor = createComponentView(handle);
    }
    
    if (!descriptor.view) {
        std::cerr << "[ComponentViewRegistry] ERROR: Failed to create view for handle: " 
                  << handle << std::endl;
        return kEmptyDescriptor;
    }
    
    // Register with tag
    registry_[tag] = descriptor;
    
    return descriptor;
}

void ComponentViewRegistry::enqueueComponentView(
    const ComponentHandle& handle, 
    Tag tag) {
    
    auto it = registry_.find(tag);
    if (it == registry_.end()) {
        std::cerr << "[ComponentViewRegistry] ERROR: Attempt to enqueue unregistered tag: " 
                  << tag << std::endl;
        return;
    }
    
    ComponentViewDescriptor descriptor = it->second;
    registry_.erase(it);
    
    auto& pool = recyclePool_[handle];
    
    // Check if we should recycle or destroy
    if (pool.size() >= kRecyclePoolMaxSize || !descriptor.shouldBeRecycled) {
        destroyComponentView(descriptor);
        return;
    }
    
    // Prepare for recycling
    prepareViewForRecycle(descriptor);
    pool.push_back(descriptor);
}

const ComponentViewDescriptor& ComponentViewRegistry::getComponentViewDescriptor(Tag tag) const {
    auto it = registry_.find(tag);
    if (it == registry_.end()) {
        return kEmptyDescriptor;
    }
    return it->second;
}

void* ComponentViewRegistry::findNativeView(Tag tag) const {
    auto it = registry_.find(tag);
    if (it == registry_.end()) {
        return nullptr;
    }
    return it->second.view;
}

bool ComponentViewRegistry::hasTag(Tag tag) const {
    return registry_.find(tag) != registry_.end();
}

Tag ComponentViewRegistry::generateTag() {
    return nextTag_++;
}

void ComponentViewRegistry::clearRecyclePool() {
    for (auto& [handle, pool] : recyclePool_) {
        for (auto& descriptor : pool) {
            destroyComponentView(descriptor);
        }
        pool.clear();
    }
}

size_t ComponentViewRegistry::recycledViewCount(const ComponentHandle& handle) const {
    auto it = recyclePool_.find(handle);
    if (it == recyclePool_.end()) {
        return 0;
    }
    return it->second.size();
}

ComponentViewDescriptor ComponentViewRegistry::createComponentView(const ComponentHandle& handle) {
    auto factoryIt = factories_.find(handle);
    if (factoryIt == factories_.end()) {
        std::cerr << "[ComponentViewRegistry] ERROR: No factory registered for: " 
                  << handle << std::endl;
        return ComponentViewDescriptor{};
    }
    
    void* view = factoryIt->second(handle);
    return ComponentViewDescriptor{view, handle};
}

void ComponentViewRegistry::destroyComponentView(ComponentViewDescriptor& descriptor) {
    if (!descriptor.view) {
        return;
    }
    
    auto destructorIt = destructors_.find(descriptor.componentHandle);
    if (destructorIt != destructors_.end() && destructorIt->second) {
        destructorIt->second(descriptor.view, descriptor.componentHandle);
    }
    
    descriptor.view = nullptr;
}

void ComponentViewRegistry::prepareViewForRecycle(ComponentViewDescriptor& descriptor) {
    if (!descriptor.view) {
        return;
    }
    
    auto recyclerIt = recyclers_.find(descriptor.componentHandle);
    if (recyclerIt != recyclers_.end() && recyclerIt->second) {
        recyclerIt->second(descriptor.view, descriptor.componentHandle);
    }
}

} // namespace obsidian::mounting
