/**
 * macOS SplitView FFI - C Interface
 * 
 * Provides C interface for creating and managing NSSplitView-based split view layouts
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * The SplitView provides a split view using NSSplitView with collapse/expand,
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
 * Split position enumeration
 * Specifies which side the primary pane appears on
 */
typedef enum {
    ObsidianSplitPositionLeading = 0,  // Primary pane on left side (LTR)
    ObsidianSplitPositionTrailing = 1  // Primary pane on right side (LTR)
} ObsidianSplitPosition;

/**
 * SplitView creation parameters
 */
typedef struct {
    ObsidianSplitPosition position;  // Split position (leading or trailing)
    double initialPrimaryPaneWidth;  // Initial primary pane width in points (default: 200.0)
    double minPrimaryPaneWidth;      // Minimum primary pane width in points (default: 150.0)
    double maxPrimaryPaneWidth;      // Maximum primary pane width in points (default: 400.0)
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
 * This is the NSSplitView that contains the primary and secondary panes
 */
void* obsidian_macos_splitview_get_view(ObsidianSplitViewHandle handle);

/**
 * Set the primary pane width (in points)
 * The width will be clamped to min/max values
 */
void obsidian_macos_splitview_set_primary_pane_width(ObsidianSplitViewHandle handle, double width);

/**
 * Get the current primary pane width (in points)
 */
double obsidian_macos_splitview_get_primary_pane_width(ObsidianSplitViewHandle handle);

/**
 * Set minimum primary pane width (in points)
 */
void obsidian_macos_splitview_set_minimum_primary_pane_width(ObsidianSplitViewHandle handle, double width);

/**
 * Set maximum primary pane width (in points)
 */
void obsidian_macos_splitview_set_maximum_primary_pane_width(ObsidianSplitViewHandle handle, double width);

/**
 * Collapse the primary pane
 */
void obsidian_macos_splitview_collapse_primary_pane(ObsidianSplitViewHandle handle);

/**
 * Expand the primary pane (restore from collapsed state)
 */
void obsidian_macos_splitview_expand_primary_pane(ObsidianSplitViewHandle handle);

/**
 * Toggle primary pane collapse/expand state
 */
void obsidian_macos_splitview_toggle_primary_pane(ObsidianSplitViewHandle handle);

/**
 * Check if primary pane is currently collapsed
 */
bool obsidian_macos_splitview_is_primary_pane_collapsed(ObsidianSplitViewHandle handle);

/**
 * Set the primary pane content view
 * @param handle SplitView handle
 * @param viewHandle NSView handle to use as primary pane content (must be a valid NSView*)
 */
void obsidian_macos_splitview_set_primary_pane_view(ObsidianSplitViewHandle handle, void* viewHandle);

/**
 * Set the secondary pane content view
 * @param handle SplitView handle
 * @param viewHandle NSView handle to use as secondary pane content (must be a valid NSView*)
 */
void obsidian_macos_splitview_set_secondary_pane_view(ObsidianSplitViewHandle handle, void* viewHandle);

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
