/**
 * Obsidian Public API - Link
 * 
 * Declarative navigation component for file-based routing.
 * Similar to Next.js Link or Expo Router Link.
 * 
 * Link wraps a Button internally and handles navigation when clicked.
 */

#pragma once

#include <string>
#include <functional>
#include <memory>

namespace obsidian {

// Forward declarations
class Window;
class Router;
class Button;

/**
 * Link class - Declarative navigation component
 * 
 * Provides a component that navigates to a route when clicked.
 * Internally uses a Button for rendering and click handling.
 */
class Link {
public:
    /**
     * Create a new link
     */
    Link();
    
    /**
     * Destructor - automatically removes and destroys the link
     */
    ~Link();
    
    /**
     * Create a link with the given href and button text
     * @param href Route path to navigate to (e.g., "/about", "/users/123")
     * @param text Text to display on the link button
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Link width in pixels
     * @param height Link height in pixels
     * @return true if link was created successfully
     */
    bool create(const std::string& href, const std::string& text, int x, int y, int width, int height);
    
    /**
     * Set the href (route path) for navigation
     * @param href Route path to navigate to
     */
    void setHref(const std::string& href);
    
    /**
     * Get the current href
     * @return Current href
     */
    std::string getHref() const;
    
    /**
     * Set the link text
     * @param text Text to display
     */
    void setText(const std::string& text);
    
    /**
     * Get the link text
     * @return Current text
     */
    std::string getText() const;
    
    /**
     * Set the click callback (optional, navigation happens automatically)
     * This callback is called before navigation occurs
     */
    void setOnClick(std::function<void()> callback);
    
    /**
     * Add link to a window's content view
     * @param window Window to add link to
     */
    void addToWindow(Window& window);
    
    /**
     * Remove link from its parent view
     */
    void removeFromParent();
    
    /**
     * Set the router instance for navigation
     * If not set, Link will try to find the global router instance
     * @param router Router instance to use for navigation
     */
    void setRouter(Router* router);
    
    /**
     * Set link visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if link is visible
     */
    bool isVisible() const;
    
    /**
     * Set link enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if link is enabled
     */
    bool isEnabled() const;
    
    /**
     * Check if link is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle (for internal use by layout system)
     * @return Platform-specific native view handle, or nullptr if invalid
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    Link(const Link&) = delete;
    Link& operator=(const Link&) = delete;
    
    // Movable
    Link(Link&&) noexcept;
    Link& operator=(Link&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
