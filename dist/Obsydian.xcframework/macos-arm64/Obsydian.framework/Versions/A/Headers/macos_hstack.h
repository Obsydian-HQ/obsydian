/**
 * macOS HStack FFI - C Interface
 * 
 * Provides C interface for creating and managing HStack container views
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS HStack container view
 */
typedef void* ObsidianHStackHandle;

/**
 * HStack creation parameters
 */
typedef struct {
    // No parameters needed - container view is created empty
    // Add dummy field to avoid empty struct warning
    int _reserved;
} ObsidianHStackParams;

/**
 * Create a new HStack container view
 * Returns a handle to the HStack, or NULL on failure
 */
ObsidianHStackHandle obsidian_macos_create_hstack(ObsidianHStackParams params);

/**
 * Get the underlying NSView from an HStack handle
 * This is the container view that holds child views
 */
void* obsidian_macos_hstack_get_view(ObsidianHStackHandle handle);

/**
 * Add a child view to the HStack container
 * @param hstackHandle HStack container handle
 * @param childViewHandle Child view handle (must be an NSView*)
 */
void obsidian_macos_hstack_add_child_view(ObsidianHStackHandle hstackHandle,
                                          void* childViewHandle);

/**
 * Remove a child view from the HStack container
 */
void obsidian_macos_hstack_remove_child_view(ObsidianHStackHandle hstackHandle,
                                             void* childViewHandle);

/**
 * Set HStack visibility
 */
void obsidian_macos_hstack_set_visible(ObsidianHStackHandle handle, bool visible);

/**
 * Check if HStack is visible
 */
bool obsidian_macos_hstack_is_visible(ObsidianHStackHandle handle);

/**
 * Add HStack to a window's content view
 */
void obsidian_macos_hstack_add_to_window(ObsidianHStackHandle hstackHandle,
                                        void* windowHandle);

/**
 * Remove HStack from its parent view
 */
void obsidian_macos_hstack_remove_from_parent(ObsidianHStackHandle handle);

/**
 * Destroy the HStack container
 */
void obsidian_macos_destroy_hstack(ObsidianHStackHandle handle);

/**
 * Check if HStack handle is valid
 */
bool obsidian_macos_hstack_is_valid(ObsidianHStackHandle handle);

/**
 * Request immediate layout update for HStack container
 * This forces Auto Layout to apply constraints immediately
 */
void obsidian_macos_hstack_request_layout_update(ObsidianHStackHandle handle);

/**
 * Set padding values for the HStack container
 * This updates the container's intrinsic content size calculation
 */
void obsidian_macos_hstack_set_padding(ObsidianHStackHandle handle,
                                       double top, double bottom,
                                       double leading, double trailing);

/**
 * Set spacing between children in the HStack
 * This updates the container's intrinsic content size calculation
 */
void obsidian_macos_hstack_set_spacing(ObsidianHStackHandle handle, double spacing);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
