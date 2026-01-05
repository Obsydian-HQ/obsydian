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
 * Destroy the view controller
 */
void obsidian_macos_destroy_viewcontroller(ObsidianViewControllerHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
