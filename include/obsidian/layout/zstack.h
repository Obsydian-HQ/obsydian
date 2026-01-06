/**
 * Obsidian Public API - ZStack
 * 
 * Overlay stack layout container that arranges child views on top of each other
 * with configurable alignment-based positioning.
 */

#pragma once

#include <cstdint>
#include <memory>
#include "obsidian/window.h"
#include "obsidian/layout/alignment.h"

namespace obsidian {

// Forward declarations
class Button;
class TextField;
class TextView;
class Link;

/**
 * ZStack class - Overlay stack layout container
 * 
 * Overlays child views on top of each other with alignment-based positioning.
 * Uses NSLayoutConstraint on macOS for native, performant layout.
 * Z-order: last child added is on top.
 */
class ZStack {
public:
    /**
     * Create a new ZStack
     */
    ZStack();
    
    /**
     * Destructor - automatically removes and destroys the ZStack
     */
    ~ZStack();
    
    /**
     * Configuration methods
     */
    
    /**
     * Set alignment for positioning child views
     * All children are positioned at the same alignment point (e.g., Center, TopLeading, etc.)
     */
    void setAlignment(layout::Alignment alignment);
    
    /**
     * Get alignment
     */
    layout::Alignment getAlignment() const;
    
    /**
     * Children management
     */
    
    /**
     * Add a button as a child
     */
    void addChild(Button& button);
    
    /**
     * Add a link as a child
     */
    void addChild(Link& link);
    
    /**
     * Remove a button child
     */
    void removeChild(Button& button);
    
    /**
     * Remove a link child
     */
    void removeChild(Link& link);
    
    /**
     * Clear all children
     */
    void clearChildren();
    
    /**
     * Window integration
     */
    
    /**
     * Add ZStack to a window's content view
     * The ZStack will fill the window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove ZStack from its parent view
     */
    void removeFromParent();
    
    /**
     * Layout control
     */
    
    /**
     * Update layout constraints (call after adding/removing children or changing properties)
     */
    void updateLayout();
    
    /**
     * Check if ZStack is valid
     */
    bool isValid() const;
    
    // Non-copyable
    ZStack(const ZStack&) = delete;
    ZStack& operator=(const ZStack&) = delete;
    
    // Movable
    ZStack(ZStack&&) noexcept;
    ZStack& operator=(ZStack&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
