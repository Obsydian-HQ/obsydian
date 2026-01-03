/**
 * macOS TextField FFI - C Interface
 * 
 * Provides C interface for creating and managing NSTextField instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS text field
 */
typedef void* ObsidianTextFieldHandle;

/**
 * TextField creation parameters
 */
typedef struct {
    const char* placeholder;
    int x;
    int y;
    int width;
    int height;
} ObsidianTextFieldParams;

/**
 * TextField enter key callback function type
 * Called when the user presses Enter/Return in the text field
 */
typedef void (*ObsidianTextFieldEnterCallback)(void* userData);

/**
 * Create a new macOS text field
 * Returns a handle to the text field, or NULL on failure
 */
ObsidianTextFieldHandle obsidian_macos_create_textfield(ObsidianTextFieldParams params);

/**
 * Set the text field text
 */
void obsidian_macos_textfield_set_text(ObsidianTextFieldHandle handle, const char* text);

/**
 * Get the text field text
 * Returns a pointer to the internal string (do not free)
 * Returns NULL if handle is invalid
 */
const char* obsidian_macos_textfield_get_text(ObsidianTextFieldHandle handle);

/**
 * Set the text field placeholder text
 */
void obsidian_macos_textfield_set_placeholder(ObsidianTextFieldHandle handle, const char* placeholder);

/**
 * Set the text field enter key callback
 * userData will be passed to the callback when Enter/Return is pressed
 */
void obsidian_macos_textfield_set_on_enter(ObsidianTextFieldHandle handle, 
                                           ObsidianTextFieldEnterCallback callback,
                                           void* userData);

/**
 * Set text field visibility
 */
void obsidian_macos_textfield_set_visible(ObsidianTextFieldHandle handle, bool visible);

/**
 * Check if text field is visible
 */
bool obsidian_macos_textfield_is_visible(ObsidianTextFieldHandle handle);

/**
 * Set text field enabled state
 */
void obsidian_macos_textfield_set_enabled(ObsidianTextFieldHandle handle, bool enabled);

/**
 * Check if text field is enabled
 */
bool obsidian_macos_textfield_is_enabled(ObsidianTextFieldHandle handle);

/**
 * Add text field to a window's content view
 * The text field will be added as a subview
 */
void obsidian_macos_textfield_add_to_window(ObsidianTextFieldHandle textFieldHandle, 
                                            void* windowHandle);

/**
 * Remove text field from its parent view
 */
void obsidian_macos_textfield_remove_from_parent(ObsidianTextFieldHandle handle);

/**
 * Close and destroy the text field
 */
void obsidian_macos_destroy_textfield(ObsidianTextFieldHandle handle);

/**
 * Check if text field handle is valid
 */
bool obsidian_macos_textfield_is_valid(ObsidianTextFieldHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

