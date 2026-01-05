/**
 * Obsidian Public API - HStack
 * 
 * Horizontal stack layout container that arranges child views horizontally
 * with configurable spacing, alignment, and padding.
 */

#pragma once

#include <cstdint>
#include <memory>
#include "obsidian/window.h"
#include "obsidian/vstack.h"  // For Padding struct
#include "core/layout/alignment.h"

namespace obsidian {

// Forward declarations
class Button;
class TextField;
class TextView;
class Spacer;
class Link;

/**
 * HStack class - Horizontal stack layout container
 * 
 * Arranges child views horizontally with spacing, alignment, and padding support.
 * Uses NSLayoutConstraint on macOS for native, performant layout.
 */
class HStack {
public:
    /**
     * Create a new HStack
     */
    HStack();
    
    /**
     * Destructor - automatically removes and destroys the HStack
     */
    ~HStack();
    
    /**
     * Configuration methods
     */
    
    /**
     * Set spacing between child views
     */
    void setSpacing(double spacing);
    
    /**
     * Get spacing between child views
     */
    double getSpacing() const;
    
    /**
     * Set vertical alignment (Top, Middle, Bottom)
     */
    void setAlignment(layout::Alignment alignment);
    
    /**
     * Get vertical alignment
     */
    layout::Alignment getAlignment() const;
    
    /**
     * Set padding around the stack
     */
    void setPadding(const Padding& padding);
    
    /**
     * Get padding around the stack
     */
    Padding getPadding() const;
    
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
     * Add a spacer as a child
     */
    void addChild(Spacer& spacer);
    
    /**
     * Remove a button child
     */
    void removeChild(Button& button);
    
    /**
     * Remove a link child
     */
    void removeChild(Link& link);
    
    /**
     * Remove a spacer child
     */
    void removeChild(Spacer& spacer);
    
    /**
     * Clear all children
     */
    void clearChildren();
    
    /**
     * Window integration
     */
    
    /**
     * Add HStack to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove HStack from its parent view
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
     * Check if HStack is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle for this HStack
     * Returns an opaque pointer to the underlying NSView, or nullptr if invalid
     * This is useful for integrating with other components that need direct view access (e.g., SplitView)
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    HStack(const HStack&) = delete;
    HStack& operator=(const HStack&) = delete;
    
    // Movable
    HStack(HStack&&) noexcept;
    HStack& operator=(HStack&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
