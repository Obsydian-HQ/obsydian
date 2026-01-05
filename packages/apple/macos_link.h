/**
 * macOS Link FFI - C Interface
 * 
 * Provides C interface for creating clickable link wrappers around any view
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS link wrapper
 */
typedef void* ObsidianLinkHandle;

/**
 * Link creation parameters
 */
typedef struct {
    const char* href;  // Route path for navigation
    void* childView;    // Native view handle of the child component to wrap
} ObsidianLinkParams;

/**
 * Link click callback function type
 * Called when the link is clicked
 */
typedef void (*ObsidianLinkClickCallback)(void* userData);

/**
 * Create a new macOS link wrapper around a child view
 * Returns a handle to the link, or NULL on failure
 */
ObsidianLinkHandle obsidian_macos_create_link(ObsidianLinkParams params);

/**
 * Set the href (route path) for navigation
 */
void obsidian_macos_link_set_href(ObsidianLinkHandle handle, const char* href);

/**
 * Get the href
 * Returns a pointer to the internal string (do not free)
 * Returns NULL if handle is invalid
 */
const char* obsidian_macos_link_get_href(ObsidianLinkHandle handle);

/**
 * Set the link click callback
 * userData will be passed to the callback when link is clicked
 */
void obsidian_macos_link_set_on_click(ObsidianLinkHandle handle,
                                       ObsidianLinkClickCallback callback,
                                       void* userData);

/**
 * Set link visibility
 */
void obsidian_macos_link_set_visible(ObsidianLinkHandle handle, bool visible);

/**
 * Check if link is visible
 */
bool obsidian_macos_link_is_visible(ObsidianLinkHandle handle);

/**
 * Set link enabled state
 */
void obsidian_macos_link_set_enabled(ObsidianLinkHandle handle, bool enabled);

/**
 * Check if link is enabled
 */
bool obsidian_macos_link_is_enabled(ObsidianLinkHandle handle);

/**
 * Add link to a window's content view
 * The link wrapper will be added as a subview
 */
void obsidian_macos_link_add_to_window(ObsidianLinkHandle linkHandle,
                                       void* windowHandle);

/**
 * Remove link from its parent view
 */
void obsidian_macos_link_remove_from_parent(ObsidianLinkHandle handle);

/**
 * Close and destroy the link
 */
void obsidian_macos_destroy_link(ObsidianLinkHandle handle);

/**
 * Release the link handle WITHOUT removing from parent.
 */
void obsidian_macos_release_link_handle(ObsidianLinkHandle handle);

/**
 * Check if link handle is valid
 */
bool obsidian_macos_link_is_valid(ObsidianLinkHandle handle);

/**
 * Get the underlying NSView from a link handle
 * Returns the wrapper NSView* (which contains the child view)
 * This is for internal use by other FFI bindings (e.g., layout constraints)
 */
void* obsidian_macos_link_get_view(ObsidianLinkHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
