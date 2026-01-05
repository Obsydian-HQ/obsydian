/**
 * macOS SplitViewItem FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSSplitViewItem APIs
 */

#import "macos_splitviewitem.h"
#import "macos_viewcontroller.h"  // For obsidian_macos_viewcontroller_get_viewcontroller
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal split view item wrapper class
@interface ObsidianSplitViewItemWrapper : NSObject {
    NSSplitViewItem* _splitViewItem;
}

@property (nonatomic, strong) NSSplitViewItem* splitViewItem;

- (instancetype)initWithSplitViewItem:(NSSplitViewItem*)splitViewItem;
- (void)setMinimumThickness:(double)thickness;
- (double)getMinimumThickness;
- (void)setMaximumThickness:(double)thickness;
- (double)getMaximumThickness;
- (void)setCollapsed:(bool)collapsed;
- (bool)getCollapsed;
- (bool)isValid;

@end

@implementation ObsidianSplitViewItemWrapper

- (instancetype)initWithSplitViewItem:(NSSplitViewItem*)splitViewItem {
    self = [super init];
    if (self) {
        _splitViewItem = splitViewItem;
        
        // Configure default sidebar behavior for proper native appearance
        if (_splitViewItem) {
            // Enable full-height layout by default for sidebar items (macOS 11+)
            if (@available(macOS 11.0, *)) {
                [_splitViewItem setAllowsFullHeightLayout:YES];
                NSLog(@"DEBUG: Set allowsFullHeightLayout = YES for split view item");
            }
            
            // Allow collapsing by default
            [_splitViewItem setCanCollapse:YES];
            NSLog(@"DEBUG: Set canCollapse = YES for split view item");
        }
    }
    return self;
}

- (void)setMinimumThickness:(double)thickness {
    if (_splitViewItem) {
        [_splitViewItem setMinimumThickness:thickness];
    }
}

- (double)getMinimumThickness {
    if (_splitViewItem) {
        return [_splitViewItem minimumThickness];
    }
    return 0.0;
}

- (void)setMaximumThickness:(double)thickness {
    if (_splitViewItem) {
        [_splitViewItem setMaximumThickness:thickness];
    }
}

- (double)getMaximumThickness {
    if (_splitViewItem) {
        return [_splitViewItem maximumThickness];
    }
    return 0.0;
}

- (void)setCollapsed:(bool)collapsed {
    if (_splitViewItem) {
        [_splitViewItem setCollapsed:collapsed];
    }
}

- (bool)getCollapsed {
    if (_splitViewItem) {
        return [_splitViewItem isCollapsed];
    }
    return false;
}

- (bool)isValid {
    return _splitViewItem != nil;
}

- (NSSplitViewItem*)splitViewItem {
    return _splitViewItem;
}

@end

// C interface implementation
extern "C" {

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_sidebar_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    
    @autoreleasepool {
        // The handle is an ObsidianViewControllerWrapper*, we need to get the actual NSViewController*
        void* actualViewController = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!actualViewController) return nullptr;
        
        NSViewController* viewController = (__bridge NSViewController*)actualViewController;
        if (!viewController) return nullptr;
        
        // Create sidebar split view item using the class method
        // This provides native macOS sidebar behavior (material background, collapse button)
        NSSplitViewItem* splitViewItem = [NSSplitViewItem sidebarWithViewController:viewController];
        if (!splitViewItem) return nullptr;
        
        // CRITICAL: Configure sidebar-specific behavior based on CodeEdit's implementation
        // titlebarSeparatorStyle = .none for sidebar (no line under titlebar in sidebar area)
        if (@available(macOS 11.0, *)) {
            [splitViewItem setTitlebarSeparatorStyle:NSTitlebarSeparatorStyleNone];
            NSLog(@"DEBUG: Set titlebarSeparatorStyle = none for sidebar item");
        }
        
        // isSpringLoaded = true for better UX when dragging items
        [splitViewItem setSpringLoaded:YES];
        NSLog(@"DEBUG: Set springLoaded = YES for sidebar item");
        
        // collapseBehavior = useConstraints for proper collapse animation
        [splitViewItem setCollapseBehavior:NSSplitViewItemCollapseBehaviorUseConstraints];
        NSLog(@"DEBUG: Set collapseBehavior = useConstraints for sidebar item");
        
        // Wrap the split view item
        ObsidianSplitViewItemWrapper* wrapper = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:splitViewItem];
        if (wrapper && wrapper.splitViewItem) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_content_list_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    
    @autoreleasepool {
        // The handle is an ObsidianViewControllerWrapper*, we need to get the actual NSViewController*
        void* actualViewController = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!actualViewController) return nullptr;
        
        NSViewController* viewController = (__bridge NSViewController*)actualViewController;
        if (!viewController) return nullptr;
        
        // Create content list split view item
        NSSplitViewItem* splitViewItem = [NSSplitViewItem splitViewItemWithViewController:viewController];
        if (!splitViewItem) return nullptr;
        
        // CRITICAL: Configure content-specific behavior based on CodeEdit's implementation
        // titlebarSeparatorStyle = .line for main content (visible line under titlebar)
        if (@available(macOS 11.0, *)) {
            [splitViewItem setTitlebarSeparatorStyle:NSTitlebarSeparatorStyleLine];
            NSLog(@"DEBUG: Set titlebarSeparatorStyle = line for content item");
        }
        
        // Wrap the split view item
        ObsidianSplitViewItemWrapper* wrapper = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:splitViewItem];
        if (wrapper && wrapper.splitViewItem) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

