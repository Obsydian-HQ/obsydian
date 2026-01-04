/**
 * macOS HStack FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSView APIs for container view management
 */

#import "macos_hstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass that calculates intrinsic content size based on children + padding
// This is the proper Auto Layout approach for sizing a container to its content
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
    }
    return self;
}

- (void)setPaddingTop:(CGFloat)top bottom:(CGFloat)bottom leading:(CGFloat)leading trailing:(CGFloat)trailing {
    _paddingTop = top;
    _paddingBottom = bottom;
    _paddingLeading = leading;
    _paddingTrailing = trailing;
    [self invalidateIntrinsicContentSize];
}

- (void)setSpacing:(CGFloat)spacing {
    _spacing = spacing;
    [self invalidateIntrinsicContentSize];
}

- (NSSize)intrinsicContentSize {
    // Calculate size based on children + padding + spacing
    // This is the proper Auto Layout way to size a container to its content
    // For HStack: width = sum(child widths) + spacing + padding, height = max(child height) + padding
    
    if (self.subviews.count == 0) {
        // No children - return size with just padding
        return NSMakeSize(_paddingLeading + _paddingTrailing, _paddingTop + _paddingBottom);
    }
    
    // Calculate width: sum of all child widths + spacing between them + horizontal padding
    CGFloat totalWidth = _paddingLeading + _paddingTrailing;
    
    // Calculate height: maximum child height + vertical padding
    CGFloat maxChildHeight = 0.0;
    BOOL isFirstChild = YES;
    BOOL hasValidChildren = NO;
    
    for (NSView* subview in self.subviews) {
        NSSize subviewSize = [subview intrinsicContentSize];
        
        // For width: sum all child widths + spacing between them
        CGFloat childWidth = 0.0;
        if (subviewSize.width != NSViewNoIntrinsicMetric) {
            childWidth = subviewSize.width;
        }
        
        // For height: take the maximum child height
        CGFloat childHeight = 0.0;
        if (subviewSize.height != NSViewNoIntrinsicMetric) {
            childHeight = subviewSize.height;
        }
        
        // Only count children with valid intrinsic sizes
        if (childWidth > 0 && childHeight > 0) {
            if (!isFirstChild) {
                // Add spacing between children
                totalWidth += _spacing;
            }
            totalWidth += childWidth;
            maxChildHeight = MAX(maxChildHeight, childHeight);
            isFirstChild = NO;
            hasValidChildren = YES;
        }
    }
    
    // If no valid child sizes found, return minimum size with padding
    // This can happen if children don't have intrinsic content size yet
    // Auto Layout will call this again after children are laid out
    if (!hasValidChildren) {
        return NSMakeSize(_paddingLeading + _paddingTrailing, _paddingTop + _paddingBottom);
    }
    
    // Add vertical padding
    CGFloat totalHeight = maxChildHeight + _paddingTop + _paddingBottom;
    
    return NSMakeSize(totalWidth, totalHeight);
}

- (void)addSubview:(NSView*)view {
    [super addSubview:view];
    [self invalidateIntrinsicContentSize];
}

- (void)willRemoveSubview:(NSView*)subview {
    [super willRemoveSubview:subview];
    [self invalidateIntrinsicContentSize];
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
        // Create custom container view with zero frame (will be constrained by layout)
        // The custom view overrides intrinsicContentSize to size itself based on children + padding
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _containerView = [[ObsidianHStackContainerView alloc] initWithFrame:frame];
        
        // Disable autoresizing mask - we'll use Auto Layout
        [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Set background color to visualize padding area (light blue to differentiate from VStack)
        [_containerView setWantsLayer:YES];
        NSColor* containerColor = [NSColor colorWithCalibratedRed:0.3 green:0.5 blue:0.7 alpha:0.9];
        [_containerView.layer setBackgroundColor:[containerColor CGColor]];
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
    
    // CRITICAL: Disable autoresizing mask BEFORE adding to superview
    // This prevents frame-based positioning from interfering with Auto Layout
    [childView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add child view as subview
    // The custom container view will automatically invalidate its intrinsic content size
    [_containerView addSubview:childView];
    
    // Mark container as needing layout update
    // Constraints will be set up by updateLayout() from the C++ side
    // Do NOT call layoutSubtreeIfNeeded here - it causes crashes when constraints haven't been set up yet
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

ObsidianHStackHandle obsidian_macos_create_hstack(ObsidianHStackParams params) {
    @autoreleasepool {
        ObsidianHStackWrapper* wrapper = [[ObsidianHStackWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.containerView) {
            // Retain the wrapper and return as opaque handle
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
        // Remove from parent and clean up children
        [wrapper removeFromParent];
        // wrapper is automatically released due to __bridge_transfer
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
            
            // CRITICAL: Force immediate layout update to apply Auto Layout constraints
            // This ensures views are positioned correctly, not at default frame positions
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
