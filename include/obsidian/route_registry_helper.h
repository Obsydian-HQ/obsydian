/**
 * Obsidian Route Registry Helper
 * 
 * Provides convenient macros and functions for registering route components.
 * Route files should include this header and use the registration macros.
 */

#pragma once

#include "obsidian/router.h"
#include <functional>

namespace obsidian {

/**
 * Global router instance (set by application)
 * Route files use this to register themselves
 */
extern Router* g_router;

/**
 * Register a route component
 * 
 * Usage in route files:
 * ```cpp
 * #include <obsidian/route_registry_helper.h>
 * 
 * void renderRoute(RouteContext& ctx) {
 *     // Route implementation
 * }
 * 
 * // Register at static initialization time
 * static bool _registered = []() {
 *     if (g_router) {
 *         g_router->registerRouteComponent("/about", renderRoute);
 *     }
 *     return true;
 * }();
 * ```
 */
inline void registerRoute(const std::string& path, std::function<void(RouteContext&)> component) {
    if (g_router) {
        g_router->registerRouteComponent(path, component);
    }
}

/**
 * Register a layout component
 * 
 * Usage in layout files:
 * ```cpp
 * #include <obsidian/route_registry_helper.h>
 * 
 * void renderLayout(RouteContext& ctx, std::function<void()> renderChild) {
 *     // Layout implementation
 *     // Call renderChild() to render nested content
 *     renderChild();
 * }
 * 
 * // Register at static initialization time
 * static bool _registered = []() {
 *     if (g_router) {
 *         g_router->registerLayoutComponent("/", renderLayout);
 *     }
 *     return true;
 * }();
 * ```
 */
inline void registerLayout(const std::string& path, 
                          std::function<void(RouteContext&, std::function<void()>)> layout) {
    if (g_router) {
        g_router->registerLayoutComponent(path, layout);
    }
}

} // namespace obsidian
