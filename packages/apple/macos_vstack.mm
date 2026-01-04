/**
 * macOS VStack FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSView APIs for container view management
 */

#import "macos_vstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass that calculates intrinsic content size based on children + padding
// This is the proper Auto Layout approach for sizing a container to its content
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
    // For VStack: width = max(child width) + padding, height = sum(child heights) + spacing + padding
    // 
    // CRITICAL: This method should rely on children's intrinsic content sizes,
    // not their frames, to avoid layout loops. Children (like buttons) should
    // have intrinsic content size based on their content (e.g., button title).
    
    if (self.subviews.count == 0) {
        // No children - return size with just padding
        return NSMakeSize(_paddingLeading + _paddingTrailing, _paddingTop + _paddingBottom);
    }
    
    // Calculate width: maximum child width + horizontal padding
    CGFloat maxChildWidth = 0.0;
    
    // Calculate height: sum of all child heights + spacing between them + vertical padding
    CGFloat totalHeight = _paddingTop + _paddingBottom;
    BOOL isFirstChild = YES;
    BOOL hasValidChildren = NO;
    
    for (NSView* subview in self.subviews) {
        NSSize subviewSize = [subview intrinsicContentSize];
        
        // For width: take the maximum child width
        // Use intrinsic content size - children (like buttons) should have this
        CGFloat childWidth = 0.0;
        if (subviewSize.width != NSViewNoIntrinsicMetric) {
            childWidth = subviewSize.width;
        }
        
        // For height: sum all child heights + spacing between them
        CGFloat childHeight = 0.0;
        if (subviewSize.height != NSViewNoIntrinsicMetric) {
            childHeight = subviewSize.height;
        }
        
        // Only count children with valid intrinsic sizes
        if (childWidth > 0 && childHeight > 0) {
            maxChildWidth = MAX(maxChildWidth, childWidth);
            
            if (!isFirstChild) {
                // Add spacing between children
                totalHeight += _spacing;
            }
            totalHeight += childHeight;
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
    
    // Add horizontal padding
    CGFloat totalWidth = maxChildWidth + _paddingLeading + _paddingTrailing;
    
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
        // Create custom container view with zero frame (will be constrained by layout)
        // The custom view overrides intrinsicContentSize to size itself based on children + padding
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _containerView = [[ObsidianVStackContainerView alloc] initWithFrame:frame];
        
        // Disable autoresizing mask - we'll use Auto Layout
        [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Set background color to visualize padding area (light green/cyan to differentiate from window)
        [_containerView setWantsLayer:YES];
        NSColor* containerColor = [NSColor colorWithCalibratedRed:0.2 green:0.6 blue:0.5 alpha:0.9];
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
    
    // Force layout update to ensure constraints are applied immediately
    // This prevents views from appearing at incorrect positions (like bottom-left)
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
        
        // Pin container view to content view edges (will be overridden by user constraints if needed)
        // But for now, we'll let the user manage container view constraints
        // The container view should be constrained by whoever adds it to the window
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
            // Retain the wrapper and return as opaque handle
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
        // Remove from parent and clean up children
        [wrapper removeFromParent];
        // wrapper is automatically released due to __bridge_transfer
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
    if (!handle) return;

    @autoreleasepool {
        ObsidianVStackWrapper* wrapper = (__bridge ObsidianVStackWrapper*)handle;
        if (wrapper && wrapper.containerView) {
            NSView* containerView = wrapper.containerView;
            
            // CRITICAL: Force immediate layout update to apply Auto Layout constraints
            // This ensures views are positioned correctly, not at default frame positions
            [containerView setNeedsLayout:YES];
            [containerView layoutSubtreeIfNeeded];
        }
    }
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

} // extern "C"
