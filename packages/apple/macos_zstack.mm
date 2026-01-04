/**
 * macOS ZStack FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSView APIs for container view management
 * ZStack overlays children and fills the parent view (unlike VStack/HStack which size to content)
 */

#import "macos_zstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal ZStack wrapper class
// ZStack container view fills the parent (unlike VStack/HStack which size to content)
@interface ObsidianZStackWrapper : NSObject {
    NSView* _containerView;
}

@property (nonatomic, strong) NSView* containerView;

- (instancetype)initWithParams:(ObsidianZStackParams)params;
- (void*)getView;
- (void)addChildView:(void*)childViewHandle;
- (void)removeChildView:(void*)childViewHandle;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianZStackWrapper

- (instancetype)initWithParams:(ObsidianZStackParams)params {
    self = [super init];
    if (self) {
        // Create container view with zero frame (will be constrained to fill parent)
        // ZStack fills the parent, unlike VStack/HStack which size to content
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _containerView = [[NSView alloc] initWithFrame:frame];
        
        // Disable autoresizing mask - we'll use Auto Layout
        [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Note: No background color set - container should be transparent
        // Users can add their own background if needed
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_containerView;
}

- (void)addChildView:(void*)childViewHandle {
    if (!_containerView || !childViewHandle) {
        return;
    }
    
    NSView* childView = (__bridge NSView*)childViewHandle;
    if (!childView) {
        return;
    }
    
    // CRITICAL: Disable autoresizing mask BEFORE adding to superview
    // This prevents frame-based positioning from interfering with Auto Layout
    [childView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add child view as subview
    // Z-order: last added subview is on top (this is NSView's default behavior)
    [_containerView addSubview:childView];
    
    // Force layout update to ensure constraints are applied immediately
    [_containerView setNeedsLayout:YES];
    [_containerView layoutSubtreeIfNeeded];
}

- (void)removeChildView:(void*)childViewHandle {
    if (!_containerView || !childViewHandle) {
        return;
    }
    
    NSView* childView = (__bridge NSView*)childViewHandle;
    if (!childView || [childView superview] != _containerView) {
        return;
    }
    
    // Remove from superview
    [childView removeFromSuperview];
}

- (void)setVisible:(bool)visible {
    if (_containerView) {
        [_containerView setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_containerView) {
        return ![_containerView isHidden];
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_containerView || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_containerView];
        
        // ZStack container should fill the parent view
        // Pin to all edges of content view
        NSDictionary* views = @{@"zstack": _containerView};
        NSArray* constraints = @[
            [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[zstack]|"
                                                     options:0
                                                     metrics:nil
                                                       views:views],
            [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[zstack]|"
                                                     options:0
                                                     metrics:nil
                                                       views:views]
        ];
        [NSLayoutConstraint activateConstraints:[constraints valueForKeyPath:@"@unionOfArrays.self"]];
    }
}

- (void)removeFromParent {
    if (!_containerView) return;
    
    // Remove all child views first
    NSArray<NSView*>* subviews = [_containerView.subviews copy];
    for (NSView* subview in subviews) {
        [subview removeFromSuperview];
    }
    
    // Remove from superview
    if ([_containerView superview]) {
        [_containerView removeFromSuperview];
    }
}

@end

// C interface implementation
extern "C" {

ObsidianZStackHandle obsidian_macos_create_zstack(ObsidianZStackParams params) {
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = [[ObsidianZStackWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.containerView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void* obsidian_macos_zstack_get_view(ObsidianZStackHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_zstack_add_child_view(ObsidianZStackHandle zstackHandle,
                                          void* childViewHandle) {
    if (!zstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)zstackHandle;
        [wrapper addChildView:childViewHandle];
    }
}

void obsidian_macos_zstack_remove_child_view(ObsidianZStackHandle zstackHandle,
                                              void* childViewHandle) {
    if (!zstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)zstackHandle;
        [wrapper removeChildView:childViewHandle];
    }
}

void obsidian_macos_zstack_set_visible(ObsidianZStackHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_zstack_is_visible(ObsidianZStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_zstack_add_to_window(ObsidianZStackHandle zstackHandle,
                                         void* windowHandle) {
    if (!zstackHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)zstackHandle;
        [wrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_zstack_remove_from_parent(ObsidianZStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_zstack(ObsidianZStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge_transfer ObsidianZStackWrapper*)handle;
        // Remove from parent before destroying
        [wrapper removeFromParent];
        // wrapper will be released when exiting @autoreleasepool
    }
}

bool obsidian_macos_zstack_is_valid(ObsidianZStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        return wrapper != nil && wrapper.containerView != nil;
    }
}

void obsidian_macos_zstack_request_layout_update(ObsidianZStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianZStackWrapper* wrapper = (__bridge ObsidianZStackWrapper*)handle;
        if (wrapper.containerView) {
            [wrapper.containerView setNeedsLayout:YES];
            [wrapper.containerView layoutSubtreeIfNeeded];
        }
    }
}

} // extern "C"
