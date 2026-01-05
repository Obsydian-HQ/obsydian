/**
 * Obsidian Core - Route Registry Implementation
 */

#include "route_registry.h"
#include <algorithm>
#include <sstream>

namespace obsidian::routing {

class RouteRegistry::Impl {
public:
    std::unique_ptr<RouteNode> root;
    bool valid = false;
    
    Impl() : root(std::make_unique<RouteNode>()) {
        root->path = "";
        root->fullPath = "/";
    }
};

RouteRegistry::RouteRegistry() : pImpl(std::make_unique<Impl>()) {}

RouteRegistry::RouteRegistry(RouteRegistry&&) noexcept = default;
RouteRegistry& RouteRegistry::operator=(RouteRegistry&&) noexcept = default;

RouteRegistry::~RouteRegistry() = default;

bool RouteRegistry::buildFromScanner(const RouteScanner& scanner) {
    if (!scanner.isValid()) {
        return false;
    }
    
    // Reset registry
    pImpl->root = std::make_unique<RouteNode>();
    pImpl->root->path = "";
    pImpl->root->fullPath = "/";
    pImpl->valid = false;
    
    // First pass: Add all layout files
    const auto& routeFiles = scanner.getRouteFiles();
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::Layout) {
            addRouteFile(routeFile);
        }
    }
    
    // Second pass: Add all route files (non-layout)
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type != RouteFileType::Layout) {
            addRouteFile(routeFile);
        }
    }
    
    pImpl->valid = true;
    return true;
}

void RouteRegistry::addRouteFile(const RouteFile& routeFile) {
    std::string routePath = routeFile.routePath;
    
    // Remove leading slash
    if (routePath.front() == '/') {
        routePath = routePath.substr(1);
    }
    
    // Split path into segments
    std::vector<std::string> segments = splitPath(routePath);
    
    // Navigate/create nodes for each segment
    RouteNode* current = pImpl->root.get();
    
    for (size_t i = 0; i < segments.size(); ++i) {
        const std::string& segment = segments[i];
        bool isLast = (i == segments.size() - 1);
        
        // Find or create child node
        RouteNode* child = findOrCreateNode(current, segment);
        
        if (isLast && routeFile.type == RouteFileType::Layout) {
            // This is a layout file - mark the child node (not current)
            child->hasLayout = true;
            child->layoutFile = routeFile;
        } else if (isLast) {
            // Set route file for the final node
            child->routeFile = routeFile;
            child->fullPath = routeFile.routePath;
        }
        
        current = child;
    }
    
    // Handle root layout (routePath == "/" or empty)
    if (segments.empty() && routeFile.type == RouteFileType::Layout) {
        current->hasLayout = true;
        current->layoutFile = routeFile;
    }
}

RouteNode* RouteRegistry::findOrCreateNode(RouteNode* parent, const std::string& segment) {
    // Check if node already exists
    for (auto& child : parent->children) {
        if (child->path == segment) {
            return child.get();
        }
    }
    
    // Create new node
    auto newNode = std::make_unique<RouteNode>(segment);
    newNode->parent = parent;
    RouteNode* nodePtr = newNode.get();
    parent->children.push_back(std::move(newNode));
    
    return nodePtr;
}

std::vector<std::string> RouteRegistry::splitPath(const std::string& path) const {
    std::vector<std::string> segments;
    
    if (path.empty()) {
        return segments;
    }
    
    std::istringstream iss(path);
    std::string segment;
    
    while (std::getline(iss, segment, '/')) {
        if (!segment.empty()) {
            segments.push_back(segment);
        }
    }
    
    return segments;
}

RouteNode* RouteRegistry::findRoute(const std::string& path) const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
    std::map<std::string, std::string> params;
    return matchRoute(path, params);
}

RouteNode* RouteRegistry::matchRoute(const std::string& path, std::map<std::string, std::string>& params) const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
    params.clear();
    
    // Normalize path
    std::string normalizedPath = path;
    if (normalizedPath.empty() || normalizedPath.front() != '/') {
        normalizedPath = "/" + normalizedPath;
    }
    
    // Remove leading slash and split into segments
    if (normalizedPath.front() == '/') {
        normalizedPath = normalizedPath.substr(1);
    }
    
    std::vector<std::string> segments = splitPath(normalizedPath);
    
    // Start matching from root
    return matchRouteRecursive(pImpl->root.get(), segments, params);
}

RouteNode* RouteRegistry::matchRouteRecursive(RouteNode* node,
                                               const std::vector<std::string>& segments,
                                               std::map<std::string, std::string>& params) const {
    // If no more segments, check if current node has a route
    if (segments.empty()) {
        if (!node->routeFile.filePath.empty() || node->hasLayout) {
            return node;
        }
        return nullptr;
    }
    
    const std::string& currentSegment = segments[0];
    std::vector<std::string> remainingSegments(segments.begin() + 1, segments.end());
    
    // Try to match with children
    for (auto& child : node->children) {
        // Exact match
        if (child->path == currentSegment) {
            RouteNode* result = matchRouteRecursive(child.get(), remainingSegments, params);
            if (result) {
                return result;
            }
        }
        // Dynamic parameter match (:param)
        else if (child->path.front() == ':' && child->path.length() > 1) {
            std::string paramName = child->path.substr(1);
            params[paramName] = currentSegment;
            RouteNode* result = matchRouteRecursive(child.get(), remainingSegments, params);
            if (result) {
                return result;
            }
            // Backtrack: remove parameter if match failed
            params.erase(paramName);
        }
        // Catch-all match (*)
        else if (child->path == "*") {
            // Catch-all matches everything remaining
            if (!child->routeFile.filePath.empty()) {
                return child.get();
            }
        }
    }
    
    return nullptr;
}

std::vector<std::string> RouteRegistry::getAllRoutes() const {
    std::vector<std::string> routes;
    
    if (!pImpl->valid) {
        return routes;
    }
    
    collectRoutes(pImpl->root.get(), routes, "");
    return routes;
}

void RouteRegistry::collectRoutes(const RouteNode* node, std::vector<std::string>& routes, std::string currentPath) const {
    // Add current route if it has a route file
    if (!node->routeFile.filePath.empty()) {
        routes.push_back(node->fullPath.empty() ? "/" : node->fullPath);
    }
    
    // Recursively collect from children
    for (const auto& child : node->children) {
        std::string childPath = currentPath;
        if (!childPath.empty() && !childPath.ends_with("/")) {
            childPath += "/";
        }
        childPath += child->path;
        
        collectRoutes(child.get(), routes, childPath);
    }
}

bool RouteRegistry::routeExists(const std::string& path) const {
    return findRoute(path) != nullptr;
}

const RouteNode* RouteRegistry::getRoot() const {
    return pImpl->valid ? pImpl->root.get() : nullptr;
}

bool RouteRegistry::isValid() const {
    return pImpl->valid;
}

} // namespace obsidian::routing
