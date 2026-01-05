/**
 * Obsidian Router Implementation
 */

#include "obsidian/router.h"
#include "obsidian/window.h"
#include "obsidian/screen_container.h"
#include "obsidian/route_registry_helper.h"  // For processPendingRegistrations
#include "core/routing/route_scanner.h"
#include "core/routing/route_registry.h"
#include "core/routing/navigation_history.h"
#include "core/routing/route_matcher.h"
#include "core/routing/route_renderer.h"
#include <iostream>

#ifdef __APPLE__
#include "packages/apple/macos_router.h"
#endif

using namespace obsidian;
using namespace obsidian::routing;

// Forward declaration of global router instance
namespace obsidian {
    extern Router* g_router;
}

class Router::Impl {
public:
    RouteScanner scanner;
    RouteRegistry registry;
    NavigationHistory history;
    RouteRenderer renderer;
    Window* window = nullptr;
    std::unique_ptr<ScreenContainer> screenContainer;
    std::function<void(const std::string& path)> onNavigation;
    std::string currentPath;
    bool valid = false;
    
#ifdef __APPLE__
    ObsidianMacOSRouterHandle macosRouterHandle = nullptr;
#endif
};

Router::Router() : pImpl(std::make_unique<Impl>()) {}

Router::Router(Router&&) noexcept = default;
Router& Router::operator=(Router&&) noexcept = default;

Router::~Router() {
#ifdef __APPLE__
    if (pImpl->macosRouterHandle) {
        obsidian_macos_router_destroy(pImpl->macosRouterHandle);
        pImpl->macosRouterHandle = nullptr;
    }
#endif
}

bool Router::initialize(const std::string& appDirectory) {
    pImpl->valid = false;
    
    // Set global router instance for route registration
    g_router = this;
    
    // Process any pending route registrations that occurred during static initialization
    // This is CRITICAL: static initializers in route files run BEFORE main(), 
    // so g_router is null at that time. They queue their registrations,
    // and we process them here after g_router is set.
    processPendingRegistrations();
    
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
    
#ifdef __APPLE__
    // Notify macOS router integration
    if (pImpl->macosRouterHandle) {
        obsidian_macos_router_handle_navigation(pImpl->macosRouterHandle, routePath.c_str());
    }
#endif
    
    // Render route component
    if (pImpl->window) {
        pImpl->renderer.renderRoute(routeNode, *pImpl->window, pImpl->screenContainer.get(), *this, routeParams, query);
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
        
#ifdef __APPLE__
        // Notify macOS router integration
        if (pImpl->macosRouterHandle) {
            obsidian_macos_router_handle_navigation(pImpl->macosRouterHandle, entry->path.c_str());
        }
#endif
        
        // Render route component
        if (pImpl->window) {
            auto* routeNode = pImpl->registry.matchRoute(entry->path, const_cast<std::map<std::string, std::string>&>(entry->params));
            if (routeNode) {
                pImpl->renderer.renderRoute(routeNode, *pImpl->window, pImpl->screenContainer.get(), *this, entry->params, entry->query);
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
        
#ifdef __APPLE__
        // Notify macOS router integration
        if (pImpl->macosRouterHandle) {
            obsidian_macos_router_handle_navigation(pImpl->macosRouterHandle, entry->path.c_str());
        }
#endif
        
        // Render route component
        if (pImpl->window) {
            auto* routeNode = pImpl->registry.matchRoute(entry->path, const_cast<std::map<std::string, std::string>&>(entry->params));
            if (routeNode) {
                pImpl->renderer.renderRoute(routeNode, *pImpl->window, pImpl->screenContainer.get(), *this, entry->params, entry->query);
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
    
#ifdef __APPLE__
    // Notify macOS router integration
    if (pImpl->macosRouterHandle) {
        obsidian_macos_router_handle_navigation(pImpl->macosRouterHandle, routePath.c_str());
    }
#endif
    
    // Render route component
    if (pImpl->window) {
        pImpl->renderer.renderRoute(routeNode, *pImpl->window, pImpl->screenContainer.get(), *this, routeParams, query);
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
    
    // Initialize ScreenContainer and attach to window
    if (!pImpl->screenContainer) {
        pImpl->screenContainer = std::make_unique<ScreenContainer>();
        pImpl->screenContainer->attachToWindow(window);
    }
    
#ifdef __APPLE__
    // Initialize macOS router integration if not already done
    if (!pImpl->macosRouterHandle && window.getNativeHandle()) {
        ObsidianMacOSRouterParams params;
        params.routerHandle = this;
        params.windowHandle = window.getNativeHandle();
        
        pImpl->macosRouterHandle = obsidian_macos_router_create(params);
        
        if (pImpl->macosRouterHandle) {
            // Set up navigation callbacks
            obsidian_macos_router_setup_navigation_callbacks(pImpl->macosRouterHandle);
        }
    }
#endif
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
                          Screen* screen,
                          const std::string& path,
                          const std::map<std::string, std::string>& params,
                          const std::map<std::string, std::string>& query,
                          Router& router)
    : window_(window), screen_(screen), path_(path), params_(params), query_(query), router_(router) {}

Window& RouteContext::getWindow() const {
    return window_;
}

Screen* RouteContext::getScreen() const {
    return screen_;
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
