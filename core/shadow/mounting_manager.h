/**
 * Obsidian Mounting Manager
 * 
 * Architecture inspired by React Native's RCTMountingManager.
 * 
 * The MountingManager:
 * - Receives mutations from ShadowTree (with computed layoutMetrics)
 * - Applies mutations to native views
 * - Native views are completely dumb - they just receive frames
 */

#pragma once

#include "shadow_tree.h"
#include <functional>

namespace obsidian::shadow {

/**
 * Callback to set frame on a native view
 * @param nativeView The native view handle
 * @param x X position relative to parent
 * @param y Y position relative to parent  
 * @param width Width of the view
 * @param height Height of the view
 */
using SetFrameFunc = std::function<void(void* nativeView, float x, float y, float width, float height)>;

/**
 * Mounting Manager
 * 
 * Applies mutations to native views.
 * This is where computed layout metrics are applied to actual native views.
 */
class MountingManager {
public:
    static MountingManager& getInstance();
    
    /**
     * Set the platform-specific frame setter.
     * Must be called before any mutations can be applied.
     */
    void setSetFrameFunc(SetFrameFunc func);
    
    /**
     * Apply a list of mutations to native views.
     * Called by ShadowTree after commit().
     */
    void applyMutations(const MutationList& mutations);
    
    /**
     * Apply a single mutation
     */
    void applyMutation(const ViewMutation& mutation);

private:
    MountingManager() = default;
    ~MountingManager() = default;
    
    SetFrameFunc setFrameFunc_;
    
    // Singleton
    MountingManager(const MountingManager&) = delete;
    MountingManager& operator=(const MountingManager&) = delete;
};

} // namespace obsidian::shadow
