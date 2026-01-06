/**
 * Obsidian Shadow Tree
 * 
 * Architecture inspired by React Native's Fabric renderer.
 * 
 * The ShadowTree:
 * - OWNS all ShadowNodes (they are not owned by UI components)
 * - Computes layout centrally when commit() is called
 * - Generates mutations (with computed layoutMetrics) for MountingManager
 * - Persists for the lifetime of a surface (window/screen)
 * 
 * Usage:
 * 1. Components register with the ShadowTree to get a ShadowNode
 * 2. Components modify their ShadowNode's style
 * 3. When ready, call commit() to compute layout and generate mutations
 * 4. MountingManager applies mutations to native views
 */

#pragma once

#include "shadow_node.h"
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>

namespace obsidian::shadow {

// Surface ID - identifies a window/screen
using SurfaceId = uint64_t;

/**
 * View mutation types
 * Similar to React Native's ShadowViewMutation
 */
enum class MutationType {
    Create,     // Create a new native view
    Delete,     // Delete a native view
    Insert,     // Insert view into parent
    Remove,     // Remove view from parent
    Update      // Update view's layoutMetrics
};

/**
 * View mutation
 * Describes a single change to apply to native views.
 * Importantly, includes the computed layoutMetrics.
 */
struct ViewMutation {
    MutationType type;
    ShadowTag tag;
    ShadowTag parentTag;        // For Insert/Remove
    size_t index;               // For Insert
    ComponentType componentType;
    LayoutMetrics layoutMetrics;
    void* nativeView;           // Native view handle
    
    // Factory methods
    static ViewMutation createCreate(ShadowTag tag, ComponentType type, void* nativeView);
    static ViewMutation createDelete(ShadowTag tag, void* nativeView);
    static ViewMutation createInsert(ShadowTag tag, ShadowTag parentTag, size_t index, void* nativeView);
    static ViewMutation createRemove(ShadowTag tag, ShadowTag parentTag, void* nativeView);
    static ViewMutation createUpdate(ShadowTag tag, const LayoutMetrics& metrics, void* nativeView);
};

using MutationList = std::vector<ViewMutation>;

/**
 * Callback type for applying mutations to native views
 */
using MountingCallback = std::function<void(const MutationList& mutations)>;

/**
 * Shadow Tree
 * 
 * Manages a tree of ShadowNodes for a single surface (window).
 */
class ShadowTree {
public:
    explicit ShadowTree(SurfaceId surfaceId);
    ~ShadowTree();
    
    // Surface identity
    SurfaceId getSurfaceId() const { return surfaceId_; }
    
    // Root node (automatically created)
    ShadowNode* getRootNode() { return rootNode_.get(); }
    const ShadowNode* getRootNode() const { return rootNode_.get(); }
    
    /**
     * Create a new shadow node.
     * The ShadowTree owns the node.
     */
    ShadowNode* createNode(ComponentType type, void* nativeView = nullptr);
    
    /**
     * Get a node by tag
     */
    ShadowNode* getNode(ShadowTag tag);
    const ShadowNode* getNode(ShadowTag tag) const;
    
    /**
     * Delete a node and all its children.
     * Generates Delete mutations.
     */
    void deleteNode(ShadowTag tag);
    
    /**
     * Set the mounting callback.
     * Called with mutations after commit().
     */
    void setMountingCallback(MountingCallback callback);
    
    /**
     * Compute layout and generate mutations.
     * This is the main entry point - similar to React Native's commit().
     * 
     * @param width Available width for layout
     * @param height Available height for layout
     * @return true if layout was computed and mutations were generated
     */
    bool commit(float width, float height);
    
    /**
     * Force a layout recalculation on next commit
     */
    void markDirty();
    
    /**
     * Check if tree needs layout
     */
    bool isDirty() const;

private:
    // Generate next unique tag
    ShadowTag generateTag();
    
    // Collect nodes with changed layout metrics
    void collectLayoutChanges(ShadowNode* node, MutationList& mutations);
    
    SurfaceId surfaceId_;
    std::unique_ptr<ShadowNode> rootNode_;
    
    // All nodes owned by this tree (including root)
    std::unordered_map<ShadowTag, std::unique_ptr<ShadowNode>> nodes_;
    
    // Tag generator
    std::atomic<ShadowTag> nextTag_{1};  // 0 is reserved
    
    // Mounting callback
    MountingCallback mountingCallback_;
    
    // Thread safety
    mutable std::mutex mutex_;
    
    // Non-copyable
    ShadowTree(const ShadowTree&) = delete;
    ShadowTree& operator=(const ShadowTree&) = delete;
};

/**
 * Shadow Tree Registry
 * 
 * Global registry of ShadowTrees, one per surface (window).
 * Similar to React Native's ShadowTreeRegistry.
 */
class ShadowTreeRegistry {
public:
    static ShadowTreeRegistry& getInstance();
    
    /**
     * Create a new ShadowTree for a surface
     */
    ShadowTree* createTree(SurfaceId surfaceId);
    
    /**
     * Get existing ShadowTree
     */
    ShadowTree* getTree(SurfaceId surfaceId);
    
    /**
     * Remove a ShadowTree
     */
    void removeTree(SurfaceId surfaceId);
    
    /**
     * Generate a unique surface ID
     */
    SurfaceId generateSurfaceId();

private:
    ShadowTreeRegistry() = default;
    ~ShadowTreeRegistry() = default;
    
    std::unordered_map<SurfaceId, std::unique_ptr<ShadowTree>> trees_;
    std::atomic<SurfaceId> nextSurfaceId_{1};
    std::mutex mutex_;
    
    // Singleton
    ShadowTreeRegistry(const ShadowTreeRegistry&) = delete;
    ShadowTreeRegistry& operator=(const ShadowTreeRegistry&) = delete;
};

} // namespace obsidian::shadow
