/**
 * ComponentViewRegistry - Manages native view lifecycle by Tag
 * 
 * This follows React Native's architecture where:
 * - Native views are identified by integer Tags, not raw pointers
 * - Views are pooled and recycled, not destroyed on C++ object destruction
 * - Only the registry decides when to create/destroy native views
 * - Mutations (Create, Delete) drive the view lifecycle
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace obsidian::mounting {

// Tag is an integer identifier for native views (like React Native)
using Tag = int32_t;

// Component handle identifies the type of component (Button, VStack, etc.)
using ComponentHandle = std::string;

// Forward declaration
struct ComponentViewDescriptor;

/**
 * Function type for creating native views
 * Returns a void* handle to the native view
 */
using ViewFactory = std::function<void*(const ComponentHandle& handle)>;

/**
 * Function type for destroying native views
 */
using ViewDestructor = std::function<void(void* nativeView, const ComponentHandle& handle)>;

/**
 * Function type for preparing a view for recycling
 */
using ViewRecycler = std::function<void(void* nativeView, const ComponentHandle& handle)>;

/**
 * Descriptor for a component view
 * Contains the native view handle and metadata
 */
struct ComponentViewDescriptor {
    void* view = nullptr;
    ComponentHandle componentHandle;
    bool shouldBeRecycled = true;
    
    ComponentViewDescriptor() = default;
    ComponentViewDescriptor(void* v, const ComponentHandle& h) 
        : view(v), componentHandle(h) {}
};

/**
 * Registry of native component views
 * Provides allocation, recycling, and querying by Tag
 * 
 * This is the SINGLE SOURCE OF TRUTH for native view lifecycle.
 * C++ component objects (Button, Link, etc.) do NOT own native views -
 * they only hold Tags that reference views in this registry.
 */
class ComponentViewRegistry {
public:
    static constexpr size_t kRecyclePoolMaxSize = 1024;
    
    static ComponentViewRegistry& getInstance();
    
    // Prevent copy/move
    ComponentViewRegistry(const ComponentViewRegistry&) = delete;
    ComponentViewRegistry& operator=(const ComponentViewRegistry&) = delete;
    
    /**
     * Register factory functions for creating/destroying views
     */
    void registerViewFactory(const ComponentHandle& handle, 
                            ViewFactory factory,
                            ViewDestructor destructor,
                            ViewRecycler recycler = nullptr);
    
    /**
     * Dequeue a view from the recycle pool or create a new one
     * Associates it with the given Tag
     * Returns the ComponentViewDescriptor
     */
    ComponentViewDescriptor dequeueComponentView(const ComponentHandle& handle, Tag tag);
    
    /**
     * Return a view to the recycle pool
     * The view is NOT destroyed - it's kept for reuse
     */
    void enqueueComponentView(const ComponentHandle& handle, Tag tag);
    
    /**
     * Get the descriptor for a view by Tag
     */
    const ComponentViewDescriptor& getComponentViewDescriptor(Tag tag) const;
    
    /**
     * Find a native view by Tag
     * Returns nullptr if not found
     */
    void* findNativeView(Tag tag) const;
    
    /**
     * Check if a Tag is registered
     */
    bool hasTag(Tag tag) const;
    
    /**
     * Generate a new unique Tag
     */
    Tag generateTag();
    
    /**
     * Clear all recycled views (e.g., on memory warning)
     */
    void clearRecyclePool();
    
    /**
     * Get count of active views
     */
    size_t activeViewCount() const { return registry_.size(); }
    
    /**
     * Get count of recycled views for a component type
     */
    size_t recycledViewCount(const ComponentHandle& handle) const;

private:
    ComponentViewRegistry() = default;
    ~ComponentViewRegistry();
    
    ComponentViewDescriptor createComponentView(const ComponentHandle& handle);
    void destroyComponentView(ComponentViewDescriptor& descriptor);
    void prepareViewForRecycle(ComponentViewDescriptor& descriptor);
    
    // Tag -> ComponentViewDescriptor mapping
    std::unordered_map<Tag, ComponentViewDescriptor> registry_;
    
    // ComponentHandle -> recycled views pool
    std::unordered_map<ComponentHandle, std::vector<ComponentViewDescriptor>> recyclePool_;
    
    // Factory functions for each component type
    std::unordered_map<ComponentHandle, ViewFactory> factories_;
    std::unordered_map<ComponentHandle, ViewDestructor> destructors_;
    std::unordered_map<ComponentHandle, ViewRecycler> recyclers_;
    
    // Next available Tag
    Tag nextTag_ = 1;  // Start at 1, 0 is reserved for "no tag"
    
    // Empty descriptor for failed lookups
    static const ComponentViewDescriptor kEmptyDescriptor;
};

// Component handle constants
namespace ComponentHandles {
    constexpr const char* Button = "Button";
    constexpr const char* TextView = "TextView";
    constexpr const char* TextField = "TextField";
    constexpr const char* VStack = "VStack";
    constexpr const char* HStack = "HStack";
    constexpr const char* ZStack = "ZStack";
    constexpr const char* Spacer = "Spacer";
    constexpr const char* ScrollView = "ScrollView";
    constexpr const char* Link = "Link";
    constexpr const char* List = "List";
    constexpr const char* Table = "Table";
    constexpr const char* Sidebar = "Sidebar";
    constexpr const char* SplitView = "SplitView";
    constexpr const char* Window = "Window";
    constexpr const char* ScreenContainer = "ScreenContainer";
    constexpr const char* Screen = "Screen";
}

} // namespace obsidian::mounting
