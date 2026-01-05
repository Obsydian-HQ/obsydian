/**
 * Obsidian Core - Route Registry
 * 
 * Builds and maintains a route tree structure from scanned route files.
 * Provides fast route matching and lookup capabilities.
 */

#pragma once

#include "route_scanner.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

namespace obsidian::routing {

/**
 * Route node in the route tree
 */
struct RouteNode {
    std::string path;                    // Route path segment (e.g., "about", ":id", "*")
    std::string fullPath;                // Full route path (e.g., "/about", "/users/:id")
    RouteFile routeFile;                 // Associated route file
    std::vector<std::unique_ptr<RouteNode>> children; // Child routes
    RouteNode* parent = nullptr;         // Parent route node
    bool hasLayout = false;              // True if this node has a layout
    RouteFile layoutFile;                // Layout file if hasLayout is true
    
    RouteNode() = default;
    RouteNode(const std::string& p) : path(p) {}
};

/**
 * Route Registry
 * 
 * Builds and maintains a route tree from scanned route files.
 * Provides route matching and lookup functionality.
 */
class RouteRegistry {
public:
    RouteRegistry();
    ~RouteRegistry();
    
    /**
     * Build route registry from route scanner results
     * @param scanner Route scanner that has scanned the app directory
     * @return true if registry was built successfully
     */
    bool buildFromScanner(const RouteScanner& scanner);
    
    /**
     * Find a route node matching the given path
     * @param path Route path (e.g., "/about", "/users/123")
     * @return Route node if found, nullptr otherwise
     */
    RouteNode* findRoute(const std::string& path) const;
    
    /**
     * Match a route path and extract parameters
     * @param path Route path to match (e.g., "/users/123")
     * @param params Output map of extracted parameters
     * @return Route node if matched, nullptr otherwise
     */
    RouteNode* matchRoute(const std::string& path, std::map<std::string, std::string>& params) const;
    
    /**
     * Get all routes in the registry
     * @return Vector of all route paths
     */
    std::vector<std::string> getAllRoutes() const;
    
    /**
     * Check if a route exists
     * @param path Route path
     * @return true if route exists
     */
    bool routeExists(const std::string& path) const;
    
    /**
     * Get the root route node
     * @return Root route node
     */
    const RouteNode* getRoot() const;
    
    /**
     * Check if registry is valid
     */
    bool isValid() const;
    
    // Non-copyable
    RouteRegistry(const RouteRegistry&) = delete;
    RouteRegistry& operator=(const RouteRegistry&) = delete;
    
    // Movable
    RouteRegistry(RouteRegistry&&) noexcept;
    RouteRegistry& operator=(RouteRegistry&&) noexcept;

private:
    /**
     * Add a route file to the registry
     * @param routeFile Route file to add
     */
    void addRouteFile(const RouteFile& routeFile);
    
    /**
     * Find or create a node for a given path segment
     * @param parent Parent node
     * @param segment Path segment (e.g., "users", ":id", "*")
     * @return Route node for the segment
     */
    RouteNode* findOrCreateNode(RouteNode* parent, const std::string& segment);
    
    /**
     * Split route path into segments
     * @param path Route path (e.g., "/users/:id")
     * @return Vector of path segments
     */
    std::vector<std::string> splitPath(const std::string& path) const;
    
    /**
     * Match a route node recursively
     * @param node Current node to match
     * @param segments Remaining path segments to match
     * @param params Output map of extracted parameters
     * @return Route node if matched, nullptr otherwise
     */
    RouteNode* matchRouteRecursive(RouteNode* node, 
                                   const std::vector<std::string>& segments,
                                   std::map<std::string, std::string>& params) const;
    
    /**
     * Collect all routes from the tree
     * @param node Current node
     * @param routes Output vector of routes
     * @param currentPath Current path being built
     */
    void collectRoutes(const RouteNode* node, std::vector<std::string>& routes, std::string currentPath) const;
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian::routing
