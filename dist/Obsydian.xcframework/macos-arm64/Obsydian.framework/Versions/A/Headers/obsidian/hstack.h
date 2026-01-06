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
class VStack;
class Screen;

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
     * Add HStack to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Add HStack to a screen (for router-managed views)
     * The screen owns the view after this call
     */
    void addToScreen(Screen& screen);
    
    /**
     * Add HStack to an arbitrary parent view (low-level API)
     * @param parentView Native view handle (NSView* on macOS)
     */
    void addToParentView(void* parentView);
    
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
    
    /**
     * Get the internal layout node for this HStack.
     * Used for layout tree construction when nesting containers.
     * The returned pointer is valid until the HStack is destroyed or ownership is released.
     */
    void* getLayoutNode() const;
    
    /**
     * Release ownership of the layout node.
     * After calling this, the HStack will not delete its layout node when destroyed.
     * This is used when the layout node is adopted by a parent container.
     * 
     * This implements the ownership transfer model inspired by React Native's Shadow Tree:
     * - When HStack is added as child to VStack, VStack takes ownership of HStack's layoutNode
     * - HStack's destructor won't delete the layoutNode (parent owns it now)
     * - This ensures layout persists even when HStack object goes out of scope
     */
    void releaseLayoutNodeOwnership();
    
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
