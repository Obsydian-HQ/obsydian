/**
 * Obsidian Public API - ScrollView
 * 
 * This is the public API header for scroll view management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <memory>

namespace obsidian {

// Forward declarations
class Window;
class TextView;

/**
 * ScrollView class - Platform-agnostic scroll view management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * scroll views across all platforms. Platform-specific details are hidden.
 * 
 * This is a 1:1 mapping to the platform's native scroll view API (NSScrollView on macOS).
 */
class ScrollView {
public:
    /**
     * Create a new scroll view
     */
    ScrollView();
    
    /**
     * Destructor - automatically removes and destroys the scroll view
     */
    ~ScrollView();
    
    /**
     * Create a scroll view with the given parameters
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Scroll view width in pixels
     * @param height Scroll view height in pixels
     * @return true if scroll view was created successfully
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Set the document view for the scroll view
     * The document view is the view that will be scrolled
     * @param textView The text view to use as the document view
     */
    void setDocumentView(TextView& textView);
    
    /**
     * Get the document view from the scroll view
     * Returns nullptr if no document view is set
     */
    void* getDocumentView() const;
    
    /**
     * Set scroll view visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if scroll view is visible
     */
    bool isVisible() const;
    
    /**
     * Add scroll view to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove scroll view from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if scroll view is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle for this ScrollView
     * Returns an opaque pointer to the underlying NSView (NSScrollView), or nullptr if invalid
     * This is useful for integrating with other components that need direct view access (e.g., SplitView)
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    ScrollView(const ScrollView&) = delete;
    ScrollView& operator=(const ScrollView&) = delete;
    
    // Movable
    ScrollView(ScrollView&&) noexcept;
    ScrollView& operator=(ScrollView&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

