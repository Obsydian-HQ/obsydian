/**
 * macOS HStack FFI - C Interface
 * 
 * DUMB CONTAINER: This view does NOT compute layout.
 * The Layout Engine sets frames directly on this view and its children.
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
    int _reserved;  // Placeholder
} ObsidianHStackParams;

/**
 * Create a new HStack container view
 */
ObsidianHStackHandle obsidian_macos_create_hstack(ObsidianHStackParams params);

/**
 * Get the underlying NSView from an HStack handle
 */
void* obsidian_macos_hstack_get_view(ObsidianHStackHandle handle);

/**
 * Add a child view to the HStack container
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
 * Destroy the HStack container and remove from parent
 */
void obsidian_macos_destroy_hstack(ObsidianHStackHandle handle);

/**
 * Release the HStack handle WITHOUT removing from parent.
 */
void obsidian_macos_release_hstack_handle(ObsidianHStackHandle handle);

/**
 * Check if HStack handle is valid
 */
bool obsidian_macos_hstack_is_valid(ObsidianHStackHandle handle);

/**
 * Request layout update (NO-OP: layout is handled by layout engine)
 */
void obsidian_macos_hstack_request_layout_update(ObsidianHStackHandle handle);

/**
 * Set padding values (stored for layout engine to read)
 */
void obsidian_macos_hstack_set_padding(ObsidianHStackHandle handle,
                                       double top, double bottom,
                                       double leading, double trailing);

/**
 * Set spacing between children (stored for layout engine to read)
 */
void obsidian_macos_hstack_set_spacing(ObsidianHStackHandle handle, double spacing);

/**
 * Set frame directly - called by the layout engine
 */
void obsidian_macos_hstack_set_frame(ObsidianHStackHandle handle,
                                      double x, double y,
                                      double width, double height);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
