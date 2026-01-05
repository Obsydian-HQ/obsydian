/**
 * macOS SplitView FFI - C Interface
 * 
 * Provides C interface for creating and managing NSSplitView-based sidebar layouts
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * The SplitView provides a native macOS sidebar using NSSplitView with collapse/expand,
 * resizing, and content customization capabilities.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS SplitView (NSSplitView wrapper)
 */
typedef void* ObsidianSplitViewHandle;

/**
 * Sidebar position enumeration
 */
typedef enum {
    ObsidianSidebarPositionLeading = 0,  // Left side (LTR)
    ObsidianSidebarPositionTrailing = 1  // Right side (LTR)
} ObsidianSidebarPosition;

/**
 * SplitView creation parameters
 */
typedef struct {
    ObsidianSidebarPosition position;  // Sidebar position (leading or trailing)
    double initialSidebarWidth;        // Initial sidebar width in points (default: 200.0)
    double minSidebarWidth;            // Minimum sidebar width in points (default: 150.0)
    double maxSidebarWidth;            // Maximum sidebar width in points (default: 400.0)
} ObsidianSplitViewParams;

/**
 * Create a new SplitView container
 * Returns a handle to the SplitView, or NULL on failure
 * 
 * @param params Creation parameters (position, widths, etc.)
 */
ObsidianSplitViewHandle obsidian_macos_create_splitview(ObsidianSplitViewParams params);

/**
 * Destroy the SplitView container
 * This removes the SplitView from its parent and cleans up resources
 */
void obsidian_macos_destroy_splitview(ObsidianSplitViewHandle handle);

/**
 * Get the underlying NSView from a SplitView handle
 * This is the NSSplitView that contains the sidebar and main content
 */
void* obsidian_macos_splitview_get_view(ObsidianSplitViewHandle handle);

/**
 * Set the sidebar width (in points)
 * The width will be clamped to min/max values
 */
void obsidian_macos_splitview_set_sidebar_width(ObsidianSplitViewHandle handle, double width);

/**
 * Get the current sidebar width (in points)
 */
double obsidian_macos_splitview_get_sidebar_width(ObsidianSplitViewHandle handle);

/**
 * Set minimum sidebar width (in points)
 */
void obsidian_macos_splitview_set_minimum_sidebar_width(ObsidianSplitViewHandle handle, double width);

/**
 * Set maximum sidebar width (in points)
 */
void obsidian_macos_splitview_set_maximum_sidebar_width(ObsidianSplitViewHandle handle, double width);

/**
 * Collapse the sidebar pane
 */
void obsidian_macos_splitview_collapse_sidebar(ObsidianSplitViewHandle handle);

/**
 * Expand the sidebar pane (restore from collapsed state)
 */
void obsidian_macos_splitview_expand_sidebar(ObsidianSplitViewHandle handle);

/**
 * Toggle sidebar collapse/expand state
 */
void obsidian_macos_splitview_toggle_sidebar(ObsidianSplitViewHandle handle);

/**
 * Check if sidebar is currently collapsed
 */
bool obsidian_macos_splitview_is_sidebar_collapsed(ObsidianSplitViewHandle handle);

/**
 * Set the sidebar content view
 * @param handle SplitView handle
 * @param viewHandle NSView handle to use as sidebar content (must be a valid NSView*)
 */
void obsidian_macos_splitview_set_sidebar_view(ObsidianSplitViewHandle handle, void* viewHandle);

/**
 * Set the main content view
 * @param handle SplitView handle
 * @param viewHandle NSView handle to use as main content (must be a valid NSView*)
 */
void obsidian_macos_splitview_set_main_view(ObsidianSplitViewHandle handle, void* viewHandle);

/**
 * Add SplitView to a window's content view
 * @param handle SplitView handle
 * @param windowHandle Window handle (must be a valid ObsidianWindowHandle)
 */
void obsidian_macos_splitview_add_to_window(ObsidianSplitViewHandle handle, void* windowHandle);

/**
 * Remove SplitView from its parent view
 */
void obsidian_macos_splitview_remove_from_parent(ObsidianSplitViewHandle handle);

/**
 * Check if SplitView handle is valid
 */
bool obsidian_macos_splitview_is_valid(ObsidianSplitViewHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
