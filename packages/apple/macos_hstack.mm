/**
 * macOS HStack FFI - Objective-C++ Implementation
 * 
 * DUMB CONTAINER: This view does NOT compute layout.
 * The Layout Engine will compute positions/sizes and set frames directly.
 */

#import "macos_hstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Simple container view - does NOT compute layout
@interface ObsidianHStackContainerView : NSView {
    CGFloat _paddingTop;
    CGFloat _paddingBottom;
    CGFloat _paddingLeading;
    CGFloat _paddingTrailing;
    CGFloat _spacing;
}

- (instancetype)initWithFrame:(NSRect)frameRect;
- (void)setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing;
- (void)setSpacing:(CGFloat)spacing;

// Getters for layout engine to read
- (CGFloat)paddingTop;
- (CGFloat)paddingBottom;
- (CGFloat)paddingLeading;
- (CGFloat)paddingTrailing;
- (CGFloat)spacing;

@end

@implementation ObsidianHStackContainerView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        _paddingTop = 0.0;
        _paddingBottom = 0.0;
        _paddingLeading = 0.0;
        _paddingTrailing = 0.0;
        _spacing = 0.0;
        // NO autoresizingMask - layout engine sets frames
    }
    return self;
}

- (void)setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing {
    _paddingTop = top;
    _paddingBottom = bottom;
    _paddingLeading = leading;
    _paddingTrailing = trailing;
}

- (void)setSpacing:(CGFloat)spacing {
    _spacing = spacing;
}

- (CGFloat)paddingTop { return _paddingTop; }
- (CGFloat)paddingBottom { return _paddingBottom; }
- (CGFloat)paddingLeading { return _paddingLeading; }
- (CGFloat)paddingTrailing { return _paddingTrailing; }
- (CGFloat)spacing { return _spacing; }

// Override layout to be a no-op
- (void)layout {
    [super layout];
    // NO AUTOMATIC LAYOUT - frames are set by the layout engine
}

- (void)addSubview:(NSView*)view {
    [super addSubview:view];
}

- (void)willRemoveSubview:(NSView*)subview {
    [super willRemoveSubview:subview];
}

@end

// Internal HStack wrapper class
@interface ObsidianHStackWrapper : NSObject {
    ObsidianHStackContainerView* _containerView;
}

@property (nonatomic, strong) ObsidianHStackContainerView* containerView;

- (instancetype)initWithParams:(ObsidianHStackParams)params;
- (void*)getView;
- (void)addChildView:(void*)childViewHandle;
- (void)removeChildView:(void*)childViewHandle;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianHStackWrapper

- (instancetype)initWithParams:(ObsidianHStackParams)params {
    self = [super init];
    if (self) {
        NSRect frame = NSMakeRect(0, 0, 0, 0);  // Start with zero frame
        _containerView = [[ObsidianHStackContainerView alloc] initWithFrame:frame];
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_containerView;
}

- (void)setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing {
    [_containerView setPaddingTop:top bottom:bottom leading:leading trailing:trailing];
}

- (void)setSpacing:(CGFloat)spacing {
    [_containerView setSpacing:spacing];
}

- (void)addChildView:(void*)childViewHandle {
    if (!_containerView || !childViewHandle) {
        return;
    }
    
    NSView* childView = (__bridge NSView*)childViewHandle;
    if (!childView) {
        return;
    }
    
    [_containerView addSubview:childView];
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
        _containerView.frame = contentView.bounds;
        [contentView addSubview:_containerView];
    }
}

- (void)removeFromParent {
    if (!_containerView) return;
    
    NSArray<NSView*>* subviews = [_containerView.subviews copy];
    for (NSView* subview in subviews) {
        [subview removeFromSuperview];
    }
    
    if ([_containerView superview]) {
        [_containerView removeFromSuperview];
    }
}

@end

// C interface implementation
extern "C" {

ObsidianHStackHandle obsidian_macos_create_hstack(ObsidianHStackParams params) {
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = [[ObsidianHStackWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.containerView) {
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void* obsidian_macos_hstack_get_view(ObsidianHStackHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_hstack_add_child_view(ObsidianHStackHandle hstackHandle,
                                         void* childViewHandle) {
    if (!hstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)hstackHandle;
        [wrapper addChildView:childViewHandle];
    }
}

void obsidian_macos_hstack_remove_child_view(ObsidianHStackHandle hstackHandle,
                                             void* childViewHandle) {
    if (!hstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)hstackHandle;
        [wrapper removeChildView:childViewHandle];
    }
}

void obsidian_macos_hstack_set_visible(ObsidianHStackHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_hstack_is_visible(ObsidianHStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_hstack_add_to_window(ObsidianHStackHandle hstackHandle,
                                        void* windowHandle) {
    if (!hstackHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)hstackHandle;
        [wrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_hstack_remove_from_parent(ObsidianHStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_hstack(ObsidianHStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge_transfer ObsidianHStackWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_release_hstack_handle(ObsidianHStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianHStackWrapper*)handle;
    }
}

bool obsidian_macos_hstack_is_valid(ObsidianHStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        return wrapper != nil && wrapper.containerView != nil;
    }
}

void obsidian_macos_hstack_request_layout_update(ObsidianHStackHandle handle) {
    // NO-OP: Layout is now handled by the layout engine
}

void obsidian_macos_hstack_set_padding(ObsidianHStackHandle handle,
                                       double top, double bottom,
                                       double leading, double trailing) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        [wrapper setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing];
    }
}

void obsidian_macos_hstack_set_spacing(ObsidianHStackHandle handle, double spacing) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        [wrapper setSpacing:(CGFloat)spacing];
    }
}

// NEW: Set frame directly - called by the layout engine
void obsidian_macos_hstack_set_frame(ObsidianHStackHandle handle,
                                      double x, double y,
                                      double width, double height) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        if (wrapper && wrapper.containerView) {
            wrapper.containerView.frame = NSMakeRect(x, y, width, height);
        }
    }
}

} // extern "C"
