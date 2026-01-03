/**
 * Obsidian Public API - TextView
 * 
 * This is the public API header for text view management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace obsidian {

// Forward declaration
class Window;

/**
 * TextView class - Platform-agnostic text view management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * multi-line text views across all platforms. Platform-specific details are hidden.
 * 
 * This is a 1:1 mapping to the platform's native text view API (NSTextView on macOS).
 */
class TextView {
public:
    /**
     * Create a new text view
     */
    TextView();
    
    /**
     * Destructor - automatically removes and destroys the text view
     */
    ~TextView();
    
    /**
     * Create a text view with the given parameters
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Text view width in pixels
     * @param height Text view height in pixels
     * @return true if text view was created successfully
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Set the text view text
     */
    void setString(const std::string& text);
    
    /**
     * Get the text view text
     */
    std::string getString() const;
    
    /**
     * Set text view editable state
     */
    void setEditable(bool editable);
    
    /**
     * Check if text view is editable
     */
    bool isEditable() const;
    
    /**
     * Set text view selectable state
     */
    void setSelectable(bool selectable);
    
    /**
     * Check if text view is selectable
     */
    bool isSelectable() const;
    
    /**
     * Set text view visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if text view is visible
     */
    bool isVisible() const;
    
    /**
     * Set text view enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if text view is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add text view to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove text view from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if text view is valid
     */
    bool isValid() const;
    
    /**
     * Get the native platform handle (for internal use)
     * @return Platform-specific native handle, or nullptr if invalid
     */
    void* getNativeHandle() const;
    
    // Non-copyable
    TextView(const TextView&) = delete;
    TextView& operator=(const TextView&) = delete;
    
    // Movable
    TextView(TextView&&) noexcept;
    TextView& operator=(TextView&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

