/**
 * macOS VStack FFI - C Interface
 * 
 * Provides C interface for creating and managing VStack container views
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS VStack container view
 */
typedef void* ObsidianVStackHandle;

/**
 * VStack creation parameters
 */
typedef struct {
    // No parameters needed - container view is created empty
    // Add dummy field to avoid empty struct warning
    int _reserved;
} ObsidianVStackParams;

/**
 * Create a new VStack container view
 * Returns a handle to the VStack, or NULL on failure
 */
ObsidianVStackHandle obsidian_macos_create_vstack(ObsidianVStackParams params);

/**
 * Get the underlying NSView from a VStack handle
 * This is the container view that holds child views
 */
void* obsidian_macos_vstack_get_view(ObsidianVStackHandle handle);

/**
 * Get the padding view from a VStack handle
 * This is the nested view that visualizes the padding area
 * Child views should be added to this view, not the container
 */
void* obsidian_macos_vstack_get_padding_view(ObsidianVStackHandle handle);

/**
 * Add a child view to the VStack container
 * @param vstackHandle VStack container handle
 * @param childViewHandle Child view handle (must be an NSView*)
 */
void obsidian_macos_vstack_add_child_view(ObsidianVStackHandle vstackHandle,
                                          void* childViewHandle);

/**
 * Remove a child view from the VStack container
 */
void obsidian_macos_vstack_remove_child_view(ObsidianVStackHandle vstackHandle,
                                              void* childViewHandle);

/**
 * Set VStack visibility
 */
void obsidian_macos_vstack_set_visible(ObsidianVStackHandle handle, bool visible);

/**
 * Check if VStack is visible
 */
bool obsidian_macos_vstack_is_visible(ObsidianVStackHandle handle);

/**
 * Add VStack to a window's content view
 */
void obsidian_macos_vstack_add_to_window(ObsidianVStackHandle vstackHandle,
                                         void* windowHandle);

/**
 * Remove VStack from its parent view
 */
void obsidian_macos_vstack_remove_from_parent(ObsidianVStackHandle handle);

/**
 * Destroy the VStack container
 */
void obsidian_macos_destroy_vstack(ObsidianVStackHandle handle);

/**
 * Check if VStack handle is valid
 */
bool obsidian_macos_vstack_is_valid(ObsidianVStackHandle handle);

/**
 * Request immediate layout update for VStack container
 * This forces Auto Layout to apply constraints immediately
 */
void obsidian_macos_vstack_request_layout_update(ObsidianVStackHandle handle);

/**
 * Set padding values for the VStack container
 * This updates the container's intrinsic content size calculation
 */
void obsidian_macos_vstack_set_padding(ObsidianVStackHandle handle,
                                        double top, double bottom,
                                        double leading, double trailing);

/**
 * Set spacing between children in the VStack
 * This updates the container's intrinsic content size calculation
 */
void obsidian_macos_vstack_set_spacing(ObsidianVStackHandle handle, double spacing);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
