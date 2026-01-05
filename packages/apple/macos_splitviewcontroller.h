/**
 * macOS SplitViewController FFI - C Interface
 * 
 * Provides C interface for creating and managing NSSplitViewController instances
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * NSSplitViewController manages NSSplitViewItem instances and provides
 * higher-level split view management than NSSplitView.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS split view controller
 */
typedef void* ObsidianSplitViewControllerHandle;

/**
 * SplitViewController creation parameters
 */
typedef struct {
    // No parameters needed - split view controller is created empty
    // Split view items will be added via addSplitViewItem
    int _reserved;
} ObsidianSplitViewControllerParams;

/**
 * Create a new NSSplitViewController
 * Returns a handle to the split view controller, or NULL on failure
 */
ObsidianSplitViewControllerHandle obsidian_macos_create_splitviewcontroller(ObsidianSplitViewControllerParams params);

/**
 * Get the view from the split view controller
 * Returns the NSView handle, or NULL if not set
 */
void* obsidian_macos_splitviewcontroller_get_view(ObsidianSplitViewControllerHandle handle);

/**
 * Add a split view item to the split view controller
 * @param handle Split view controller handle
 * @param splitViewItemHandle NSSplitViewItem handle (created via NSSplitViewItem functions)
 */
void obsidian_macos_splitviewcontroller_add_splitviewitem(ObsidianSplitViewControllerHandle handle,
                                                          void* splitViewItemHandle);

/**
 * Remove a split view item from the split view controller
 * @param handle Split view controller handle
 * @param splitViewItemHandle NSSplitViewItem handle to remove
 */
void obsidian_macos_splitviewcontroller_remove_splitviewitem(ObsidianSplitViewControllerHandle handle,
                                                             void* splitViewItemHandle);

/**
 * Add the split view controller's view to a window's content view
 * Sets up Auto Layout constraints to fill the content view
 * @param handle Split view controller handle
 * @param windowHandle Window handle
 */
void obsidian_macos_splitviewcontroller_add_to_window(ObsidianSplitViewControllerHandle handle,
                                                      void* windowHandle);

/**
 * Check if split view controller is valid
 */
bool obsidian_macos_splitviewcontroller_is_valid(ObsidianSplitViewControllerHandle handle);

/**
 * Destroy the split view controller
 */
void obsidian_macos_destroy_splitviewcontroller(ObsidianSplitViewControllerHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
