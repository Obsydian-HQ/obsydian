/**
 * Obsidian Route Registry Helper
 * 
 * Provides convenient macros and functions for registering route components.
 * Route files should include this header and use the registration macros.
 * 
 * This system handles the C++ static initialization order problem:
 * - Route files use static initializers (run BEFORE main())
 * - The Router is created in main() (AFTER static initializers)
 * - Solution: Queue registrations when g_router is null, process them when Router initializes
 */

#pragma once

#include "obsidian/router.h"
#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace obsidian {

/**
 * Global router instance (set by Router::initialize())
 * Route files use this to register themselves
 */
extern Router* g_router;

/**
 * Pending route registration (deferred until Router is ready)
 */
struct PendingRouteRegistration {
    std::string path;
    std::function<void(RouteContext&)> component;
};

/**
 * Pending layout registration (deferred until Router is ready)
 */
struct PendingLayoutRegistration {
    std::string path;
    std::function<void(RouteContext&, std::function<void()>)> layout;
};

/**
 * Get the pending route registrations queue.
 * Defined in route_registry_helper.cpp to ensure single definition across all translation units.
 */
std::vector<PendingRouteRegistration>& getPendingRoutes();

/**
 * Get the pending layout registrations queue.
 * Defined in route_registry_helper.cpp to ensure single definition across all translation units.
 */
std::vector<PendingLayoutRegistration>& getPendingLayouts();

/**
 * Register a route component.
 * If g_router is available, registers immediately.
 * Otherwise, queues for deferred registration.
 */
inline void registerRoute(const std::string& path, std::function<void(RouteContext&)> component) {
    if (g_router) {
        g_router->registerRouteComponent(path, component);
    } else {
        // Queue for later when Router is initialized
        getPendingRoutes().push_back({path, component});
    }
}

/**
 * Register a layout component.
 * If g_router is available, registers immediately.
 * Otherwise, queues for deferred registration.
 */
inline void registerLayout(const std::string& path, 
                          std::function<void(RouteContext&, std::function<void()>)> layout) {
    if (g_router) {
        g_router->registerLayoutComponent(path, layout);
    } else {
        // Queue for later when Router is initialized
        getPendingLayouts().push_back({path, layout});
    }
}

/**
 * Process all pending route and layout registrations.
 * Called by Router::initialize() after setting g_router.
 */
inline void processPendingRegistrations() {
    if (!g_router) {
        std::cerr << "[RegistryHelper] processPendingRegistrations: g_router is null!" << std::endl;
        return;
    }
    
    // Process pending route registrations
    auto& pendingRoutes = getPendingRoutes();
    std::cerr << "[RegistryHelper] Processing " << pendingRoutes.size() << " pending routes" << std::endl;
    for (const auto& pending : pendingRoutes) {
        std::cerr << "[RegistryHelper] Registering route: " << pending.path << std::endl;
        g_router->registerRouteComponent(pending.path, pending.component);
    }
    pendingRoutes.clear();
    
    // Process pending layout registrations
    auto& pendingLayouts = getPendingLayouts();
    std::cerr << "[RegistryHelper] Processing " << pendingLayouts.size() << " pending layouts" << std::endl;
    for (const auto& pending : pendingLayouts) {
        std::cerr << "[RegistryHelper] Registering layout: " << pending.path << std::endl;
        g_router->registerLayoutComponent(pending.path, pending.layout);
    }
    pendingLayouts.clear();
}

/**
 * Macro to register a route component
 * 
 * Usage:
 * ```cpp
 * void renderRoute(RouteContext& ctx) { ... }
 * REGISTER_ROUTE("/about", renderRoute);
 * ```
 * 
 * This registers at static initialization time. If the Router isn't ready yet,
 * the registration is queued and processed when Router::initialize() is called.
 */
#define REGISTER_ROUTE(path, component) \
    static bool _registered_route_##component = []() { \
        ::obsidian::registerRoute(path, component); \
        return true; \
    }()

/**
 * Macro to register a layout component
 * 
 * Usage:
 * ```cpp
 * void renderLayout(RouteContext& ctx, std::function<void()> renderChild) { ... }
 * REGISTER_LAYOUT("/", renderLayout);
 * ```
 * 
 * This registers at static initialization time. If the Router isn't ready yet,
 * the registration is queued and processed when Router::initialize() is called.
 */
#define REGISTER_LAYOUT(path, layout) \
    static bool _registered_layout_##layout = []() { \
        ::obsidian::registerLayout(path, layout); \
        return true; \
    }()

} // namespace obsidian
