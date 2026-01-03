/**
 * macOS Button FFI - C Interface
 * 
 * Provides C interface for creating and managing NSButton instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS button
 */
typedef void* ObsidianButtonHandle;

/**
 * Button creation parameters
 */
typedef struct {
    const char* title;
    int x;
    int y;
    int width;
    int height;
} ObsidianButtonParams;

/**
 * Button click callback function type
 * Called when the button is clicked
 */
typedef void (*ObsidianButtonClickCallback)(void* userData);

/**
 * Create a new macOS button
 * Returns a handle to the button, or NULL on failure
 */
ObsidianButtonHandle obsidian_macos_create_button(ObsidianButtonParams params);

/**
 * Set the button title/text
 */
void obsidian_macos_button_set_title(ObsidianButtonHandle handle, const char* title);

/**
 * Get the button title/text
 * Returns a pointer to the internal string (do not free)
 * Returns NULL if handle is invalid
 */
const char* obsidian_macos_button_get_title(ObsidianButtonHandle handle);

/**
 * Set the button click callback
 * userData will be passed to the callback when button is clicked
 */
void obsidian_macos_button_set_on_click(ObsidianButtonHandle handle, 
                                         ObsidianButtonClickCallback callback,
                                         void* userData);

/**
 * Set button visibility
 */
void obsidian_macos_button_set_visible(ObsidianButtonHandle handle, bool visible);

/**
 * Check if button is visible
 */
bool obsidian_macos_button_is_visible(ObsidianButtonHandle handle);

/**
 * Set button enabled state
 */
void obsidian_macos_button_set_enabled(ObsidianButtonHandle handle, bool enabled);

/**
 * Check if button is enabled
 */
bool obsidian_macos_button_is_enabled(ObsidianButtonHandle handle);

/**
 * Add button to a window's content view
 * The button will be added as a subview
 */
void obsidian_macos_button_add_to_window(ObsidianButtonHandle buttonHandle, 
                                         void* windowHandle);

/**
 * Remove button from its parent view
 */
void obsidian_macos_button_remove_from_parent(ObsidianButtonHandle handle);

/**
 * Close and destroy the button
 */
void obsidian_macos_destroy_button(ObsidianButtonHandle handle);

/**
 * Check if button handle is valid
 */
bool obsidian_macos_button_is_valid(ObsidianButtonHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

