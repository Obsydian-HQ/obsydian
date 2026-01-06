/**
 * Obsidian Shadow Tree Implementation
 */

#include "shadow_tree.h"
#include "../layout/engine.h"
#include <iostream>

namespace obsidian::shadow {

// ViewMutation factory methods
ViewMutation ViewMutation::createCreate(ShadowTag tag, ComponentType type, void* nativeView) {
    ViewMutation m;
    m.type = MutationType::Create;
    m.tag = tag;
    m.parentTag = 0;
    m.index = 0;
    m.componentType = type;
    m.nativeView = nativeView;
    return m;
}

ViewMutation ViewMutation::createDelete(ShadowTag tag, void* nativeView) {
    ViewMutation m;
    m.type = MutationType::Delete;
    m.tag = tag;
    m.parentTag = 0;
    m.index = 0;
    m.componentType = ComponentType::Custom;
    m.nativeView = nativeView;
    return m;
}

ViewMutation ViewMutation::createInsert(ShadowTag tag, ShadowTag parentTag, size_t index, void* nativeView) {
    ViewMutation m;
    m.type = MutationType::Insert;
    m.tag = tag;
    m.parentTag = parentTag;
    m.index = index;
    m.componentType = ComponentType::Custom;
    m.nativeView = nativeView;
    return m;
}

ViewMutation ViewMutation::createRemove(ShadowTag tag, ShadowTag parentTag, void* nativeView) {
    ViewMutation m;
    m.type = MutationType::Remove;
    m.tag = tag;
    m.parentTag = parentTag;
    m.index = 0;
    m.componentType = ComponentType::Custom;
    m.nativeView = nativeView;
    return m;
}

ViewMutation ViewMutation::createUpdate(ShadowTag tag, const LayoutMetrics& metrics, void* nativeView) {
    ViewMutation m;
    m.type = MutationType::Update;
    m.tag = tag;
    m.parentTag = 0;
    m.index = 0;
    m.componentType = ComponentType::Custom;
    m.layoutMetrics = metrics;
    m.nativeView = nativeView;
    return m;
}

// ShadowTree implementation

ShadowTree::ShadowTree(SurfaceId surfaceId)
    : surfaceId_(surfaceId)
{
    // Create root node with tag 0
    auto rootTag = generateTag();
    rootNode_ = std::make_unique<ShadowNode>(rootTag, ComponentType::Root);
    
    // Configure root node style
    auto& style = rootNode_->getStyle();
    style.flexDirection = layout::FlexDirection::Column;
    style.width = layout::LayoutValue::percent(100.0f);
    style.height = layout::LayoutValue::percent(100.0f);
}

ShadowTree::~ShadowTree() {
    // nodes_ will be cleaned up automatically
}

ShadowTag ShadowTree::generateTag() {
    return nextTag_.fetch_add(1);
}

ShadowNode* ShadowTree::createNode(ComponentType type, void* nativeView) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto tag = generateTag();
    auto node = std::make_unique<ShadowNode>(tag, type);
    
    if (nativeView) {
        node->setNativeView(nativeView);
    }
    
    auto* nodePtr = node.get();
    nodes_[tag] = std::move(node);
    
    return nodePtr;
}

ShadowNode* ShadowTree::getNode(ShadowTag tag) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if it's the root
    if (rootNode_ && rootNode_->getTag() == tag) {
        return rootNode_.get();
    }
    
    auto it = nodes_.find(tag);
    if (it != nodes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ShadowNode* ShadowTree::getNode(ShadowTag tag) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (rootNode_ && rootNode_->getTag() == tag) {
        return rootNode_.get();
    }
    
    auto it = nodes_.find(tag);
    if (it != nodes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ShadowTree::deleteNode(ShadowTag tag) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodes_.find(tag);
    if (it != nodes_.end()) {
        // TODO: Generate Delete mutations for this node and children
        nodes_.erase(it);
    }
}

void ShadowTree::setMountingCallback(MountingCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    mountingCallback_ = std::move(callback);
}

bool ShadowTree::commit(float width, float height) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!rootNode_) {
        return false;
    }
    
    // Step 1: Calculate layout
    // This computes layout for the entire tree starting from root
    layout::LayoutEngine::calculateLayout(
        rootNode_->getLayoutNode(),
        width,
        height
    );
    
    // Step 2: Update layout metrics from computed layout
    rootNode_->updateFromLayoutResult();
    
    // Step 3: Collect mutations for views with changed layout
    MutationList mutations;
    collectLayoutChanges(rootNode_.get(), mutations);
    
    // Step 4: Call mounting callback with mutations
    if (mountingCallback_ && !mutations.empty()) {
        mountingCallback_(mutations);
    }
    
    return !mutations.empty();
}

void ShadowTree::markDirty() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (rootNode_) {
        rootNode_->markDirty();
    }
}

bool ShadowTree::isDirty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return rootNode_ && rootNode_->isDirty();
}

void ShadowTree::collectLayoutChanges(ShadowNode* node, MutationList& mutations) {
    if (!node) {
        return;
    }
    
    // If layout changed and has a native view, generate Update mutation
    if (node->layoutMetricsChanged_ && node->getNativeView()) {
        mutations.push_back(
            ViewMutation::createUpdate(
                node->getTag(),
                node->getLayoutMetrics(),
                node->getNativeView()
            )
        );
        node->layoutMetricsChanged_ = false;
    }
    
    // Recurse to children
    for (auto* child : node->getChildren()) {
        collectLayoutChanges(child, mutations);
    }
}

// ShadowTreeRegistry implementation

ShadowTreeRegistry& ShadowTreeRegistry::getInstance() {
    static ShadowTreeRegistry instance;
    return instance;
}

ShadowTree* ShadowTreeRegistry::createTree(SurfaceId surfaceId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if already exists
    auto it = trees_.find(surfaceId);
    if (it != trees_.end()) {
        return it->second.get();
    }
    
    // Create new tree
    auto tree = std::make_unique<ShadowTree>(surfaceId);
    auto* treePtr = tree.get();
    trees_[surfaceId] = std::move(tree);
    
    return treePtr;
}

ShadowTree* ShadowTreeRegistry::getTree(SurfaceId surfaceId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = trees_.find(surfaceId);
    if (it != trees_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ShadowTreeRegistry::removeTree(SurfaceId surfaceId) {
    std::lock_guard<std::mutex> lock(mutex_);
    trees_.erase(surfaceId);
}

SurfaceId ShadowTreeRegistry::generateSurfaceId() {
    return nextSurfaceId_.fetch_add(1);
}

} // namespace obsidian::shadow
