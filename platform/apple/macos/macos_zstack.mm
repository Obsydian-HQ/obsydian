/**
 * macOS ZStack FFI - Objective-C++ Implementation
 * 
 * Frame-based overlay layout container
 * ZStack overlays children and fills the parent view
 * NO AUTO LAYOUT - Pure frame-based positioning
 */

#import "macos_zstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass for ZStack that fills children to bounds
@interface ObsidianZStackContainerView : NSView
@end

@implementation ObsidianZStackContainerView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        // NO autoresizingMask - layout engine sets frames
    }
    return self;
}

// Layout is a no-op - layout engine sets frames
- (void)layout {
    [super layout];
    // NO AUTOMATIC LAYOUT - frames are set by the layout engine
}

- (void)addSubview:(NSView*)view {
    [super addSubview:view];
    [self setNeedsLayout:YES];
}

- (void)willRemoveSubview:(NSView*)subview {
    [super willRemoveSubview:subview];
    [self setNeedsLayout:YES];
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldSize {
    [super resizeSubviewsWithOldSize:oldSize];
    [self setNeedsLayout:YES];
}

@end

// Internal ZStack wrapper class
@interface ObsidianZStackWrapper : NSObject {
    ObsidianZStackContainerView* _containerView;
}

@property (nonatomic, strong) ObsidianZStackContainerView* containerView;

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
        NSRect frame = NSMakeRect(0, 0, 300, 200);
        _containerView = [[ObsidianZStackContainerView alloc] initWithFrame:frame];
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
    
    // FRAME-BASED: Do NOT disable translatesAutoresizingMaskIntoConstraints
    [_containerView addSubview:childView];
    [_containerView setNeedsLayout:YES];
}

- (void)removeChildView:(void*)childViewHandle {
    if (!_containerView || !childViewHandle) {
        return;
    }
    
    NSView* childView = (__bridge NSView*)childViewHandle;
    if (!childView || [childView superview] != _containerView) {
        return;
    }
    
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
    
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        
        // Fill the content view
        _containerView.frame = contentView.bounds;
        [contentView addSubview:_containerView];
        [_containerView setNeedsLayout:YES];
    }
}

- (void)removeFromParent {
    if (!_containerView) return;
    
    // IMPORTANT: Only remove the container from its parent superview.
    // Do NOT remove the container's children - they should stay attached.
    // This allows the container to be moved to a new parent without losing its contents.
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
        [wrapper removeFromParent];
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
