/**
 * macOS SplitViewController FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSSplitViewController APIs
 */

#import "macos_splitviewcontroller.h"
#import "macos_splitviewitem.h"  // For obsidian_macos_splitviewitem_get_splitviewitem
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal split view controller wrapper class
@interface ObsidianSplitViewControllerWrapper : NSObject {
    NSSplitViewController* _splitViewController;
}

@property (nonatomic, strong) NSSplitViewController* splitViewController;

- (instancetype)initWithParams:(ObsidianSplitViewControllerParams)params;
- (void*)getView;
- (void)addSplitViewItem:(void*)splitViewItemHandle;
- (void)removeSplitViewItem:(void*)splitViewItemHandle;
- (bool)isValid;

@end

@implementation ObsidianSplitViewControllerWrapper

- (instancetype)initWithParams:(ObsidianSplitViewControllerParams)params {
    self = [super init];
    if (self) {
        // Create NSSplitViewController
        _splitViewController = [[NSSplitViewController alloc] init];
    }
    return self;
}

- (void*)getView {
    if (!_splitViewController) return nullptr;
    
    NSView* view = [_splitViewController view];
    if (!view) return nullptr;
    
    // Return the view as an opaque handle
    // Note: This is a borrowed reference, caller should not release it
    return (__bridge void*)view;
}

- (void)addSplitViewItem:(void*)splitViewItemHandle {
    if (!_splitViewController || !splitViewItemHandle) return;
    
    // The handle is an ObsidianSplitViewItemWrapper*, we need to get the actual NSSplitViewItem*
    void* actualSplitViewItem = obsidian_macos_splitviewitem_get_splitviewitem(splitViewItemHandle);
    if (!actualSplitViewItem) return;
    
    NSSplitViewItem* splitViewItem = (__bridge NSSplitViewItem*)actualSplitViewItem;
    if (splitViewItem) {
        [_splitViewController addSplitViewItem:splitViewItem];
    }
}

- (void)removeSplitViewItem:(void*)splitViewItemHandle {
    if (!_splitViewController || !splitViewItemHandle) return;
    
    // The handle is an ObsidianSplitViewItemWrapper*, we need to get the actual NSSplitViewItem*
    void* actualSplitViewItem = obsidian_macos_splitviewitem_get_splitviewitem(splitViewItemHandle);
    if (!actualSplitViewItem) return;
    
    NSSplitViewItem* splitViewItem = (__bridge NSSplitViewItem*)actualSplitViewItem;
    if (splitViewItem) {
        [_splitViewController removeSplitViewItem:splitViewItem];
    }
}

- (void)addToWindow:(void*)windowHandle {
    if (!_splitViewController || !windowHandle) return;
    
    // The windowHandle is an ObsidianWindowWrapper, get the actual NSWindow
    // We use obsidian_macos_window_get_content_view to get the content view
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentViewPtr) return;
    
    NSView* contentView = (__bridge NSView*)contentViewPtr;
    NSView* splitView = [_splitViewController view];
    if (!splitView) return;
    
    // Remove split view from its current parent if it has one
    if ([splitView superview]) {
        [splitView removeFromSuperview];
    }
    
    // Add split view to window's content view
    [contentView addSubview:splitView];
    
    // Set up Auto Layout constraints to fill the content view
    [splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [NSLayoutConstraint activateConstraints:@[
        [splitView.leadingAnchor constraintEqualToAnchor:contentView.leadingAnchor],
        [splitView.trailingAnchor constraintEqualToAnchor:contentView.trailingAnchor],
        [splitView.topAnchor constraintEqualToAnchor:contentView.topAnchor],
        [splitView.bottomAnchor constraintEqualToAnchor:contentView.bottomAnchor]
    ]];
}

- (bool)isValid {
    return _splitViewController != nil;
}

- (NSSplitViewController*)splitViewController {
    return _splitViewController;
}

@end

// C interface implementation
extern "C" {

ObsidianSplitViewControllerHandle obsidian_macos_create_splitviewcontroller(ObsidianSplitViewControllerParams params) {
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = [[ObsidianSplitViewControllerWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.splitViewController) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void* obsidian_macos_splitviewcontroller_get_view(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_splitviewcontroller_add_splitviewitem(ObsidianSplitViewControllerHandle handle,
                                                          void* splitViewItemHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        [wrapper addSplitViewItem:splitViewItemHandle];
    }
}

void obsidian_macos_splitviewcontroller_remove_splitviewitem(ObsidianSplitViewControllerHandle handle,
                                                             void* splitViewItemHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        [wrapper removeSplitViewItem:splitViewItemHandle];
    }
}

void obsidian_macos_splitviewcontroller_add_to_window(ObsidianSplitViewControllerHandle handle,
                                                      void* windowHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        [wrapper addToWindow:windowHandle];
    }
}

bool obsidian_macos_splitviewcontroller_is_valid(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        return [wrapper isValid];
    }
}

void obsidian_macos_destroy_splitviewcontroller(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release the wrapper (ARC will handle cleanup)
        // __bridge_transfer transfers ownership to ARC, which will release it
        (void)(__bridge_transfer ObsidianSplitViewControllerWrapper*)handle;
    }
}

} // extern "C"
