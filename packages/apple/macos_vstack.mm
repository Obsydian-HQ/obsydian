/**
 * macOS VStack FFI - Objective-C++ Implementation
 * 
 * DUMB CONTAINER: This view does NOT compute layout.
 * The Layout Engine will compute positions/sizes and set frames directly.
 * 
 * This follows React Native's architecture:
 * - Native views are simple containers
 * - Layout is computed externally (in a shadow tree/layout engine)
 * - Frames are set directly by the layout system
 */

#import "macos_vstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Simple container view - does NOT compute layout
// The Layout Engine sets frames directly on this and its children
@interface ObsidianVStackContainerView : NSView {
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

@implementation ObsidianVStackContainerView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        _paddingTop = 0.0;
        _paddingBottom = 0.0;
        _paddingLeading = 0.0;
        _paddingTrailing = 0.0;
        _spacing = 0.0;
        
        // IMPORTANT: We do NOT set autoresizingMask or any automatic layout
        // The layout engine is responsible for setting our frame
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

// Use top-left coordinate system (matches layout engine)
- (BOOL)isFlipped {
    return YES;
}

// Override layout to be a no-op - the layout engine sets frames
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

// Internal VStack wrapper class
@interface ObsidianVStackWrapper : NSObject {
    ObsidianVStackContainerView* _containerView;
}

@property (nonatomic, strong) ObsidianVStackContainerView* containerView;

- (instancetype)initWithParams:(ObsidianVStackParams)params;
- (void*)getView;
- (void)addChildView:(void*)childViewHandle;
- (void)removeChildView:(void*)childViewHandle;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianVStackWrapper

- (instancetype)initWithParams:(ObsidianVStackParams)params {
    self = [super init];
    if (self) {
        // Create container view - the layout engine will set the proper frame
        NSRect frame = NSMakeRect(0, 0, 0, 0);  // Start with zero frame
        _containerView = [[ObsidianVStackContainerView alloc] initWithFrame:frame];
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
        NSLog(@"[VStack DEBUG] addChildView FAILED: containerView=%p childViewHandle=%p", _containerView, childViewHandle);
        return;
    }
    
    NSView* childView = (__bridge NSView*)childViewHandle;
    if (!childView) {
        NSLog(@"[VStack DEBUG] addChildView FAILED: childView bridge failed");
        return;
    }
    
    NSLog(@"[VStack DEBUG] Adding child %@ frame:%@ to container (subview count before: %lu)", 
          NSStringFromClass([childView class]),
          NSStringFromRect(childView.frame),
          (unsigned long)_containerView.subviews.count);
    
    // Simply add the child - the layout engine will set frames
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
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        
        // Set frame to fill content view (temporary until layout engine takes over)
        _containerView.frame = contentView.bounds;
        
        [contentView addSubview:_containerView];
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

ObsidianVStackHandle obsidian_macos_create_vstack(ObsidianVStackParams params) {
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = [[ObsidianVStackWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.containerView) {
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void* obsidian_macos_vstack_get_view(ObsidianVStackHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_vstack_add_child_view(ObsidianVStackHandle vstackHandle,
                                          void* childViewHandle) {
    if (!vstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)vstackHandle;
        [wrapper addChildView:childViewHandle];
    }
}

void obsidian_macos_vstack_remove_child_view(ObsidianVStackHandle vstackHandle,
                                              void* childViewHandle) {
    if (!vstackHandle || !childViewHandle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)vstackHandle;
        [wrapper removeChildView:childViewHandle];
    }
}

void obsidian_macos_vstack_set_visible(ObsidianVStackHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_vstack_is_visible(ObsidianVStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_vstack_add_to_window(ObsidianVStackHandle vstackHandle,
                                         void* windowHandle) {
    if (!vstackHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)vstackHandle;
        [wrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_vstack_remove_from_parent(ObsidianVStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_vstack(ObsidianVStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge_transfer ObsidianVStackWrapper*)handle;
        [wrapper removeFromParent];
        // wrapper is automatically released
    }
}

void obsidian_macos_release_vstack_handle(ObsidianVStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianVStackWrapper*)handle;
    }
}

bool obsidian_macos_vstack_is_valid(ObsidianVStackHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        return wrapper != nil && wrapper.containerView != nil;
    }
}

void obsidian_macos_vstack_request_layout_update(ObsidianVStackHandle handle) {
    // NO-OP: Layout is now handled by the layout engine
    // This function exists for API compatibility but does nothing
}

void obsidian_macos_vstack_set_padding(ObsidianVStackHandle handle,
                                        double top, double bottom,
                                        double leading, double trailing) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        [wrapper setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing];
    }
}

void obsidian_macos_vstack_set_spacing(ObsidianVStackHandle handle, double spacing) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        [wrapper setSpacing:(CGFloat)spacing];
    }
}

// NEW: Set frame directly - called by the layout engine
void obsidian_macos_vstack_set_frame(ObsidianVStackHandle handle,
                                      double x, double y,
                                      double width, double height) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        if (wrapper && wrapper.containerView) {
            wrapper.containerView.frame = NSMakeRect(x, y, width, height);
        }
    }
}

} // extern "C"
