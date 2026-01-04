/**
 * macOS ZStack FFI - C Interface
 * 
 * Provides C interface for creating and managing ZStack container views
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS ZStack container view
 */
typedef void* ObsidianZStackHandle;

/**
 * ZStack creation parameters
 */
typedef struct {
    // No parameters needed - container view is created empty
    // Add dummy field to avoid empty struct warning
    int _reserved;
} ObsidianZStackParams;

/**
 * Create a new ZStack container view
 * Returns a handle to the ZStack, or NULL on failure
 */
ObsidianZStackHandle obsidian_macos_create_zstack(ObsidianZStackParams params);

/**
 * Get the underlying NSView from a ZStack handle
 * This is the container view that holds child views
 */
void* obsidian_macos_zstack_get_view(ObsidianZStackHandle handle);

/**
 * Add a child view to the ZStack container
 * @param zstackHandle ZStack container handle
 * @param childViewHandle Child view handle (must be an NSView*)
 */
void obsidian_macos_zstack_add_child_view(ObsidianZStackHandle zstackHandle,
                                          void* childViewHandle);

/**
 * Remove a child view from the ZStack container
 */
void obsidian_macos_zstack_remove_child_view(ObsidianZStackHandle zstackHandle,
                                              void* childViewHandle);

/**
 * Set ZStack visibility
 */
void obsidian_macos_zstack_set_visible(ObsidianZStackHandle handle, bool visible);

/**
 * Check if ZStack is visible
 */
bool obsidian_macos_zstack_is_visible(ObsidianZStackHandle handle);

/**
 * Add ZStack to a window's content view
 */
void obsidian_macos_zstack_add_to_window(ObsidianZStackHandle zstackHandle,
                                         void* windowHandle);

/**
 * Remove ZStack from its parent view
 */
void obsidian_macos_zstack_remove_from_parent(ObsidianZStackHandle handle);

/**
 * Destroy the ZStack container
 */
void obsidian_macos_destroy_zstack(ObsidianZStackHandle handle);

/**
 * Check if ZStack handle is valid
 */
bool obsidian_macos_zstack_is_valid(ObsidianZStackHandle handle);

/**
 * Request immediate layout update for ZStack container
 * This forces Auto Layout to apply constraints immediately
 */
void obsidian_macos_zstack_request_layout_update(ObsidianZStackHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
