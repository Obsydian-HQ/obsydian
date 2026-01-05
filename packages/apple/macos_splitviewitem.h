/**
 * macOS SplitViewItem FFI - C Interface
 * 
 * Provides C interface for creating and managing NSSplitViewItem instances
 * from C++ code. This header is C-compatible and can be included from C++.
 * 
 * NSSplitViewItem represents a single pane in an NSSplitViewController.
 * The sidebar behavior provides native macOS sidebar features like material
 * background and collapse button.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS split view item
 */
typedef void* ObsidianSplitViewItemHandle;

/**
 * SplitViewItem behavior types
 */
typedef enum {
    ObsidianSplitViewItemBehaviorContentList = 0,  // Standard content list
    ObsidianSplitViewItemBehaviorSidebar = 1,     // Native sidebar (material background, collapse button)
    ObsidianSplitViewItemBehaviorInspector = 2    // Inspector pane
} ObsidianSplitViewItemBehavior;

/**
 * Create a sidebar split view item with a view controller
 * This provides native macOS sidebar behavior (material background, collapse button)
 * @param viewControllerHandle NSViewController handle
 * @return Split view item handle, or NULL on failure
 */
ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_sidebar_with_viewcontroller(void* viewControllerHandle);

/**
 * Create a content list split view item with a view controller
 * @param viewControllerHandle NSViewController handle
 * @return Split view item handle, or NULL on failure
 */
ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_content_list_with_viewcontroller(void* viewControllerHandle);

/**
 * Create an inspector split view item with a view controller
 * @param viewControllerHandle NSViewController handle
 * @return Split view item handle, or NULL on failure
 */
ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_inspector_with_viewcontroller(void* viewControllerHandle);

/**
 * Set minimum thickness for the split view item
 * @param handle Split view item handle
 * @param thickness Minimum thickness in points
 */
void obsidian_macos_splitviewitem_set_minimum_thickness(ObsidianSplitViewItemHandle handle, double thickness);

/**
 * Get minimum thickness for the split view item
 * @param handle Split view item handle
 * @return Minimum thickness in points
 */
double obsidian_macos_splitviewitem_get_minimum_thickness(ObsidianSplitViewItemHandle handle);

/**
 * Set maximum thickness for the split view item
 * @param handle Split view item handle
 * @param thickness Maximum thickness in points
 */
void obsidian_macos_splitviewitem_set_maximum_thickness(ObsidianSplitViewItemHandle handle, double thickness);

/**
 * Get maximum thickness for the split view item
 * @param handle Split view item handle
 * @return Maximum thickness in points
 */
double obsidian_macos_splitviewitem_get_maximum_thickness(ObsidianSplitViewItemHandle handle);

/**
 * Set whether the split view item is collapsed
 * @param handle Split view item handle
 * @param collapsed true to collapse, false to expand
 */
void obsidian_macos_splitviewitem_set_collapsed(ObsidianSplitViewItemHandle handle, bool collapsed);

/**
 * Get whether the split view item is collapsed
 * @param handle Split view item handle
 * @return true if collapsed, false if expanded
 */
bool obsidian_macos_splitviewitem_get_collapsed(ObsidianSplitViewItemHandle handle);

/**
 * Check if split view item is valid
 */
bool obsidian_macos_splitviewitem_is_valid(ObsidianSplitViewItemHandle handle);

/**
 * Get the actual NSSplitViewItem* from the wrapper handle
 * This is for internal use by SplitViewController to add/remove items
 * @param handle Split view item wrapper handle
 * @return The actual NSSplitViewItem* (bridged, caller should not release)
 */
void* obsidian_macos_splitviewitem_get_splitviewitem(ObsidianSplitViewItemHandle handle);

/**
 * Destroy the split view item
 */
void obsidian_macos_destroy_splitviewitem(ObsidianSplitViewItemHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
