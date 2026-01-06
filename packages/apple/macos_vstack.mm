/**
 * macOS VStack FFI - Objective-C++ Implementation
 * 
 * Frame-based layout container for vertical stacking
 * Following React Native's pattern: compute layout and set frames directly
 * NO AUTO LAYOUT - Pure frame-based positioning
 */

#import "macos_vstack.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass that performs frame-based vertical layout
// This is the React Native approach: calculate positions and set frames directly
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
        
        // Use autoresizing mask for flexible sizing within parent
        // This is the frame-based way to fill parent or resize with window
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

// CORE LAYOUT METHOD - Frame-based vertical stacking
// This is how React Native does it: calculate positions, set frames
- (void)layout {
    [super layout];
    
    if (self.subviews.count == 0) {
        return;
    }
    
    NSRect bounds = self.bounds;
    CGFloat availableWidth = bounds.size.width - _paddingLeading - _paddingTrailing;
    
    // macOS coordinate system: origin is bottom-left, Y increases upward
    // For VStack, we want to lay out from top to bottom
    // So we start at (height - paddingTop) and go down
    
    // Start Y position from the top (in macOS coordinates, top = bounds.height - paddingTop)
    CGFloat currentY = bounds.size.height - _paddingTop;
    
    // Second pass: position each child
    for (NSView* subview in self.subviews) {
        if (subview.isHidden) continue;
        
        // Get the child's preferred size
        NSSize childSize = [self preferredSizeForChild:subview availableWidth:availableWidth];
        
        // Position the child
        // X: leading padding
        // Y: currentY - childHeight (because macOS Y is bottom-up)
        // Width: fill available width (or use child's preferred width)
        // Height: child's preferred height
        
        CGFloat childX = _paddingLeading;
        CGFloat childY = currentY - childSize.height;
        CGFloat childWidth = availableWidth;
        CGFloat childHeight = childSize.height;
        
        // Set the frame directly - NO CONSTRAINTS
        subview.frame = NSMakeRect(childX, childY, childWidth, childHeight);
        
        // Move Y down for next child (in macOS coords, this means subtracting)
        currentY = childY - _spacing;
    }
}

// Helper to get preferred size for a child view
- (NSSize)preferredSizeForChild:(NSView*)child availableWidth:(CGFloat)availableWidth {
    // First try fittingSize - works for most AppKit views
    NSSize fittingSize = child.fittingSize;
    
    // If fittingSize returns valid dimensions, use them
    if (fittingSize.width > 0 && fittingSize.height > 0) {
        return NSMakeSize(availableWidth, fittingSize.height);
    }
    
    // Fallback: check intrinsicContentSize (for views that support it)
    NSSize intrinsicSize = child.intrinsicContentSize;
    if (intrinsicSize.height > 0 && intrinsicSize.height != NSViewNoIntrinsicMetric) {
        return NSMakeSize(availableWidth, intrinsicSize.height);
    }
    
    // Last resort: use current frame size or a default
    NSSize currentSize = child.frame.size;
    if (currentSize.height > 0) {
        return NSMakeSize(availableWidth, currentSize.height);
    }
    
    // Absolute fallback: default height
    return NSMakeSize(availableWidth, 24.0);
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
    // Re-layout when container is resized
    [self setNeedsLayout:YES];
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
        // Create container view with a reasonable default size
        // The frame will be set by the parent when added to the view hierarchy
        NSRect frame = NSMakeRect(0, 0, 300, 200);
        _containerView = [[ObsidianVStackContainerView alloc] initWithFrame:frame];
        
        // NO translatesAutoresizingMaskIntoConstraints = NO
        // We use frame-based layout with autoresizing mask for flexibility
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
    // The child keeps its autoresizing behavior, and we set its frame in layout
    
    // Add child view as subview
    [_containerView addSubview:childView];
    
    // Trigger layout to position the new child
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
        
        // Set frame to fill content view
        _containerView.frame = contentView.bounds;
        
        // Add to content view
        [contentView addSubview:_containerView];
        
        // Trigger layout
        [_containerView setNeedsLayout:YES];
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

void obsidian_macos_release_vstack_handle(ObsidianVStackHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release our reference to the wrapper without removing from parent.
        // The containerView stays in the view hierarchy (retained by superview).
        // This is used when the C++ VStack is destroyed but we want the native
        // view to remain visible (e.g., in route rendering).
        (void)(__bridge_transfer ObsidianVStackWrapper*)handle;
        // wrapper is released but containerView stays in superview
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
            
            // Mark for layout and perform it immediately
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
