#import "macos_splitviewitem.h"
#import "macos_viewcontroller.h"
#import <AppKit/AppKit.h>

@interface ObsidianSplitViewItemWrapper : NSObject
@property (nonatomic, strong) NSSplitViewItem* splitViewItem;
@end

@implementation ObsidianSplitViewItemWrapper

- (instancetype)initWithSplitViewItem:(NSSplitViewItem*)item {
    self = [super init];
    if (self) {
        _splitViewItem = item;
    }
    return self;
}

@end

extern "C" {

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_sidebar_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    @autoreleasepool {
        void* vcPtr = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!vcPtr) return nullptr;
        
        NSViewController* vc = (__bridge NSViewController*)vcPtr;
        NSSplitViewItem* item = [NSSplitViewItem sidebarWithViewController:vc];
        
        if (@available(macOS 11.0, *)) {
            [item setAllowsFullHeightLayout:YES];
            [item setTitlebarSeparatorStyle:NSTitlebarSeparatorStyleNone];
        }
        [item setCanCollapse:YES];
        [item setSpringLoaded:YES];
        // CRITICAL FIX: Use PreferResizingSiblingsWithFixedSplitView to keep window size fixed
        // when sidebar collapses. The siblings (content area) will expand to fill the space.
        // Previously used PreferResizingSplitViewWithFixedSiblings which was causing the window
        // to shrink instead of the sidebar collapsing within the window.
        // Reference: VLC, CodeEdit, and Apple's recommended sidebar behavior.
        [item setCollapseBehavior:NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView];
        
        ObsidianSplitViewItemWrapper* w = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:item];
        return (__bridge_retained void*)w;
    }
}

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_content_list_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    @autoreleasepool {
        void* vcPtr = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!vcPtr) return nullptr;
        
        NSViewController* vc = (__bridge NSViewController*)vcPtr;
        NSSplitViewItem* item = [NSSplitViewItem contentListWithViewController:vc];
        
        if (@available(macOS 11.0, *)) {
            [item setAllowsFullHeightLayout:YES];
        }
        [item setCanCollapse:NO];
        
        ObsidianSplitViewItemWrapper* w = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:item];
        return (__bridge_retained void*)w;
    }
}

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_inspector_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    @autoreleasepool {
        void* vcPtr = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!vcPtr) return nullptr;
        
        NSViewController* vc = (__bridge NSViewController*)vcPtr;
        NSSplitViewItem* item = [NSSplitViewItem inspectorWithViewController:vc];
        
        if (@available(macOS 11.0, *)) {
            [item setAllowsFullHeightLayout:YES];
        }
        
        ObsidianSplitViewItemWrapper* w = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:item];
        return (__bridge_retained void*)w;
    }
}

void* obsidian_macos_splitviewitem_get_splitviewitem(ObsidianSplitViewItemHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem;
    }
}

void obsidian_macos_splitviewitem_set_minimum_thickness(ObsidianSplitViewItemHandle handle, double thickness) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.minimumThickness = thickness;
    }
}

double obsidian_macos_splitviewitem_get_minimum_thickness(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.minimumThickness;
    }
}

void obsidian_macos_splitviewitem_set_maximum_thickness(ObsidianSplitViewItemHandle handle, double thickness) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.maximumThickness = thickness;
    }
}

double obsidian_macos_splitviewitem_get_maximum_thickness(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.maximumThickness;
    }
}

void obsidian_macos_splitviewitem_set_can_collapse(ObsidianSplitViewItemHandle handle, bool canCollapse) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.canCollapse = canCollapse;
    }
}

bool obsidian_macos_splitviewitem_get_can_collapse(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.canCollapse;
    }
}

void obsidian_macos_splitviewitem_set_holding_priority(ObsidianSplitViewItemHandle handle, float priority) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.holdingPriority = priority;
    }
}

float obsidian_macos_splitviewitem_get_holding_priority(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.holdingPriority;
    }
}

void obsidian_macos_splitviewitem_set_collapsed(ObsidianSplitViewItemHandle handle, bool collapsed) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.collapsed = collapsed;
    }
}

void obsidian_macos_splitviewitem_set_collapsed_animated(ObsidianSplitViewItemHandle handle, bool collapsed) {
    if (!handle) return;
    @autoreleasepool {
        [[((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem animator] setCollapsed:collapsed];
    }
}

bool obsidian_macos_splitviewitem_get_collapsed(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.collapsed;
    }
}

void obsidian_macos_splitviewitem_set_allows_full_height_layout(ObsidianSplitViewItemHandle handle, bool allows) {
    if (!handle) return;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.allowsFullHeightLayout = allows;
        }
    }
}

