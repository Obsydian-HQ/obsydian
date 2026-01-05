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
        
        // CRITICAL: Set preferredContentSize to prevent macOS from inferring minimal size
        // during makeKeyAndOrderFront:. NSSplitViewController's view has no intrinsic
        // content size (-1, -1) because it's a container view controller. Without
        // preferredContentSize, macOS will default to a minimal size (128px height).
        //
        // preferredContentSize is the PROPER way to tell macOS what size the view
        // controller wants. This is not a workaround - it's the intended API for
        // container view controllers that don't have intrinsic content sizes.
        //
        // We use a reasonable default size (1200x800) that matches typical window sizes.
        // The actual window size will be set explicitly via the window's frame, but
        // this prevents macOS from shrinking the window during makeKeyAndOrderFront:.
        NSSize preferredSize = NSMakeSize(1200, 800);
        [_splitViewController setPreferredContentSize:preferredSize];
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
    
    // DEBUG: Get window size to set proper preferredContentSize
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        NSRect contentRect = [contentView frame];
        NSSize contentSize = contentRect.size;
        
        // Update preferredContentSize to match actual window content size
        // This must be done BEFORE setContentViewController: to prevent shrinking
        if (contentSize.width > 0 && contentSize.height > 0) {
            NSSize currentPreferred = [_splitViewController preferredContentSize];
            if (currentPreferred.width != contentSize.width || currentPreferred.height != contentSize.height) {
                [_splitViewController setPreferredContentSize:contentSize];
                NSLog(@"DEBUG: Updated NSSplitViewController preferredContentSize from (%.0f, %.0f) to (%.0f, %.0f)",
                      currentPreferred.width, currentPreferred.height,
                      contentSize.width, contentSize.height);
            }
        }
    }
    
    // CRITICAL: Use setContentViewController: instead of addSubview:
    // This is REQUIRED for native sidebar behavior:
    // 1. Collapse button appears automatically
    // 2. Proper window resizing behavior (vertical and horizontal)
    // 3. Correct view controller lifecycle management
    // 4. Window size constraints work properly
    //
    // When using setContentViewController:, macOS automatically:
    // - Sets up the view controller's view as the window's content view
    // - Manages the view controller's lifecycle
    // - Handles window resizing based on preferredContentSize
    // - Enables native sidebar features (collapse button, etc.)
    obsidian_macos_window_set_content_view_controller(windowHandle, (__bridge void*)_splitViewController);
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
