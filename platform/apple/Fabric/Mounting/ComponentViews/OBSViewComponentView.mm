/**
 * OBSViewComponentView Implementation
 *
 * Base container view - dumb, no layout computation.
 */

#import "OBSViewComponentView.h"

@implementation OBSViewComponentView

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        self.wantsLayer = YES;  // Use layer-backed for better performance
    }
    return self;
}

// Use top-left coordinate system (matches layout engine)
- (BOOL)isFlipped {
    return YES;
}

// Override layout to be a no-op - frames set by layout engine
- (void)layout {
    [super layout];
    // NO AUTOMATIC LAYOUT
}

// Drawing
- (void)drawRect:(NSRect)dirtyRect {
    // No background by default - transparent container
}

// Layer drawing
- (BOOL)wantsUpdateLayer {
    return YES;
}

- (void)updateLayer {
    // Default: no background
}

#pragma mark - OBSComponentViewProtocol

- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index {
    NSAssert(
        childComponentView.superview == nil,
        @"Attempt to mount already mounted component view."
    );
    
    if (index >= 0 && (NSUInteger)index < self.subviews.count) {
        // Insert at specific index - need to reverse because NSView ordering
        NSView *referenceView = self.subviews[index];
        [self addSubview:childComponentView positioned:NSWindowBelow relativeTo:referenceView];
    } else {
        [self addSubview:childComponentView];
    }
}

- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index {
    NSAssert(
        childComponentView.superview == self,
        @"Attempt to unmount view not mounted in this view."
    );
    
    [childComponentView removeFromSuperview];
}

- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics {
    // Validate
    if (!isfinite(layoutMetrics.x) || !isfinite(layoutMetrics.y) ||
        !isfinite(layoutMetrics.width) || !isfinite(layoutMetrics.height)) {
        NSLog(@"[OBS] Warning: Invalid layout metrics for OBSViewComponentView");
        return;
    }
    
    self.frame = NSMakeRect(
        layoutMetrics.x,
        layoutMetrics.y,
        layoutMetrics.width,
        layoutMetrics.height
    );
}

- (void)prepareForRecycle {
    // Remove all children
    for (NSView *subview in [self.subviews copy]) {
        [subview removeFromSuperview];
    }
    
    // Reset frame
    self.frame = NSZeroRect;
    
    // Remove from superview if mounted
    if (self.superview) {
        [self removeFromSuperview];
    }
}

- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask {
    // No-op for base view
}

- (void)invalidate {
    // No-op for base view
}

@end
