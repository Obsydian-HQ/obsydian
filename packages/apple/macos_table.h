/**
 * macOS Table FFI - C Interface
 * 
 * Provides C interface for creating and managing NSTableView instances
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * The Table component supports NSTableView for tabular data display.
 * It provides a unified API that can be extended to support NSOutlineView
 * and NSCollectionView in the future.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS table view
 */
typedef void* ObsidianTableHandle;

/**
 * Table creation parameters
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ObsidianTableParams;

/**
 * Table selection callback function type
 * Called when the user selects a row in the table
 * @param rowIndex The index of the selected row (0-based)
 * @param userData User data passed when setting the callback
 */
typedef void (*ObsidianTableSelectionCallback)(int rowIndex, void* userData);

/**
 * Create a new macOS table view (NSTableView)
 * Returns a handle to the table view, or NULL on failure
 */
ObsidianTableHandle obsidian_macos_create_table(ObsidianTableParams params);

/**
 * Add a column to the list
 * @param identifier Unique identifier for the column (used for data access)
 * @param title Display title for the column header
 * @param width Initial width of the column in pixels
 * @return true if column was added successfully
 */
bool obsidian_macos_table_add_column(ObsidianTableHandle handle, const char* identifier, const char* title, int width);

/**
 * Add a row to the list
 * @param rowData Array of strings, one for each column (in order of column creation)
 * @param columnCount Number of columns (must match the number of columns in the list)
 * @return The index of the newly added row, or -1 on failure
 */
int obsidian_macos_table_add_row(ObsidianTableHandle handle, const char** rowData, int columnCount);

/**
 * Remove a row from the list
 * @param rowIndex Index of the row to remove (0-based)
 * @return true if row was removed successfully
 */
bool obsidian_macos_table_remove_row(ObsidianTableHandle handle, int rowIndex);

/**
 * Remove all rows from the list
 */
void obsidian_macos_table_clear(ObsidianTableHandle handle);

/**
 * Get the number of rows in the list
 */
int obsidian_macos_table_get_row_count(ObsidianTableHandle handle);

/**
 * Get the number of columns in the list
 */
int obsidian_macos_table_get_column_count(ObsidianTableHandle handle);

/**
 * Set the data for a specific row
 * @param rowIndex Index of the row to update (0-based)
 * @param rowData Array of strings, one for each column (in order of column creation)
 * @param columnCount Number of columns (must match the number of columns in the list)
 * @return true if row was updated successfully
 */
bool obsidian_macos_table_set_row_data(ObsidianTableHandle handle, int rowIndex, const char** rowData, int columnCount);

/**
 * Get the data for a specific row
 * @param rowIndex Index of the row to get (0-based)
 * @param columnIndex Index of the column to get (0-based)
 * @return Pointer to the string data (do not free), or NULL if invalid
 */
const char* obsidian_macos_table_get_row_data(ObsidianTableHandle handle, int rowIndex, int columnIndex);

/**
 * Set the list selection callback
 * userData will be passed to the callback when a row is selected
 */
void obsidian_macos_table_set_on_selection(ObsidianTableHandle handle, 
                                          ObsidianTableSelectionCallback callback,
                                          void* userData);

/**
 * Get the currently selected row index
 * @return The index of the selected row (0-based), or -1 if no row is selected
 */
int obsidian_macos_table_get_selected_row(ObsidianTableHandle handle);

/**
 * Set the selected row
 * @param rowIndex Index of the row to select (0-based), or -1 to deselect all
 * @return true if selection was set successfully
 */
bool obsidian_macos_table_set_selected_row(ObsidianTableHandle handle, int rowIndex);

/**
 * Set list visibility
 */
void obsidian_macos_table_set_visible(ObsidianTableHandle handle, bool visible);

/**
 * Check if list is visible
 */
bool obsidian_macos_table_is_visible(ObsidianTableHandle handle);

/**
 * Set list enabled state
 */
void obsidian_macos_table_set_enabled(ObsidianTableHandle handle, bool enabled);

/**
 * Check if list is enabled
 */
bool obsidian_macos_table_is_enabled(ObsidianTableHandle handle);

/**
 * Add list to a window's content view
 * The list will be added as a subview (wrapped in an NSScrollView for scrolling)
 */
void obsidian_macos_table_add_to_window(ObsidianTableHandle listHandle, 
                                      void* windowHandle);

/**
 * Remove list from its parent view
 */
void obsidian_macos_table_remove_from_parent(ObsidianTableHandle handle);

/**
 * Close and destroy the list
 */
void obsidian_macos_destroy_table(ObsidianTableHandle handle);

/**
 * Check if list handle is valid
 */
bool obsidian_macos_table_is_valid(ObsidianTableHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

