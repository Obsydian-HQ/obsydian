/**
 * Obsidian Public API - TextField
 * 
 * This is the public API header for text field management.
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
 * TextField class - Platform-agnostic text field management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * single-line text input fields across all platforms. Platform-specific details are hidden.
 */
class TextField {
public:
    /**
     * Create a new text field
     */
    TextField();
    
    /**
     * Destructor - automatically removes and destroys the text field
     */
    ~TextField();
    
    /**
     * Create a text field with the given parameters
     * @param placeholder Placeholder text to display when empty
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Text field width in pixels
     * @param height Text field height in pixels
     * @return true if text field was created successfully
     */
    bool create(const std::string& placeholder, int x, int y, int width, int height);
    
    /**
     * Set the text field text
     */
    void setText(const std::string& text);
    
    /**
     * Get the text field text
     */
    std::string getText() const;
    
    /**
     * Set the text field placeholder text
     */
    void setPlaceholder(const std::string& placeholder);
    
    /**
     * Set the text field enter key callback
     * The callback will be called when the user presses Enter/Return
     */
    void setOnEnter(std::function<void()> callback);
    
    /**
     * Set text field visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if text field is visible
     */
    bool isVisible() const;
    
    /**
     * Set text field enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if text field is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add text field to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove text field from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if text field is valid
     */
    bool isValid() const;
    
    // Non-copyable
    TextField(const TextField&) = delete;
    TextField& operator=(const TextField&) = delete;
    
    // Movable
    TextField(TextField&&) noexcept;
    TextField& operator=(TextField&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

