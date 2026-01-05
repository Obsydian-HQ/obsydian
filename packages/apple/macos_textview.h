/**
 * macOS TextView FFI - C Interface
 * 
 * Provides C interface for creating and managing NSTextView instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS text view
 */
typedef void* ObsidianTextViewHandle;

/**
 * TextView creation parameters
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ObsidianTextViewParams;

/**
 * Create a new macOS text view
 * Returns a handle to the text view, or NULL on failure
 */
ObsidianTextViewHandle obsidian_macos_create_textview(ObsidianTextViewParams params);

/**
 * Set the text view text
 */
void obsidian_macos_textview_set_string(ObsidianTextViewHandle handle, const char* text);

/**
 * Get the text view text
 * Returns a pointer to the internal string (do not free)
 * Returns NULL if handle is invalid
 */
const char* obsidian_macos_textview_get_string(ObsidianTextViewHandle handle);

/**
 * Set text view editable state
 */
void obsidian_macos_textview_set_editable(ObsidianTextViewHandle handle, bool editable);

/**
 * Check if text view is editable
 */
bool obsidian_macos_textview_is_editable(ObsidianTextViewHandle handle);

/**
 * Set text view selectable state
 */
void obsidian_macos_textview_set_selectable(ObsidianTextViewHandle handle, bool selectable);

/**
 * Check if text view is selectable
 */
bool obsidian_macos_textview_is_selectable(ObsidianTextViewHandle handle);

/**
 * Set text view visibility
 */
void obsidian_macos_textview_set_visible(ObsidianTextViewHandle handle, bool visible);

/**
 * Check if text view is visible
 */
bool obsidian_macos_textview_is_visible(ObsidianTextViewHandle handle);

/**
 * Set text view enabled state
 */
void obsidian_macos_textview_set_enabled(ObsidianTextViewHandle handle, bool enabled);

/**
 * Check if text view is enabled
 */
bool obsidian_macos_textview_is_enabled(ObsidianTextViewHandle handle);

/**
 * Add text view to a window's content view
 * The text view will be added as a subview
 */
void obsidian_macos_textview_add_to_window(ObsidianTextViewHandle textViewHandle, 
                                           void* windowHandle);

/**
 * Remove text view from its parent view
 */
void obsidian_macos_textview_remove_from_parent(ObsidianTextViewHandle handle);

/**
 * Close and destroy the text view
 */
void obsidian_macos_destroy_textview(ObsidianTextViewHandle handle);

/**
 * Release the text view handle WITHOUT removing from parent.
 * The text view stays in the view hierarchy (retained by superview).
 */
void obsidian_macos_release_textview_handle(ObsidianTextViewHandle handle);

/**
 * Check if text view handle is valid
 */
bool obsidian_macos_textview_is_valid(ObsidianTextViewHandle handle);

/**
 * Get the underlying NSView handle from a text view handle
 * This is used for setting the text view as a document view in a scroll view
 * Returns the NSView handle, or NULL if invalid
 */
void* obsidian_macos_textview_get_view_handle(ObsidianTextViewHandle handle);

/**
 * Font weight enumeration (matches C++ FontWeight enum)
 */
typedef enum {
    ObsidianFontWeightRegular = 0,
    ObsidianFontWeightBold = 1,
    ObsidianFontWeightSemibold = 2,
    ObsidianFontWeightMedium = 3,
    ObsidianFontWeightLight = 4,
    ObsidianFontWeightThin = 5
} ObsidianFontWeight;

/**
 * Set font size
 * @param handle Text view handle
 * @param size Font size in points
 */
void obsidian_macos_textview_set_font_size(ObsidianTextViewHandle handle, double size);

/**
 * Get font size
 * @param handle Text view handle
 * @return Font size in points, or 0.0 if invalid
 */
double obsidian_macos_textview_get_font_size(ObsidianTextViewHandle handle);

/**
 * Set font weight
 * @param handle Text view handle
 * @param weight Font weight
 */
void obsidian_macos_textview_set_font_weight(ObsidianTextViewHandle handle, ObsidianFontWeight weight);

/**
 * Get font weight
 * @param handle Text view handle
 * @return Font weight, or Regular if invalid
 */
ObsidianFontWeight obsidian_macos_textview_get_font_weight(ObsidianTextViewHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

