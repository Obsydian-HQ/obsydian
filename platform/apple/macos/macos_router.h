/**
 * macOS Router Integration
 * 
 * Integrates Obsydian Router with macOS NSViewController system
 * for native navigation patterns.
 */

#pragma once

#ifdef __APPLE__

#ifdef __cplusplus
#include <cstdint>
#include <string>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to macOS router integration
 */
typedef void* ObsidianMacOSRouterHandle;

/**
 * Parameters for creating a macOS router integration
 */
typedef struct {
    void* routerHandle;  // C++ Router* pointer (cast to void*)
    void* windowHandle;  // macOS window handle
} ObsidianMacOSRouterParams;

/**
 * Create a macOS router integration
 * @param params Router integration parameters
 * @return Handle to router integration, or nullptr on failure
 */
ObsidianMacOSRouterHandle obsidian_macos_router_create(ObsidianMacOSRouterParams params);

/**
 * Destroy a macOS router integration
 * @param handle Router integration handle
 */
void obsidian_macos_router_destroy(ObsidianMacOSRouterHandle handle);

/**
 * Check if router integration is valid
 * @param handle Router integration handle
 * @return true if valid
 */
bool obsidian_macos_router_is_valid(ObsidianMacOSRouterHandle handle);

/**
 * Get the view controller for the current route
 * @param handle Router integration handle
 * @return View controller handle, or nullptr if none
 */
void* obsidian_macos_router_get_current_viewcontroller(ObsidianMacOSRouterHandle handle);

/**
 * Handle navigation event (called when Router navigates)
 * This updates the view controller hierarchy
 * @param handle Router integration handle
 * @param path New route path
 */
void obsidian_macos_router_handle_navigation(ObsidianMacOSRouterHandle handle, const char* path);

/**
 * Set up navigation callbacks
 * This connects the Router's navigation events to macOS view controller updates
 * @param handle Router integration handle
 */
void obsidian_macos_router_setup_navigation_callbacks(ObsidianMacOSRouterHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