ObsidianSplitViewItemHandle obsidian_macos_splitviewitem_inspector_with_viewcontroller(void* viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    
    @autoreleasepool {
        // The handle is an ObsidianViewControllerWrapper*, we need to get the actual NSViewController*
        void* actualViewController = obsidian_macos_viewcontroller_get_viewcontroller(viewControllerHandle);
        if (!actualViewController) return nullptr;
        
        NSViewController* viewController = (__bridge NSViewController*)actualViewController;
        if (!viewController) return nullptr;
        
        // Create inspector split view item
        NSSplitViewItem* splitViewItem = [NSSplitViewItem inspectorWithViewController:viewController];
        if (!splitViewItem) return nullptr;
        
        // Wrap the split view item
        ObsidianSplitViewItemWrapper* wrapper = [[ObsidianSplitViewItemWrapper alloc] initWithSplitViewItem:splitViewItem];
        if (wrapper && wrapper.splitViewItem) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_splitviewitem_set_minimum_thickness(ObsidianSplitViewItemHandle handle, double thickness) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        [wrapper setMinimumThickness:thickness];
    }
}

double obsidian_macos_splitviewitem_get_minimum_thickness(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        return [wrapper getMinimumThickness];
    }
}

void obsidian_macos_splitviewitem_set_maximum_thickness(ObsidianSplitViewItemHandle handle, double thickness) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        [wrapper setMaximumThickness:thickness];
    }
}

double obsidian_macos_splitviewitem_get_maximum_thickness(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        return [wrapper getMaximumThickness];
    }
}

void obsidian_macos_splitviewitem_set_collapsed(ObsidianSplitViewItemHandle handle, bool collapsed) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        [wrapper setCollapsed:collapsed];
    }
}

bool obsidian_macos_splitviewitem_get_collapsed(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        return [wrapper getCollapsed];
    }
}

bool obsidian_macos_splitviewitem_is_valid(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        return [wrapper isValid];
    }
}

void obsidian_macos_splitviewitem_set_allows_full_height_layout(ObsidianSplitViewItemHandle handle, bool allowed) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            if (@available(macOS 11.0, *)) {
                [wrapper.splitViewItem setAllowsFullHeightLayout:allowed];
                NSLog(@"DEBUG: Set allowsFullHeightLayout = %d", allowed);
            } else {
                NSLog(@"WARNING: allowsFullHeightLayout requires macOS 11.0+");
            }
        }
    }
}

bool obsidian_macos_splitviewitem_get_allows_full_height_layout(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            if (@available(macOS 11.0, *)) {
                return [wrapper.splitViewItem allowsFullHeightLayout];
            }
        }
        return false;
    }
}

void obsidian_macos_splitviewitem_set_can_collapse(ObsidianSplitViewItemHandle handle, bool canCollapse) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            [wrapper.splitViewItem setCanCollapse:canCollapse];
            NSLog(@"DEBUG: Set canCollapse = %d", canCollapse);
        }
    }
}

bool obsidian_macos_splitviewitem_get_can_collapse(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            return [wrapper.splitViewItem canCollapse];
        }
        return false;
    }
}

void obsidian_macos_splitviewitem_set_holding_priority(ObsidianSplitViewItemHandle handle, float priority) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            [wrapper.splitViewItem setHoldingPriority:priority];
            NSLog(@"DEBUG: Set holdingPriority = %.0f", priority);
        }
    }
}

float obsidian_macos_splitviewitem_get_holding_priority(ObsidianSplitViewItemHandle handle) {
    if (!handle) return 0.0f;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            return [wrapper.splitViewItem holdingPriority];
        }
        return 0.0f;
    }
}

void* obsidian_macos_splitviewitem_get_splitviewitem(ObsidianSplitViewItemHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (!wrapper || !wrapper.splitViewItem) return nullptr;
        
        // Return the actual NSSplitViewItem* as an opaque handle
        // This is a borrowed reference, caller should not release it
        return (__bridge void*)wrapper.splitViewItem;
    }
}

