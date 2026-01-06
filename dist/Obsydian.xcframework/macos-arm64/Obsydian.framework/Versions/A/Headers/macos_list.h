/**
 * macOS List FFI - C Interface
 * 
 * Provides C interface for creating and managing simple single-column lists
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * The List component uses NSTableView configured as a single-column list
 * (no headers, simple text items). This matches how SwiftUI's List view
 * works on macOS under the hood.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS list view
 */
typedef void* ObsidianListHandle;

/**
 * List creation parameters
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ObsidianListParams;

/**
 * List selection callback function type
 * Called when the user selects an item in the list
 * @param itemIndex The index of the selected item (0-based)
 * @param userData User data passed when setting the callback
 */
typedef void (*ObsidianListSelectionCallback)(int itemIndex, void* userData);

/**
 * Create a new macOS list view (NSTableView configured as single-column list)
 * Returns a handle to the list view, or NULL on failure
 */
ObsidianListHandle obsidian_macos_create_list(ObsidianListParams params);

/**
 * Add an item to the list
 * @param itemText The text to display for this item
 * @return The index of the newly added item, or -1 on failure
 */
int obsidian_macos_list_add_item(ObsidianListHandle handle, const char* itemText);

/**
 * Remove an item from the list
 * @param itemIndex Index of the item to remove (0-based)
 * @return true if item was removed successfully
 */
bool obsidian_macos_list_remove_item(ObsidianListHandle handle, int itemIndex);

/**
 * Remove all items from the list
 */
void obsidian_macos_list_clear(ObsidianListHandle handle);

/**
 * Get the number of items in the list
 */
int obsidian_macos_list_get_item_count(ObsidianListHandle handle);

/**
 * Get the text for a specific item
 * @param itemIndex Index of the item to get (0-based)
 * @return Pointer to the string data (do not free), or NULL if invalid
 */
const char* obsidian_macos_list_get_item(ObsidianListHandle handle, int itemIndex);

/**
 * Set the text for a specific item
 * @param itemIndex Index of the item to update (0-based)
 * @param itemText The new text for this item
 * @return true if item was updated successfully
 */
bool obsidian_macos_list_set_item(ObsidianListHandle handle, int itemIndex, const char* itemText);

/**
 * Set the list selection callback
 * userData will be passed to the callback when an item is selected
 */
void obsidian_macos_list_set_on_selection(ObsidianListHandle handle, 
                                          ObsidianListSelectionCallback callback,
                                          void* userData);

/**
 * Get the currently selected item index
 * @return The index of the selected item (0-based), or -1 if no item is selected
 */
int obsidian_macos_list_get_selected_index(ObsidianListHandle handle);

/**
 * Set the selected item
 * @param itemIndex Index of the item to select (0-based), or -1 to deselect all
 * @return true if selection was set successfully
 */
bool obsidian_macos_list_set_selected_index(ObsidianListHandle handle, int itemIndex);

/**
 * Set list visibility
 */
void obsidian_macos_list_set_visible(ObsidianListHandle handle, bool visible);

/**
 * Check if list is visible
 */
bool obsidian_macos_list_is_visible(ObsidianListHandle handle);

/**
 * Set list enabled state
 */
void obsidian_macos_list_set_enabled(ObsidianListHandle handle, bool enabled);

/**
 * Check if list is enabled
 */
bool obsidian_macos_list_is_enabled(ObsidianListHandle handle);

/**
 * Add list to a window's content view
 * The list will be added as a subview (wrapped in an NSScrollView for scrolling)
 */
void obsidian_macos_list_add_to_window(ObsidianListHandle listHandle, 
                                      void* windowHandle);

/**
 * Remove list from its parent view
 */
void obsidian_macos_list_remove_from_parent(ObsidianListHandle handle);

/**
 * Close and destroy the list
 */
void obsidian_macos_destroy_list(ObsidianListHandle handle);

/**
 * Check if list handle is valid
 */
bool obsidian_macos_list_is_valid(ObsidianListHandle handle);

/**
 * Get the underlying NSView (NSScrollView) from a List handle
 * This is the scroll view that wraps the table view
 */
void* obsidian_macos_list_get_view(ObsidianListHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