bool obsidian_macos_splitviewitem_get_allows_full_height_layout(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.allowsFullHeightLayout;
        }
        return false;
    }
}

void obsidian_macos_splitviewitem_set_titlebar_separator_style(ObsidianSplitViewItemHandle handle, ObsidianTitlebarSeparatorStyle style) {
    if (!handle) return;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            NSSplitViewItem* item = ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem;
            switch (style) {
                case ObsidianTitlebarSeparatorStyleNone: item.titlebarSeparatorStyle = NSTitlebarSeparatorStyleNone; break;
                case ObsidianTitlebarSeparatorStyleLine: item.titlebarSeparatorStyle = NSTitlebarSeparatorStyleLine; break;
                case ObsidianTitlebarSeparatorStyleShadow: item.titlebarSeparatorStyle = NSTitlebarSeparatorStyleShadow; break;
                default: item.titlebarSeparatorStyle = NSTitlebarSeparatorStyleAutomatic;
            }
        }
    }
}

ObsidianTitlebarSeparatorStyle obsidian_macos_splitviewitem_get_titlebar_separator_style(ObsidianSplitViewItemHandle handle) {
    if (!handle) return ObsidianTitlebarSeparatorStyleAutomatic;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            switch (((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.titlebarSeparatorStyle) {
                case NSTitlebarSeparatorStyleNone: return ObsidianTitlebarSeparatorStyleNone;
                case NSTitlebarSeparatorStyleLine: return ObsidianTitlebarSeparatorStyleLine;
                case NSTitlebarSeparatorStyleShadow: return ObsidianTitlebarSeparatorStyleShadow;
                default: return ObsidianTitlebarSeparatorStyleAutomatic;
            }
        }
        return ObsidianTitlebarSeparatorStyleAutomatic;
    }
}

void obsidian_macos_splitviewitem_set_collapse_behavior(ObsidianSplitViewItemHandle handle, ObsidianCollapseBehavior behavior) {
    if (!handle) return;
    @autoreleasepool {
        NSSplitViewItem* item = ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem;
        switch (behavior) {
            case ObsidianCollapseBehaviorPreferResizingSplitViewWithFixedSiblings:
                item.collapseBehavior = NSSplitViewItemCollapseBehaviorPreferResizingSplitViewWithFixedSiblings; break;
            case ObsidianCollapseBehaviorPreferResizingSiblingsWithFixedSplitView:
                item.collapseBehavior = NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView; break;
            case ObsidianCollapseBehaviorUseConstraints:
                item.collapseBehavior = NSSplitViewItemCollapseBehaviorUseConstraints; break;
            default:
                item.collapseBehavior = NSSplitViewItemCollapseBehaviorDefault;
        }
    }
}

ObsidianCollapseBehavior obsidian_macos_splitviewitem_get_collapse_behavior(ObsidianSplitViewItemHandle handle) {
    if (!handle) return ObsidianCollapseBehaviorDefault;
    @autoreleasepool {
        switch (((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.collapseBehavior) {
            case NSSplitViewItemCollapseBehaviorPreferResizingSplitViewWithFixedSiblings:
                return ObsidianCollapseBehaviorPreferResizingSplitViewWithFixedSiblings;
            case NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView:
                return ObsidianCollapseBehaviorPreferResizingSiblingsWithFixedSplitView;
            case NSSplitViewItemCollapseBehaviorUseConstraints:
                return ObsidianCollapseBehaviorUseConstraints;
            default:
                return ObsidianCollapseBehaviorDefault;
        }
    }
}

void obsidian_macos_splitviewitem_set_spring_loaded(ObsidianSplitViewItemHandle handle, bool springLoaded) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.springLoaded = springLoaded;
    }
}

bool obsidian_macos_splitviewitem_get_spring_loaded(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem.springLoaded;
    }
}

bool obsidian_macos_splitviewitem_is_valid(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewItemWrapper*)handle).splitViewItem != nil;
    }
}

void obsidian_macos_destroy_splitviewitem(ObsidianSplitViewItemHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianSplitViewItemWrapper*)handle;
    }
}

}
