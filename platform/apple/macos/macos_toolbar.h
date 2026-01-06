/**
 * macOS Toolbar FFI - C Interface
 * 
 * Provides C interface for creating and managing NSToolbar instances
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * NSToolbar is REQUIRED for native sidebar collapse button functionality.
 * The collapse button is provided by NSToolbarToggleSidebarItemIdentifier
 * which must be added to the toolbar.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS toolbar
 */
typedef void* ObsidianToolbarHandle;

/**
 * Toolbar display mode
 */
typedef enum {
    ObsidianToolbarDisplayModeDefault = 0,      // Default display mode
    ObsidianToolbarDisplayModeIconAndLabel = 1, // Icons with labels
    ObsidianToolbarDisplayModeIconOnly = 2,     // Icons only
    ObsidianToolbarDisplayModeLabelOnly = 3     // Labels only
} ObsidianToolbarDisplayMode;

/**
 * Toolbar style (macOS 11+)
 */
typedef enum {
    ObsidianToolbarStyleAutomatic = 0,          // Automatic based on window
    ObsidianToolbarStyleExpanded = 1,           // Expanded toolbar
    ObsidianToolbarStylePreference = 2,         // Preference window style
    ObsidianToolbarStyleUnified = 3,            // Unified with title bar
    ObsidianToolbarStyleUnifiedCompact = 4      // Compact unified with title bar
} ObsidianToolbarStyle;

/**
 * Toolbar creation parameters
 */
typedef struct {
    const char* identifier;                     // Unique toolbar identifier
    ObsidianToolbarDisplayMode displayMode;     // Display mode
    bool allowsUserCustomization;               // Allow user to customize toolbar
    bool autosavesConfiguration;               // Auto-save toolbar configuration
} ObsidianToolbarParams;

/**
 * Create a new macOS toolbar
 * @param params Creation parameters
 * @return Toolbar handle, or NULL on failure
 */
ObsidianToolbarHandle obsidian_macos_create_toolbar(ObsidianToolbarParams params);

/**
 * Set the toolbar's display mode
 * @param handle Toolbar handle
 * @param displayMode The display mode to set
 */
void obsidian_macos_toolbar_set_display_mode(ObsidianToolbarHandle handle, ObsidianToolbarDisplayMode displayMode);

/**
 * Get the toolbar's display mode
 * @param handle Toolbar handle
 * @return Current display mode
 */
ObsidianToolbarDisplayMode obsidian_macos_toolbar_get_display_mode(ObsidianToolbarHandle handle);

/**
 * Insert the sidebar toggle item at the specified index
 * This adds NSToolbarToggleSidebarItemIdentifier which provides the native
 * collapse/expand button for sidebars.
 * @param handle Toolbar handle
 * @param index Index where to insert the item
 */
void obsidian_macos_toolbar_insert_sidebar_toggle_item(ObsidianToolbarHandle handle, int index);

/**
 * Insert the sidebar tracking separator at the specified index (macOS 11+)
 * This adds NSToolbarSidebarTrackingSeparatorItemIdentifier which provides
 * proper separator tracking when the sidebar is resized.
 * @param handle Toolbar handle
 * @param index Index where to insert the separator
 */
void obsidian_macos_toolbar_insert_sidebar_tracking_separator(ObsidianToolbarHandle handle, int index);

/**
 * Insert a flexible space item at the specified index
 * @param handle Toolbar handle
 * @param index Index where to insert the item
 */
void obsidian_macos_toolbar_insert_flexible_space(ObsidianToolbarHandle handle, int index);

/**
 * Get the item count in the toolbar
 * @param handle Toolbar handle
 * @return Number of items in the toolbar
 */
int obsidian_macos_toolbar_get_item_count(ObsidianToolbarHandle handle);

/**
 * Check if toolbar is valid
 * @param handle Toolbar handle
 * @return true if valid
 */
bool obsidian_macos_toolbar_is_valid(ObsidianToolbarHandle handle);

/**
 * Get the actual NSToolbar* from the wrapper handle
 * This is for internal use by Window to set the toolbar
 * @param handle Toolbar wrapper handle
 * @return The actual NSToolbar* (bridged, caller should not release)
 */
void* obsidian_macos_toolbar_get_nstoolbar(ObsidianToolbarHandle handle);

/**
 * Destroy the toolbar
 * @param handle Toolbar handle
 */
void obsidian_macos_destroy_toolbar(ObsidianToolbarHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
