/**
 * macOS ScrollView FFI - C Interface
 * 
 * Provides C interface for creating and managing NSScrollView instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS scroll view
 */
typedef void* ObsidianScrollViewHandle;

/**
 * ScrollView creation parameters
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ObsidianScrollViewParams;

/**
 * Create a new macOS scroll view
 * Returns a handle to the scroll view, or NULL on failure
 */
ObsidianScrollViewHandle obsidian_macos_create_scrollview(ObsidianScrollViewParams params);

/**
 * Set the document view for the scroll view
 * The document view is the view that will be scrolled
 */
void obsidian_macos_scrollview_set_document_view(ObsidianScrollViewHandle scrollViewHandle,
                                                  void* documentViewHandle);

/**
 * Get the document view from the scroll view
 * Returns the handle to the document view, or NULL if none is set
 */
void* obsidian_macos_scrollview_get_document_view(ObsidianScrollViewHandle handle);

/**
 * Set scroll view visibility
 */
void obsidian_macos_scrollview_set_visible(ObsidianScrollViewHandle handle, bool visible);

/**
 * Check if scroll view is visible
 */
bool obsidian_macos_scrollview_is_visible(ObsidianScrollViewHandle handle);

/**
 * Add scroll view to a window's content view
 * The scroll view will be added as a subview
 */
void obsidian_macos_scrollview_add_to_window(ObsidianScrollViewHandle scrollViewHandle,
                                             void* windowHandle);

/**
 * Remove scroll view from its parent view
 */
void obsidian_macos_scrollview_remove_from_parent(ObsidianScrollViewHandle handle);

/**
 * Close and destroy the scroll view
 */
void obsidian_macos_destroy_scrollview(ObsidianScrollViewHandle handle);

/**
 * Check if scroll view handle is valid
 */
bool obsidian_macos_scrollview_is_valid(ObsidianScrollViewHandle handle);

/**
 * Get the underlying NSView (NSScrollView) from a ScrollView handle
 */
void* obsidian_macos_scrollview_get_view(ObsidianScrollViewHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

