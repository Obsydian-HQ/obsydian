/**
 * macOS VStack FFI - C Interface
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
 * Opaque handle to a macOS VStack container view
 */
typedef void* ObsidianVStackHandle;

/**
 * VStack creation parameters
 */
typedef struct {
    int _reserved;  // Placeholder
} ObsidianVStackParams;

/**
 * Create a new VStack container view
 */
ObsidianVStackHandle obsidian_macos_create_vstack(ObsidianVStackParams params);

/**
 * Get the underlying NSView from a VStack handle
 */
void* obsidian_macos_vstack_get_view(ObsidianVStackHandle handle);

/**
 * Add a child view to the VStack container
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
 * Destroy the VStack container and remove from parent
 */
void obsidian_macos_destroy_vstack(ObsidianVStackHandle handle);

/**
 * Release the VStack handle WITHOUT removing from parent.
 */
void obsidian_macos_release_vstack_handle(ObsidianVStackHandle handle);

/**
 * Check if VStack handle is valid
 */
bool obsidian_macos_vstack_is_valid(ObsidianVStackHandle handle);

/**
 * Request layout update (NO-OP: layout is handled by layout engine)
 */
void obsidian_macos_vstack_request_layout_update(ObsidianVStackHandle handle);

/**
 * Set padding values (stored for layout engine to read)
 */
void obsidian_macos_vstack_set_padding(ObsidianVStackHandle handle,
                                        double top, double bottom,
                                        double leading, double trailing);

/**
 * Set spacing between children (stored for layout engine to read)
 */
void obsidian_macos_vstack_set_spacing(ObsidianVStackHandle handle, double spacing);

/**
 * Set frame directly - called by the layout engine
 */
void obsidian_macos_vstack_set_frame(ObsidianVStackHandle handle,
                                      double x, double y,
                                      double width, double height);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
