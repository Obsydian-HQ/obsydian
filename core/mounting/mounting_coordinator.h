/**
 * MountingCoordinator - Coordinates view mutations and applies them to native views
 * 
 * Following React Native's RCTMountingManager pattern:
 * - Receives mutations from the ShadowTree
 * - Applies them to native views via ComponentViewRegistry
 * - Ensures mutations are applied on the main thread
 */

#pragma once

#include "component_view_registry.h"
#include "view_mutation.h"
#include <functional>
#include <memory>

namespace obsidian::mounting {

/**
 * Platform-specific functions for applying mutations to native views
 */
struct MountingPlatformDelegate {
    // Add child view to parent at index
    std::function<void(void* parentView, void* childView, int32_t index)> mountChildView;
    
    // Remove child view from parent at index
    std::function<void(void* parentView, void* childView, int32_t index)> unmountChildView;
    
    // Update view's layout metrics (frame)
    std::function<void(void* view, const LayoutMetrics& metrics)> updateLayoutMetrics;
    
    // Update view's state (hidden, opacity, etc.)
    std::function<void(void* view, const ViewState& state)> updateViewState;
    
    // Prepare view for recycling (reset to default state)
    std::function<void(void* view)> prepareForRecycle;
};

/**
 * Coordinates the application of view mutations to the native layer
 */
class MountingCoordinator {
public:
    static MountingCoordinator& getInstance();
    
    // Prevent copy/move
    MountingCoordinator(const MountingCoordinator&) = delete;
    MountingCoordinator& operator=(const MountingCoordinator&) = delete;
    
    /**
     * Set the platform delegate for native operations
     */
    void setPlatformDelegate(MountingPlatformDelegate delegate);
    
    /**
     * Apply a list of mutations to native views
     * This is the main entry point called after ShadowTree diff
     */
    void performMutations(const ViewMutationList& mutations);
    
    /**
     * Apply a single mutation
     */
    void performMutation(const ViewMutation& mutation);
    
    /**
     * Get the ComponentViewRegistry
     */
    ComponentViewRegistry& getRegistry() { 
        return ComponentViewRegistry::getInstance(); 
    }

private:
    MountingCoordinator() = default;
    
    void handleCreateMutation(const ViewMutation& mutation);
    void handleDeleteMutation(const ViewMutation& mutation);
    void handleInsertMutation(const ViewMutation& mutation);
    void handleRemoveMutation(const ViewMutation& mutation);
    void handleUpdateMutation(const ViewMutation& mutation);
    
    MountingPlatformDelegate delegate_;
    bool hasDelegate_ = false;
};

} // namespace obsidian::mounting
