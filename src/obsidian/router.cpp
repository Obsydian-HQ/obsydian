/**
 * Obsidian Router Implementation
 */

#include "obsidian/router.h"
#include "obsidian/window.h"
#include "core/routing/route_scanner.h"
#include "core/routing/route_registry.h"
#include "core/routing/navigation_history.h"
#include "core/routing/route_matcher.h"
#include "core/routing/route_renderer.h"
#include <iostream>

using namespace obsidian;
using namespace obsidian::routing;

class Router::Impl {
public:
    RouteScanner scanner;
    RouteRegistry registry;
    NavigationHistory history;
    RouteRenderer renderer;
    Window* window = nullptr;
    std::function<void(const std::string& path)> onNavigation;
    std::string currentPath;
    bool valid = false;
};

Router::Router() : pImpl(std::make_unique<Impl>()) {}

Router::Router(Router&&) noexcept = default;
Router& Router::operator=(Router&&) noexcept = default;

Router::~Router() = default;

bool Router::initialize(const std::string& appDirectory) {
    pImpl->valid = false;
    
    // Set global router instance for route registration
    g_router = this;
    
    // Scan app directory
    if (!pImpl->scanner.scan(appDirectory)) {
        std::cerr << "Router: Failed to scan app directory: " << appDirectory << std::endl;
        g_router = nullptr;
        return false;
    }
    
    // Build route registry
    if (!pImpl->registry.buildFromScanner(pImpl->scanner)) {
        std::cerr << "Router: Failed to build route registry" << std::endl;
        g_router = nullptr;
        return false;
    }
    
    pImpl->valid = true;
    return true;
}

void Router::navigate(const std::string& path) {
    navigate(path, {});
}

void Router::navigate(const std::string& path, const std::map<std::string, std::string>& params) {
    if (!pImpl->valid) {
        std::cerr << "Router: Cannot navigate, router not initialized" << std::endl;
        return;
    }
    
    // Parse URL to extract path and query parameters
    std::string routePath;
    std::map<std::string, std::string> query;
    RouteMatcher::parseUrl(path, routePath, query);
    
    // Match route
    std::map<std::string, std::string> routeParams = params;
    auto* routeNode = pImpl->registry.matchRoute(routePath, routeParams);
    
    if (!routeNode) {
        std::cerr << "Router: Route not found: " << routePath << std::endl;
        return;
    }
    
    // Merge route params with provided params
    for (const auto& param : params) {
        routeParams[param.first] = param.second;
    }
    
    // Add to history
    pImpl->history.push(routePath, routeParams, query);
    pImpl->currentPath = routePath;
    
    // Call navigation callback
    if (pImpl->onNavigation) {
        pImpl->onNavigation(routePath);
    }
    
    // Render route component
    if (pImpl->window) {
        pImpl->renderer.renderRoute(routeNode, *pImpl->window, *this, routeParams, query);
    }
}

void Router::goBack() {
    if (!pImpl->valid) {
        return;
    }
    
    const auto* entry = pImpl->history.goBack();
    if (entry) {
        pImpl->currentPath = entry->path;
        
        if (pImpl->onNavigation) {
            pImpl->onNavigation(entry->path);
        }
        
        // Render route component
        if (pImpl->window) {
            auto* routeNode = pImpl->registry.matchRoute(entry->path, const_cast<std::map<std::string, std::string>&>(entry->params));
            if (routeNode) {
                pImpl->renderer.renderRoute(routeNode, *pImpl->window, *this, entry->params, entry->query);
            }
        }
    }
}

void Router::goForward() {
    if (!pImpl->valid) {
        return;
    }
    
    const auto* entry = pImpl->history.goForward();
    if (entry) {
        pImpl->currentPath = entry->path;
        
        if (pImpl->onNavigation) {
            pImpl->onNavigation(entry->path);
        }
        
        // Render route component
        if (pImpl->window) {
            auto* routeNode = pImpl->registry.matchRoute(entry->path, const_cast<std::map<std::string, std::string>&>(entry->params));
            if (routeNode) {
                pImpl->renderer.renderRoute(routeNode, *pImpl->window, *this, entry->params, entry->query);
            }
        }
    }
}

