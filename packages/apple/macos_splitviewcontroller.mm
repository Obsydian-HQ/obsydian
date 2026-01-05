/**
 * macOS SplitViewController FFI - Objective-C++ Implementation
 * 
 * Simple implementation based on real working apps (Watt editor, HelloAppKit, etc.)
 * The key insight: set the view frame size BEFORE adding items. That's it.
 */

#import "macos_splitviewcontroller.h"
#import "macos_splitviewitem.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>

@interface ObsidianSplitViewControllerWrapper : NSObject
@property (nonatomic, strong) NSSplitViewController* splitViewController;
@end

@implementation ObsidianSplitViewControllerWrapper

- (instancetype)init {
    self = [super init];
    if (self) {
        _splitViewController = [[NSSplitViewController alloc] init];
    }
    return self;
}

- (void)setViewFrameSize:(CGFloat)width height:(CGFloat)height {
    if (!_splitViewController) return;
    
    // This is what real apps do (from Watt editor):
    // view.frame.size = CGSize(width: 800, height: 600)
    // Set the frame BEFORE adding any items.
    NSView* view = [_splitViewController view];
    [view setFrame:NSMakeRect(0, 0, width, height)];
}

- (void*)getView {
    if (!_splitViewController) return nullptr;
    return (__bridge void*)[_splitViewController view];
}

- (void)addSplitViewItem:(void*)splitViewItemHandle {
    if (!_splitViewController || !splitViewItemHandle) return;
    
    void* actualSplitViewItem = obsidian_macos_splitviewitem_get_splitviewitem(splitViewItemHandle);
    if (!actualSplitViewItem) return;
    
    NSSplitViewItem* splitViewItem = (__bridge NSSplitViewItem*)actualSplitViewItem;
    [_splitViewController addSplitViewItem:splitViewItem];
}

- (void)removeSplitViewItem:(void*)splitViewItemHandle {
    if (!_splitViewController || !splitViewItemHandle) return;
    
    void* actualSplitViewItem = obsidian_macos_splitviewitem_get_splitviewitem(splitViewItemHandle);
    if (!actualSplitViewItem) return;
    
    NSSplitViewItem* splitViewItem = (__bridge NSSplitViewItem*)actualSplitViewItem;
    [_splitViewController removeSplitViewItem:splitViewItem];
}

- (void)addToWindow:(void*)windowHandle {
    if (!_splitViewController || !windowHandle) return;
    
    // Simply set as content view controller - that's what real apps do
    obsidian_macos_window_set_content_view_controller(windowHandle, (__bridge void*)_splitViewController);
}

@end

// C interface implementation
extern "C" {

ObsidianSplitViewControllerHandle obsidian_macos_create_splitviewcontroller(ObsidianSplitViewControllerParams params) {
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = [[ObsidianSplitViewControllerWrapper alloc] init];
        if (wrapper && wrapper.splitViewController) {
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
        return wrapper.splitViewController != nil;
    }
}

void obsidian_macos_splitviewcontroller_set_view_frame_size(ObsidianSplitViewControllerHandle handle,
                                                            double width, double height) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* wrapper = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        [wrapper setViewFrameSize:width height:height];
    }
}

void obsidian_macos_destroy_splitviewcontroller(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianSplitViewControllerWrapper*)handle;
    }
}

} // extern "C"
