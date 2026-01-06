/**
 * Obsidian Core - Route Renderer
 * 
 * Handles rendering of route components and layout nesting.
 * Uses a function pointer registry to call route component functions.
 */

#pragma once

#include "route_registry.h"
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace obsidian {
    class Window;
    class RouteContext;
    class Router;
    class ScreenContainer;
    class Screen;
}

namespace obsidian::routing {

/**
 * Route component function type
 * Route files should define functions with this signature
 */
using RouteComponentFunction = std::function<void(obsidian::RouteContext&)>;

/**
 * Layout component function type
 * Layout files should define functions with this signature.
 * The function receives a RouteContext and a renderChild callback.
 */
using LayoutComponentFunction = std::function<void(obsidian::RouteContext&, std::function<void()>)>;

/**
 * Route Renderer
 * 
 * Manages route component rendering, including layout nesting.
 */
class RouteRenderer {
public:
    RouteRenderer();
    ~RouteRenderer();
    
    /**
     * Register a route component function
     * @param routePath Route path (e.g., "/about", "/users/:id")
     * @param componentFunction Function to call when rendering this route
     */
    void registerRouteComponent(const std::string& routePath, RouteComponentFunction componentFunction);
    
    /**
     * Register a layout component function
     * @param routePath Route path for the layout (e.g., "/", "/users")
     * @param layoutFunction Function to call when rendering this layout
     */
    void registerLayoutComponent(const std::string& routePath, LayoutComponentFunction layoutFunction);
    
    /**
     * Render a route
     * @param routeNode Route node to render
     * @param window Window to render into
     * @param screenContainer Screen container for managing route views
     * @param router Router instance for navigation
     * @param params Route parameters
     * @param query Query parameters
     * @return true if rendering was successful
     */
    bool renderRoute(const RouteNode* routeNode,
                     obsidian::Window& window,
                     obsidian::ScreenContainer* screenContainer,
                     obsidian::Router& router,
                     const std::map<std::string, std::string>& params,
                     const std::map<std::string, std::string>& query);
    
    /**
     * Check if a route component is registered
     * @param routePath Route path
     * @return true if component is registered
     */
    bool isRouteComponentRegistered(const std::string& routePath) const;
    
    /**
     * Check if a layout component is registered
     * @param routePath Route path
     * @return true if layout is registered
     */
    bool isLayoutComponentRegistered(const std::string& routePath) const;
    
    /**
     * Clear all registered components (for testing)
     */
    void clear();
    
    // Non-copyable
    RouteRenderer(const RouteRenderer&) = delete;
    RouteRenderer& operator=(const RouteRenderer&) = delete;
    
    // Movable
    RouteRenderer(RouteRenderer&&) noexcept;
    RouteRenderer& operator=(RouteRenderer&&) noexcept;

private:
    /**
     * Render route with layout nesting
     * @param routeNode Route node to render
     * @param window Window to render into
     * @param screen Screen to render into (owned by ScreenContainer)
     * @param router Router instance
     * @param params Route parameters
     * @param query Query parameters
     * @param currentPath Current path being built for layout lookup
     * @return true if rendering was successful
     */
    bool renderRouteWithLayouts(const RouteNode* routeNode,
                                obsidian::Window& window,
                                obsidian::Screen* screen,
                                obsidian::Router& router,
                                const std::map<std::string, std::string>& params,
                                const std::map<std::string, std::string>& query,
                                const std::string& currentPath);
    
    /**
     * Build path string from route node hierarchy
     * @param routeNode Route node
     * @return Path string
     */
    std::string buildPathFromNode(const RouteNode* routeNode) const;
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian::routing
