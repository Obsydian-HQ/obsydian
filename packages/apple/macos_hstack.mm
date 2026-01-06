/**
 * macOS HStack FFI - Objective-C++ Implementation
 * 
 * Frame-based layout container for horizontal stacking
 * Following React Native's pattern: compute layout and set frames directly
 * NO AUTO LAYOUT - Pure frame-based positioning
 */

#import "macos_hstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass that performs frame-based horizontal layout
// This is the React Native approach: calculate positions and set frames directly
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
        
        // Use autoresizing mask for flexible sizing within parent
        self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    }
    return self;
}

- (void)setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing {
    _paddingTop = top;
    _paddingBottom = bottom;
    _paddingLeading = leading;
    _paddingTrailing = trailing;
    [self setNeedsLayout:YES];
}

- (void)setSpacing:(CGFloat)spacing {
    _spacing = spacing;
    [self setNeedsLayout:YES];
}

// CORE LAYOUT METHOD - Frame-based horizontal stacking
- (void)layout {
    [super layout];
    
    if (self.subviews.count == 0) {
        return;
    }
    
    NSRect bounds = self.bounds;
    CGFloat availableHeight = bounds.size.height - _paddingTop - _paddingBottom;
    
    // For HStack: layout from left to right
    CGFloat currentX = _paddingLeading;
    
    for (NSView* subview in self.subviews) {
        if (subview.isHidden) continue;
        
        // Get the child's preferred size
        NSSize childSize = [self preferredSizeForChild:subview availableHeight:availableHeight];
        
        // Position the child
        // X: currentX (advancing left to right)
        // Y: centered vertically, or at bottom padding (macOS coords)
        CGFloat childX = currentX;
        CGFloat childY = _paddingBottom;  // Start from bottom in macOS coordinates
        CGFloat childWidth = childSize.width;
        CGFloat childHeight = availableHeight;  // Fill available height
        
        // Set the frame directly - NO CONSTRAINTS
        subview.frame = NSMakeRect(childX, childY, childWidth, childHeight);
        
        // Move X right for next child
        currentX += childWidth + _spacing;
    }
}

// Helper to get preferred size for a child view
- (NSSize)preferredSizeForChild:(NSView*)child availableHeight:(CGFloat)availableHeight {
    // First try fittingSize
    NSSize fittingSize = child.fittingSize;
    
    if (fittingSize.width > 0 && fittingSize.height > 0) {
        return NSMakeSize(fittingSize.width, availableHeight);
    }
    
    // Fallback: check intrinsicContentSize
    NSSize intrinsicSize = child.intrinsicContentSize;
    if (intrinsicSize.width > 0 && intrinsicSize.width != NSViewNoIntrinsicMetric) {
        return NSMakeSize(intrinsicSize.width, availableHeight);
    }
    
    // Last resort: use current frame size or a default
    NSSize currentSize = child.frame.size;
    if (currentSize.width > 0) {
        return NSMakeSize(currentSize.width, availableHeight);
    }
    
    // Absolute fallback: default width
    return NSMakeSize(80.0, availableHeight);
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
        NSRect frame = NSMakeRect(0, 0, 300, 200);
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
        _containerView.frame = contentView.bounds;
        [contentView addSubview:_containerView];
        [_containerView setNeedsLayout:YES];
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
    if (!handle) return;

    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = (__bridge ObsidianHStackWrapper*)handle;
        if (wrapper && wrapper.containerView) {
            NSView* containerView = wrapper.containerView;
            [containerView setNeedsLayout:YES];
            [containerView layoutSubtreeIfNeeded];
        }
    }
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

} // extern "C"
