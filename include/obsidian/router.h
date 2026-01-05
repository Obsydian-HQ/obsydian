/**
 * Obsidian Public API - Router
 * 
 * File-based routing system for Obsydian applications.
 * Provides navigation and route management similar to Next.js App Router.
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace obsidian {

// Forward declarations
class Window;
class RouteContext;

/**
 * Router class
 * 
 * Manages file-based routing, navigation history, and route matching.
 * Scans the app/ directory at initialization to discover routes.
 */
class Router {
public:
    Router();
    ~Router();
    
    /**
     * Initialize the router by scanning the app directory
     * @param appDirectory Path to the app/ directory (default: "app")
     * @return true if initialization was successful
     */
    bool initialize(const std::string& appDirectory = "app");
    
    /**
     * Navigate to a route
     * @param path Route path (e.g., "/about", "/users/123")
     */
    void navigate(const std::string& path);
    
    /**
     * Navigate to a route with parameters
     * @param path Route path
     * @param params Route parameters (for dynamic routes)
     */
    void navigate(const std::string& path, const std::map<std::string, std::string>& params);
    
    /**
     * Navigate back in history
     */
    void goBack();
    
    /**
     * Navigate forward in history
     */
    void goForward();
    
    /**
     * Replace current route (doesn't add to history)
     * @param path Route path
     */
    void replace(const std::string& path);
    
    /**
     * Push a new route onto history stack
     * @param path Route path
     */
    void push(const std::string& path);
    
    /**
     * Get the current route path
     * @return Current path
     */
    std::string getCurrentPath() const;
    
    /**
     * Check if back navigation is possible
     */
    bool canGoBack() const;
    
    /**
     * Check if forward navigation is possible
     */
    bool canGoForward() const;
    
    /**
     * Clear navigation history
     */
    void clearHistory();
    
    /**
     * Get the number of entries in history
     */
    int getHistorySize() const;
    
    /**
     * Set the window for route rendering
     * @param window Window reference
     */
    void setWindow(Window& window);
    
    /**
     * Set callback for navigation events
     * @param callback Function called when navigation occurs
     */
    void setOnNavigation(std::function<void(const std::string& path)> callback);
    
    /**
     * Check if a route exists
     * @param path Route path
     * @return true if route exists
     */
    bool routeExists(const std::string& path) const;
    
    /**
     * Get all available routes
     * @return Vector of all route paths
     */
    std::vector<std::string> getAllRoutes() const;
    
    /**
     * Check if router is valid and initialized
     */
    bool isValid() const;
    
    /**
     * Register a route component function
     * Route files should call this to register their component functions
     * @param routePath Route path (e.g., "/about", "/users/:id")
     * @param componentFunction Function to call when rendering this route
     */
    void registerRouteComponent(const std::string& routePath, 
                                std::function<void(RouteContext&)> componentFunction);
    
    /**
     * Register a layout component function
     * Layout files should call this to register their layout functions
     * @param routePath Route path for the layout (e.g., "/", "/users")
     * @param layoutFunction Function to call when rendering this layout.
     *                       The function receives a RouteContext and a renderChild callback.
     */
    void registerLayoutComponent(const std::string& routePath,
                                  std::function<void(RouteContext&, std::function<void()>)> layoutFunction);
    
    // Non-copyable
    Router(const Router&) = delete;
    Router& operator=(Router&) = delete;
    
    // Movable
    Router(Router&&) noexcept;
    Router& operator=(Router&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Route Context
 * 
 * Provides route information and navigation methods to route components.
 */
class RouteContext {
public:
    RouteContext(Window& window, 
                const std::string& path,
                const std::map<std::string, std::string>& params,
                const std::map<std::string, std::string>& query,
                Router& router);
    
    /**
     * Get the window for rendering
     */
    Window& getWindow() const;
    
    /**
     * Get the current route path
     */
    const std::string& getPath() const;
    
    /**
     * Get route parameters (for dynamic routes)
     */
    const std::map<std::string, std::string>& getParams() const;
    
    /**
     * Get query parameters
     */
    const std::map<std::string, std::string>& getQuery() const;
    
    /**
     * Navigate to a new route
     */
    void navigate(const std::string& path);
    
    /**
     * Navigate back
     */
    void goBack();
    
    /**
     * Navigate forward
     */
    void goForward();
    
    /**
     * Check if back navigation is possible
     */
    bool canGoBack() const;
    
    /**
     * Check if forward navigation is possible
     */
    bool canGoForward() const;

private:
    Window& window_;
    std::string path_;
    std::map<std::string, std::string> params_;
    std::map<std::string, std::string> query_;
    Router& router_;
};

} // namespace obsidian
