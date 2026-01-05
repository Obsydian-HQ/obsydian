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
 * Toolbar style enumeration (macOS 11+)
 */
enum class ToolbarStyle {
    Automatic = 0,      // Automatic based on window
    Expanded = 1,       // Expanded toolbar
    Preference = 2,     // Preference window style
    Unified = 3,        // Unified with title bar
    UnifiedCompact = 4  // Compact unified with title bar (best for sidebars)
};

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
     * Set up a toolbar with the native sidebar toggle button.
     * This is REQUIRED for the native macOS sidebar collapse button.
     * On macOS: Creates an NSToolbar with NSToolbarToggleSidebarItemIdentifier
     * @return true if toolbar was created successfully
     */
    bool setupToolbarForSidebar();
    
    /**
     * Check if the window has a toolbar
     * @return true if a toolbar is configured
     */
    bool hasToolbar() const;
    
    /**
     * Set the window's toolbar style (macOS 11+)
     * For proper sidebar integration, use UnifiedCompact or Unified.
     * @param style The toolbar style to set
     */
    void setToolbarStyle(ToolbarStyle style);
    
    /**
     * Get the window's toolbar style
     * @return Current toolbar style
     */
    ToolbarStyle getToolbarStyle() const;
    
    /**
     * Get the native platform handle (for internal use)
     * @return Platform-specific native handle, or nullptr if invalid
     */
    void* getNativeHandle() const;
    
    /**
     * Set window content (convenience method)
     * This is a convenience method that calls addToWindow on the component
     * @tparam T Component type (VStack, HStack, etc.)
     * @param component Component to set as window content
     */
    template<typename T>
    void setContent(T& component) {
        component.addToWindow(*this);
    }
    
    /**
     * Clear all content from the window
     * This removes all subviews from the content view.
     * Used by the router to clear old content before rendering new routes.
     */
    void clearContent();
    
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