void Router::replace(const std::string& path) {
    if (!pImpl->valid) {
        return;
    }
    
    // Parse URL
    std::string routePath;
    std::map<std::string, std::string> query;
    RouteMatcher::parseUrl(path, routePath, query);
    
    // Match route
    std::map<std::string, std::string> routeParams;
    auto* routeNode = pImpl->registry.matchRoute(routePath, routeParams);
    
    if (!routeNode) {
        std::cerr << "Router: Route not found: " << routePath << std::endl;
        return;
    }
    
    // Replace current history entry
    pImpl->history.replace(routePath, routeParams, query);
    pImpl->currentPath = routePath;
    
    if (pImpl->onNavigation) {
        pImpl->onNavigation(routePath);
    }
    
    // Render route component
    if (pImpl->window) {
        pImpl->renderer.renderRoute(routeNode, *pImpl->window, *this, routeParams, query);
    }
}

void Router::push(const std::string& path) {
    navigate(path);
}

std::string Router::getCurrentPath() const {
    if (!pImpl->valid) {
        return "";
    }
    
    const auto* current = pImpl->history.getCurrent();
    if (current) {
        return current->path;
    }
    
    return pImpl->currentPath;
}

bool Router::canGoBack() const {
    if (!pImpl->valid) {
        return false;
    }
    return pImpl->history.canGoBack();
}

bool Router::canGoForward() const {
    if (!pImpl->valid) {
        return false;
    }
    return pImpl->history.canGoForward();
}

void Router::clearHistory() {
    if (!pImpl->valid) {
        return;
    }
    pImpl->history.clear();
    pImpl->currentPath = "";
}

int Router::getHistorySize() const {
    if (!pImpl->valid) {
        return 0;
    }
    return static_cast<int>(pImpl->history.size());
}

void Router::setWindow(Window& window) {
    pImpl->window = &window;
}

void Router::setOnNavigation(std::function<void(const std::string& path)> callback) {
    pImpl->onNavigation = callback;
}

bool Router::routeExists(const std::string& path) const {
    if (!pImpl->valid) {
        return false;
    }
    
    // Parse URL to get path without query
    std::string routePath;
    std::map<std::string, std::string> query;
    RouteMatcher::parseUrl(path, routePath, query);
    
    return pImpl->registry.routeExists(routePath);
}

std::vector<std::string> Router::getAllRoutes() const {
    if (!pImpl->valid) {
        return {};
    }
    return pImpl->registry.getAllRoutes();
}

bool Router::isValid() const {
    return pImpl->valid;
}

// Global router instance for route registration
namespace obsidian {
    Router* g_router = nullptr;
}

void Router::registerRouteComponent(const std::string& routePath, 
                                    std::function<void(RouteContext&)> componentFunction) {
    pImpl->renderer.registerRouteComponent(routePath, componentFunction);
}

void Router::registerLayoutComponent(const std::string& routePath,
                                     std::function<void(RouteContext&, std::function<void()>)> layoutFunction) {
    pImpl->renderer.registerLayoutComponent(routePath, layoutFunction);
}

// RouteContext implementation
RouteContext::RouteContext(Window& window,
                          const std::string& path,
                          const std::map<std::string, std::string>& params,
                          const std::map<std::string, std::string>& query,
                          Router& router)
    : window_(window), path_(path), params_(params), query_(query), router_(router) {}

Window& RouteContext::getWindow() const {
    return window_;
}

const std::string& RouteContext::getPath() const {
    return path_;
}

const std::map<std::string, std::string>& RouteContext::getParams() const {
    return params_;
}

const std::map<std::string, std::string>& RouteContext::getQuery() const {
    return query_;
}

void RouteContext::navigate(const std::string& path) {
    router_.navigate(path);
}

void RouteContext::goBack() {
    router_.goBack();
}

void RouteContext::goForward() {
    router_.goForward();
}

bool RouteContext::canGoBack() const {
    return router_.canGoBack();
}

bool RouteContext::canGoForward() const {
    return router_.canGoForward();
}
