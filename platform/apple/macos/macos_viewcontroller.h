/**
 * macOS ViewController FFI - C Interface
 * 
 * Provides C interface for creating and managing NSViewController instances
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * NSViewController is a fundamental AppKit class that manages a view hierarchy.
 * It's required for NSSplitViewController and NSSplitViewItem composition.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS view controller
 */
typedef void* ObsidianViewControllerHandle;

/**
 * ViewController creation parameters
 */
typedef struct {
    // No parameters needed - view controller is created empty
    // View will be set via setView
    int _reserved;
} ObsidianViewControllerParams;

/**
 * Create a new NSViewController
 * Returns a handle to the view controller, or NULL on failure
 */
ObsidianViewControllerHandle obsidian_macos_create_viewcontroller(ObsidianViewControllerParams params);

/**
 * Set the view for the view controller
 * @param viewControllerHandle View controller handle
 * @param viewHandle NSView handle (from VStack, HStack, List, etc.)
 */
void obsidian_macos_viewcontroller_set_view(ObsidianViewControllerHandle viewControllerHandle,
                                            void* viewHandle);

/**
 * Get the view from the view controller
 * Returns the NSView handle, or NULL if not set
 */
void* obsidian_macos_viewcontroller_get_view(ObsidianViewControllerHandle viewControllerHandle);

/**
 * Get the actual NSViewController* from the wrapper handle
 * This is for internal use by other FFI bindings (e.g., split view items)
 * @param viewControllerHandle View controller wrapper handle
 * @return The actual NSViewController* (bridged, caller should not release)
 */
void* obsidian_macos_viewcontroller_get_viewcontroller(ObsidianViewControllerHandle viewControllerHandle);

/**
 * Check if view controller is valid
 */
bool obsidian_macos_viewcontroller_is_valid(ObsidianViewControllerHandle handle);

/**
 * Configure the view controller for use as a sidebar with proper visual effect background.
 * This wraps the view in an NSVisualEffectView with the sidebar material to get
 * the native macOS sidebar appearance (translucent, vibrancy effect).
 * 
 * IMPORTANT: Call this AFTER setting the view via obsidian_macos_viewcontroller_set_view.
 * 
 * @param handle View controller handle
 */
void obsidian_macos_viewcontroller_configure_for_sidebar(ObsidianViewControllerHandle handle);

/**
 * Set the preferred content size for the view controller.
 * This is CRITICAL for NSSplitViewController to properly size its child items.
 * NSSplitView uses the preferredContentSize of child view controllers to determine
 * the layout of split view items.
 * 
 * @param handle View controller handle
 * @param width Preferred width (use 0 for unconstrained)
 * @param height Preferred height (use 0 for unconstrained)
 */
void obsidian_macos_viewcontroller_set_preferred_content_size(ObsidianViewControllerHandle handle,
                                                               double width, double height);

/**
 * Get the preferred content size for the view controller.
 * @param handle View controller handle
 * @param outWidth Pointer to receive width (can be NULL)
 * @param outHeight Pointer to receive height (can be NULL)
 */
void obsidian_macos_viewcontroller_get_preferred_content_size(ObsidianViewControllerHandle handle,
                                                               double* outWidth, double* outHeight);

/**
 * Destroy the view controller
 */
void obsidian_macos_destroy_viewcontroller(ObsidianViewControllerHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
