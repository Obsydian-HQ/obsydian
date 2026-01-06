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
 * Set whether the split view item allows full-height layout (macOS 11+)
 * When true, the sidebar extends into the title bar area for a seamless look.
 * This is the default for sidebar items created with sidebarWithViewController:.
 * @param handle Split view item handle
 * @param allowed true to allow full-height layout
 */
void obsidian_macos_splitviewitem_set_allows_full_height_layout(ObsidianSplitViewItemHandle handle, bool allowed);

/**
 * Get whether the split view item allows full-height layout (macOS 11+)
 * @param handle Split view item handle
 * @return true if full-height layout is allowed
 */
bool obsidian_macos_splitviewitem_get_allows_full_height_layout(ObsidianSplitViewItemHandle handle);

/**
 * Set whether the split view item can collapse
 * @param handle Split view item handle
 * @param canCollapse true to allow collapsing
 */
void obsidian_macos_splitviewitem_set_can_collapse(ObsidianSplitViewItemHandle handle, bool canCollapse);

/**
 * Get whether the split view item can collapse
 * @param handle Split view item handle
 * @return true if can collapse
 */
bool obsidian_macos_splitviewitem_get_can_collapse(ObsidianSplitViewItemHandle handle);

/**
 * Set the holding priority for the split view item
 * Higher priority items resist being resized when other items are resized.
 * Default is NSLayoutPriorityDefaultLow (250).
 * @param handle Split view item handle
 * @param priority Layout priority (0-1000)
 */
void obsidian_macos_splitviewitem_set_holding_priority(ObsidianSplitViewItemHandle handle, float priority);

/**
 * Get the holding priority for the split view item
 * @param handle Split view item handle
 * @return Layout priority (0-1000)
 */
float obsidian_macos_splitviewitem_get_holding_priority(ObsidianSplitViewItemHandle handle);

/**
 * Titlebar separator style for the split view item
 */
typedef enum {
    ObsidianTitlebarSeparatorStyleAutomatic = 0,
    ObsidianTitlebarSeparatorStyleNone = 1,
    ObsidianTitlebarSeparatorStyleLine = 2,
    ObsidianTitlebarSeparatorStyleShadow = 3
} ObsidianTitlebarSeparatorStyle;

/**
 * Set the titlebar separator style for the split view item (macOS 11+)
 * Controls the visual separator between the titlebar and the item's content.
 * - None: No separator (typically used for sidebars)
 * - Line: A thin line separator (typically used for main content)
 * - Automatic: System decides based on context
 * @param handle Split view item handle
 * @param style Titlebar separator style
 */
void obsidian_macos_splitviewitem_set_titlebar_separator_style(ObsidianSplitViewItemHandle handle, ObsidianTitlebarSeparatorStyle style);

/**
 * Get the titlebar separator style for the split view item
 * @param handle Split view item handle
 * @return Titlebar separator style
 */
ObsidianTitlebarSeparatorStyle obsidian_macos_splitviewitem_get_titlebar_separator_style(ObsidianSplitViewItemHandle handle);

/**
 * Collapse behavior for the split view item
 */
typedef enum {
    ObsidianCollapseBehaviorDefault = 0,
    ObsidianCollapseBehaviorPreferResizingSplitViewWithFixedSiblings = 1,
    ObsidianCollapseBehaviorPreferResizingSiblingsWithFixedSplitView = 2,
    ObsidianCollapseBehaviorUseConstraints = 3  // Recommended for proper animation
} ObsidianCollapseBehavior;

/**
 * Set the collapse behavior for the split view item
 * UseConstraints is recommended for proper collapse animation.
 * @param handle Split view item handle
 * @param behavior Collapse behavior
 */
void obsidian_macos_splitviewitem_set_collapse_behavior(ObsidianSplitViewItemHandle handle, ObsidianCollapseBehavior behavior);

/**
 * Get the collapse behavior for the split view item
 * @param handle Split view item handle
 * @return Collapse behavior
 */
ObsidianCollapseBehavior obsidian_macos_splitviewitem_get_collapse_behavior(ObsidianSplitViewItemHandle handle);

/**
 * Set whether the split view item is spring loaded
 * When spring loaded, dragging items over the collapsed item will reveal it.
 * @param handle Split view item handle
 * @param springLoaded true to enable spring loading
 */
void obsidian_macos_splitviewitem_set_spring_loaded(ObsidianSplitViewItemHandle handle, bool springLoaded);

/**
 * Get whether the split view item is spring loaded
 * @param handle Split view item handle
 * @return true if spring loaded
 */
bool obsidian_macos_splitviewitem_get_spring_loaded(ObsidianSplitViewItemHandle handle);

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
