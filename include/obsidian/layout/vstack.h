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
#include "obsidian/layout/alignment.h"

namespace obsidian {

// Forward declarations
class Button;
class TextField;
class TextView;
class Spacer;
class Link;
class HStack;
class Screen;

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
     * Add a text view as a child
     */
    void addChild(TextView& textView);
    
    /**
     * Add a VStack as a child (nesting support)
     */
    void addChild(VStack& vstack);
    
    /**
     * Add an HStack as a child (nesting support)
     */
    void addChild(HStack& hstack);
    
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
     * Remove a text view child
     */
    void removeChild(TextView& textView);
    
    /**
     * Remove a VStack child
     */
    void removeChild(VStack& vstack);
    
    /**
     * Remove an HStack child
     */
    void removeChild(HStack& hstack);
    
    /**
     * Clear all children
     */
    void clearChildren();
    
    /**
     * View integration
     */
    
    /**
     * Add VStack to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Add VStack to a screen (for router-managed views)
     * The screen owns the view after this call
     */
    void addToScreen(Screen& screen);
    
    /**
     * Add VStack to an arbitrary parent view (low-level API)
     * @param parentView Native view handle (NSView* on macOS)
     */
    void addToParentView(void* parentView);
    
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
    
    /**
     * Get the internal layout node for this VStack.
     * Used for layout tree construction when nesting containers.
     * The returned pointer is valid until the VStack is destroyed or ownership is released.
     */
    void* getLayoutNode() const;
    
    /**
     * Release ownership of the layout node.
     * After calling this, the VStack will not delete its layout node when destroyed.
     * This is used when the layout node is adopted by a parent container.
     * 
     * This implements the ownership transfer model inspired by React Native's Shadow Tree:
     * - When VStack is added as child to HStack/VStack, parent takes ownership of VStack's layoutNode
     * - VStack's destructor won't delete the layoutNode (parent owns it now)
     * - This ensures layout persists even when VStack object goes out of scope
     */
    void releaseLayoutNodeOwnership();
    
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
