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
        [_splitViewController.splitView setVertical:YES];
        [_splitViewController.splitView setDividerStyle:NSSplitViewDividerStyleThin];
        [_splitViewController.view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    }
    return self;
}

- (void)addSplitViewItem:(void*)itemHandle {
    if (!itemHandle) return;
    
    void* actualItem = obsidian_macos_splitviewitem_get_splitviewitem(itemHandle);
    if (!actualItem) return;
    
    NSSplitViewItem* item = (__bridge NSSplitViewItem*)actualItem;
    [_splitViewController addSplitViewItem:item];
}

- (void)removeSplitViewItem:(void*)itemHandle {
    if (!itemHandle) return;
    
    void* actualItem = obsidian_macos_splitviewitem_get_splitviewitem(itemHandle);
    if (!actualItem) return;
    
    NSSplitViewItem* item = (__bridge NSSplitViewItem*)actualItem;
    [_splitViewController removeSplitViewItem:item];
}

@end

extern "C" {

ObsidianSplitViewControllerHandle obsidian_macos_create_splitviewcontroller(ObsidianSplitViewControllerParams /* params */) {
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* w = [[ObsidianSplitViewControllerWrapper alloc] init];
        return w ? (__bridge_retained void*)w : nullptr;
    }
}

void* obsidian_macos_splitviewcontroller_get_view(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)((__bridge ObsidianSplitViewControllerWrapper*)handle).splitViewController.view;
    }
}

void obsidian_macos_splitviewcontroller_add_splitviewitem(ObsidianSplitViewControllerHandle handle, void* itemHandle) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianSplitViewControllerWrapper*)handle addSplitViewItem:itemHandle];
    }
}

void obsidian_macos_splitviewcontroller_remove_splitviewitem(ObsidianSplitViewControllerHandle handle, void* itemHandle) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianSplitViewControllerWrapper*)handle removeSplitViewItem:itemHandle];
    }
}

void obsidian_macos_splitviewcontroller_add_to_window(ObsidianSplitViewControllerHandle handle, void* windowHandle) {
    if (!handle || !windowHandle) return;
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* w = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        NSWindow* window = (__bridge NSWindow*)obsidian_macos_window_get_nswindow(windowHandle);
        if (!window) return;
        
        NSSize contentSize = [[window contentView] frame].size;
        [window setContentViewController:w.splitViewController];
        [window setContentSize:contentSize];
        [w.splitViewController.splitView adjustSubviews];
        [w.splitViewController.view layoutSubtreeIfNeeded];
    }
}

bool obsidian_macos_splitviewcontroller_is_valid(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianSplitViewControllerWrapper*)handle).splitViewController != nil;
    }
}

void obsidian_macos_splitviewcontroller_set_view_frame_size(ObsidianSplitViewControllerHandle handle, double width, double height) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianSplitViewControllerWrapper* w = (__bridge ObsidianSplitViewControllerWrapper*)handle;
        [w.splitViewController.view setFrameSize:NSMakeSize(width, height)];
    }
}

void obsidian_macos_destroy_splitviewcontroller(ObsidianSplitViewControllerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianSplitViewControllerWrapper*)handle;
    }
}

}
