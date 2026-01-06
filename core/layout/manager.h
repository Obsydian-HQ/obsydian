/**
 * Obsidian Layout Manager
 * 
 * Bridges the Layout Engine to native views.
 * Provides platform-agnostic interface for triggering layout calculation
 * and applying results to native views.
 */

#pragma once

#include "node.h"
#include "engine.h"
#include <memory>
#include <unordered_map>

namespace obsidian::layout {

/**
 * Platform-specific frame setter function
 * Must be implemented by each platform (macOS, iOS, etc.)
 */
using NativeSetFrameFunc = void(*)(void* nativeView, 
                                    float x, float y, 
                                    float width, float height);

/**
 * LayoutManager - Manages layout calculation and application
 * 
 * Usage:
 * 1. Create LayoutNodes and build tree
 * 2. Associate native views with nodes
 * 3. Call calculateAndApply() on root
 */
class LayoutManager {
public:
    /**
     * Get the singleton instance
     */
    static LayoutManager& getInstance();
    
    /**
     * Set the platform-specific frame setter function
     * Must be called before any layout application
     */
    void setNativeSetFrameFunc(NativeSetFrameFunc func);
    
    /**
     * Calculate layout for a node tree and apply to native views
     * 
     * @param root Root LayoutNode
     * @param width Available width
     * @param height Available height
     */
    void calculateAndApply(LayoutNode* root, float width, float height);
    
    /**
     * Apply layout to native views (after calculateLayout has been called)
     */
    void applyToNativeViews(LayoutNode* root);
    
private:
    LayoutManager() = default;
    ~LayoutManager() = default;
    
    // Non-copyable
    LayoutManager(const LayoutManager&) = delete;
    LayoutManager& operator=(const LayoutManager&) = delete;
    
    NativeSetFrameFunc setFrameFunc_ = nullptr;
    
    // Internal recursive apply
    void applyRecursive(LayoutNode* node, float parentAbsX, float parentAbsY);
};

} // namespace obsidian::layout