void obsidian_macos_splitviewitem_set_titlebar_separator_style(ObsidianSplitViewItemHandle handle, ObsidianTitlebarSeparatorStyle style) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            if (@available(macOS 11.0, *)) {
                NSTitlebarSeparatorStyle nsStyle;
                switch (style) {
                    case ObsidianTitlebarSeparatorStyleNone:
                        nsStyle = NSTitlebarSeparatorStyleNone;
                        break;
                    case ObsidianTitlebarSeparatorStyleLine:
                        nsStyle = NSTitlebarSeparatorStyleLine;
                        break;
                    case ObsidianTitlebarSeparatorStyleShadow:
                        nsStyle = NSTitlebarSeparatorStyleShadow;
                        break;
                    case ObsidianTitlebarSeparatorStyleAutomatic:
                    default:
                        nsStyle = NSTitlebarSeparatorStyleAutomatic;
                        break;
                }
                [wrapper.splitViewItem setTitlebarSeparatorStyle:nsStyle];
                NSLog(@"DEBUG: Set titlebarSeparatorStyle = %ld for split view item", (long)nsStyle);
            } else {
                NSLog(@"WARNING: titlebarSeparatorStyle requires macOS 11.0+");
            }
        }
    }
}

ObsidianTitlebarSeparatorStyle obsidian_macos_splitviewitem_get_titlebar_separator_style(ObsidianSplitViewItemHandle handle) {
    if (!handle) return ObsidianTitlebarSeparatorStyleAutomatic;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            if (@available(macOS 11.0, *)) {
                NSTitlebarSeparatorStyle nsStyle = [wrapper.splitViewItem titlebarSeparatorStyle];
                switch (nsStyle) {
                    case NSTitlebarSeparatorStyleNone:
                        return ObsidianTitlebarSeparatorStyleNone;
                    case NSTitlebarSeparatorStyleLine:
                        return ObsidianTitlebarSeparatorStyleLine;
                    case NSTitlebarSeparatorStyleShadow:
                        return ObsidianTitlebarSeparatorStyleShadow;
                    case NSTitlebarSeparatorStyleAutomatic:
                    default:
                        return ObsidianTitlebarSeparatorStyleAutomatic;
                }
            }
        }
        return ObsidianTitlebarSeparatorStyleAutomatic;
    }
}

void obsidian_macos_splitviewitem_set_collapse_behavior(ObsidianSplitViewItemHandle handle, ObsidianCollapseBehavior behavior) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            NSSplitViewItemCollapseBehavior nsBehavior;
            switch (behavior) {
                case ObsidianCollapseBehaviorPreferResizingSplitViewWithFixedSiblings:
                    nsBehavior = NSSplitViewItemCollapseBehaviorPreferResizingSplitViewWithFixedSiblings;
                    break;
                case ObsidianCollapseBehaviorPreferResizingSiblingsWithFixedSplitView:
                    nsBehavior = NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView;
                    break;
                case ObsidianCollapseBehaviorUseConstraints:
                    nsBehavior = NSSplitViewItemCollapseBehaviorUseConstraints;
                    break;
                case ObsidianCollapseBehaviorDefault:
                default:
                    nsBehavior = NSSplitViewItemCollapseBehaviorDefault;
                    break;
            }
            [wrapper.splitViewItem setCollapseBehavior:nsBehavior];
            NSLog(@"DEBUG: Set collapseBehavior = %ld for split view item", (long)nsBehavior);
        }
    }
}

ObsidianCollapseBehavior obsidian_macos_splitviewitem_get_collapse_behavior(ObsidianSplitViewItemHandle handle) {
    if (!handle) return ObsidianCollapseBehaviorDefault;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            NSSplitViewItemCollapseBehavior nsBehavior = [wrapper.splitViewItem collapseBehavior];
            switch (nsBehavior) {
                case NSSplitViewItemCollapseBehaviorPreferResizingSplitViewWithFixedSiblings:
                    return ObsidianCollapseBehaviorPreferResizingSplitViewWithFixedSiblings;
                case NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView:
                    return ObsidianCollapseBehaviorPreferResizingSiblingsWithFixedSplitView;
                case NSSplitViewItemCollapseBehaviorUseConstraints:
                    return ObsidianCollapseBehaviorUseConstraints;
                case NSSplitViewItemCollapseBehaviorDefault:
                default:
                    return ObsidianCollapseBehaviorDefault;
            }
        }
        return ObsidianCollapseBehaviorDefault;
    }
}

void obsidian_macos_splitviewitem_set_spring_loaded(ObsidianSplitViewItemHandle handle, bool springLoaded) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            [wrapper.splitViewItem setSpringLoaded:springLoaded];
            NSLog(@"DEBUG: Set springLoaded = %d for split view item", springLoaded);
        }
    }
}

bool obsidian_macos_splitviewitem_get_spring_loaded(ObsidianSplitViewItemHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewItemWrapper* wrapper = (__bridge ObsidianSplitViewItemWrapper*)handle;
        if (wrapper && wrapper.splitViewItem) {
            return [wrapper.splitViewItem isSpringLoaded];
        }
        return false;
    }
}

void obsidian_macos_destroy_splitviewitem(ObsidianSplitViewItemHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release the wrapper (ARC will handle cleanup)
        // __bridge_transfer transfers ownership to ARC, which will release it
        (void)(__bridge_transfer ObsidianSplitViewItemWrapper*)handle;
    }
}

} // extern "C"
