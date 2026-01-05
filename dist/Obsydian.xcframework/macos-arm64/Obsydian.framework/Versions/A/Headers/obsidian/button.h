/**
 * Obsidian Public API - Button
 * 
 * This is the public API header for button management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace obsidian {

// Forward declaration
class Window;

/**
 * Button class - Platform-agnostic button management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * buttons across all platforms. Platform-specific details are hidden.
 */
class Button {
public:
    /**
     * Create a new button
     */
    Button();
    
    /**
     * Destructor - automatically removes and destroys the button
     */
    ~Button();
    
    /**
     * Create a button with the given parameters
     * @param title Button text/title
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Button width in pixels
     * @param height Button height in pixels
     * @return true if button was created successfully
     */
    bool create(const std::string& title, int x, int y, int width, int height);
    
    /**
     * Set the button title/text
     */
    void setTitle(const std::string& title);
    
    /**
     * Get the button title/text
     */
    std::string getTitle() const;
    
    /**
     * Set the button click callback
     * The callback will be called when the button is clicked
     */
    void setOnClick(std::function<void()> callback);
    
    /**
     * Set button visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if button is visible
     */
    bool isVisible() const;
    
    /**
     * Set button enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if button is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add button to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove button from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if button is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle (for internal use by layout system)
     * @return Platform-specific native view handle, or nullptr if invalid
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;
    
    // Movable
    Button(Button&&) noexcept;
    Button& operator=(Button&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

