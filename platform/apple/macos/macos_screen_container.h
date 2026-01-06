/**
 * macOS Screen Container FFI - C Interface
 * 
 * Based on react-native-screens' RNSScreenContainerView architecture.
 * The container OWNS and RETAINS all screen views. Routes don't directly
 * manage native views - the container does.
 * 
 * Key principles:
 * - Container owns all screen views (they're not destroyed when C++ goes out of scope)
 * - Screens are attached/detached explicitly
 * - Single active screen is displayed, others are retained but hidden
 * - Container handles all view lifecycle
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to the screen container
 */
typedef void* ObsidianScreenContainerHandle;

/**
 * Opaque handle to a screen (route view)
 */
typedef void* ObsidianScreenHandle;

/**
 * Screen container creation parameters
 */
typedef struct {
    int _reserved;
} ObsidianScreenContainerParams;

/**
 * Screen creation parameters
 */
typedef struct {
    const char* routePath;  // Route path this screen represents (e.g., "/", "/about")
} ObsidianScreenParams;

// ============================================================================
// Screen Container Management
// ============================================================================

/**
 * Create a new screen container
 * The container will own and manage all screens added to it
 */
ObsidianScreenContainerHandle obsidian_macos_create_screen_container(ObsidianScreenContainerParams params);

/**
 * Destroy the screen container and all screens it owns
 */
void obsidian_macos_destroy_screen_container(ObsidianScreenContainerHandle handle);

/**
 * Attach container to a window
 * The container's view becomes a subview of the window's contentView
 */
void obsidian_macos_screen_container_attach_to_window(ObsidianScreenContainerHandle handle, void* windowHandle);

/**
 * Attach container to an arbitrary parent view
 * The container's view becomes a subview of the parent view.
 * Use this when integrating with Sidebar or other container components.
 * @param handle Screen container handle
 * @param parentView NSView* to attach to
 */
void obsidian_macos_screen_container_attach_to_view(ObsidianScreenContainerHandle handle, void* parentView);

/**
 * Get the container's NSView
 */
void* obsidian_macos_screen_container_get_view(ObsidianScreenContainerHandle handle);

// ============================================================================
// Screen Management
// ============================================================================

/**
 * Create a screen for a route path
 * The screen is owned by the container
 */
ObsidianScreenHandle obsidian_macos_screen_container_create_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenParams params);

/**
 * Get an existing screen by route path (returns NULL if not found)
 */
ObsidianScreenHandle obsidian_macos_screen_container_get_screen(
    ObsidianScreenContainerHandle containerHandle,
    const char* routePath);

/**
 * Set the active screen (makes it visible, hides others)
 */
void obsidian_macos_screen_container_set_active_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenHandle screenHandle);

/**
 * Remove a screen from the container
 */
void obsidian_macos_screen_container_remove_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenHandle screenHandle);

/**
 * Clear all screens from the container
 */
void obsidian_macos_screen_container_clear_all(ObsidianScreenContainerHandle containerHandle);

// ============================================================================
// Screen Content Management
// ============================================================================

/**
 * Get the screen's content view (where route content goes)
 */
void* obsidian_macos_screen_get_content_view(ObsidianScreenHandle handle);

/**
 * Clear the screen's content
 */
void obsidian_macos_screen_clear_content(ObsidianScreenHandle handle);

/**
 * Add a subview to the screen's content
 */
void obsidian_macos_screen_add_subview(ObsidianScreenHandle handle, void* viewHandle);

/**
 * Get the route path for a screen
 */
const char* obsidian_macos_screen_get_route_path(ObsidianScreenHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
