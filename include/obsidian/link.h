/**
 * Obsidian Public API - Link
 * 
 * Declarative navigation component for file-based routing.
 * Similar to Next.js Link or Expo Router Link.
 * 
 * Link wraps any child component and makes it clickable for navigation.
 * Supports Button, TextView, VStack, HStack, and other Obsydian components.
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
class TextView;
class VStack;
class HStack;
class ZStack;
class Spacer;

/**
 * Link class - Declarative navigation component
 * 
 * Provides a component that wraps any child component and navigates to a route when clicked.
 * Similar to Next.js <Link> or React Router <Link> - wraps children and makes them clickable.
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
     * Create a link with the given href and child component
     * @param href Route path to navigate to (e.g., "/about", "/users/123")
     * @param child Child component to wrap (Button, TextView, VStack, etc.)
     * @return true if link was created successfully
     */
    bool create(const std::string& href, Button& child);
    bool create(const std::string& href, TextView& child);
    bool create(const std::string& href, VStack& child);
    bool create(const std::string& href, HStack& child);
    // Note: ZStack support deferred until getNativeViewHandle() is added to ZStack API
    
    /**
     * Legacy API: Create a link with text (creates a Button internally)
     * @deprecated Use create(href, Button&) instead for better flexibility
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
     * Set the link text (only works with legacy text-based API)
     * @param text Text to display
     */
    void setText(const std::string& text);
    
    /**
     * Get the link text (only works with legacy text-based API)
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
