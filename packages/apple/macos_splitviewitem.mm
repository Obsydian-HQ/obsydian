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

void obsidian_macos_destroy_splitviewitem(ObsidianSplitViewItemHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release the wrapper (ARC will handle cleanup)
        // __bridge_transfer transfers ownership to ARC, which will release it
        (void)(__bridge_transfer ObsidianSplitViewItemWrapper*)handle;
    }
}

} // extern "C"
