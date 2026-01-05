/**
 * Obsidian Public API - VStack
 * 
 * Vertical stack layout container that arranges child views vertically
 * with configurable spacing, alignment, and padding.
 */

#pragma once

#include <cstdint>
#include <memory>
#include "obsidian/window.h"
#include "core/layout/alignment.h"

namespace obsidian {

// Forward declarations
class Button;
class TextField;
class TextView;
class Spacer;
class Link;

/**
 * Padding structure for layout containers
 */
struct Padding {
    double top;
    double bottom;
    double leading;
    double trailing;
    
    /**
     * Create padding with equal values on all sides
     */
    static Padding all(double value) {
        return Padding{value, value, value, value};
    }
    
    /**
     * Create symmetric padding
     */
    static Padding symmetric(double vertical, double horizontal) {
        return Padding{vertical, vertical, horizontal, horizontal};
    }
    
    /**
     * Create padding with specific values for each side
     */
    static Padding only(double top, double bottom, double leading, double trailing) {
        return Padding{top, bottom, leading, trailing};
    }
};

/**
 * VStack class - Vertical stack layout container
 * 
 * Arranges child views vertically with spacing, alignment, and padding support.
 * Uses NSLayoutConstraint on macOS for native, performant layout.
 */
class VStack {
public:
    /**
     * Create a new VStack
     */
    VStack();
    
    /**
     * Destructor - automatically removes and destroys the VStack
     */
    ~VStack();
    
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
     * Set horizontal alignment (Leading, Center, Trailing)
     */
    void setAlignment(layout::Alignment alignment);
    
    /**
     * Get horizontal alignment
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
     * Add VStack to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove VStack from its parent view
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
     * Check if VStack is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle for this VStack
     * Returns an opaque pointer to the underlying NSView, or nullptr if invalid
     * This is useful for integrating with other components that need direct view access (e.g., SplitView)
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    VStack(const VStack&) = delete;
    VStack& operator=(const VStack&) = delete;
    
    // Movable
    VStack(VStack&&) noexcept;
    VStack& operator=(VStack&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
