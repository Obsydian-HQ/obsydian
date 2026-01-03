/**
 * Obsidian Public API - Window
 * 
 * This is the public API header for window management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace obsidian {

/**
 * Window class - Platform-agnostic window management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * windows across all platforms. Platform-specific details are hidden.
 */
class Window {
public:
    /**
     * Create a new window
     */
    Window();
    
    /**
     * Destructor - automatically closes the window
     */
    ~Window();
    
    /**
     * Create a window with the given parameters
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param title Window title
     * @return true if window was created successfully
     */
    bool create(int width, int height, const std::string& title);
    
    /**
     * Show the window
     */
    void show();
    
    /**
     * Close the window
     */
    void close();
    
    /**
     * Check if window is valid and open
     */
    bool isValid() const;
    
    /**
     * Get the native platform handle (for internal use)
     * @return Platform-specific native handle, or nullptr if invalid
     */
    void* getNativeHandle() const;
    
    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // Movable
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

