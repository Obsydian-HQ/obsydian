/**
 * NSView+OBSComponentViewProtocol
 *
 * Default implementation of OBSComponentViewProtocol for NSView.
 */

#import "NSView+OBSComponentViewProtocol.h"

@implementation NSView (OBSComponentViewProtocol)

#pragma mark - OBSComponentViewProtocol

- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index
{
    NSAssert(
        childComponentView.superview == nil,
        @"Attempt to mount already mounted component view. (parent: %@, child: %@, index: %ld)",
        self,
        childComponentView,
        (long)index
    );
    
    if (index >= 0 && (NSUInteger)index < self.subviews.count) {
        // Insert at specific index
        NSView *referenceView = self.subviews[index];
        [self addSubview:childComponentView positioned:NSWindowBelow relativeTo:referenceView];
    } else {
        // Append at end
        [self addSubview:childComponentView];
    }
}

- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index
{
    NSAssert(
        childComponentView.superview == self,
        @"Attempt to unmount a view which is mounted inside different view. (parent: %@, child: %@)",
        self,
        childComponentView
    );
    
    [childComponentView removeFromSuperview];
}

- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics
{
    // Check for NaN/Inf values
    if (!isfinite(layoutMetrics.x) || !isfinite(layoutMetrics.y) ||
        !isfinite(layoutMetrics.width) || !isfinite(layoutMetrics.height)) {
        NSLog(@"[OBS] Warning: Invalid layout metrics received for view %@: (%.2f, %.2f, %.2f, %.2f)",
              self, layoutMetrics.x, layoutMetrics.y, layoutMetrics.width, layoutMetrics.height);
        return;
    }
    
    // Apply frame directly
    self.frame = NSMakeRect(
        layoutMetrics.x,
        layoutMetrics.y,
        layoutMetrics.width,
        layoutMetrics.height
    );
}

- (void)prepareForRecycle
{
    // Default: remove from superview and reset frame
    if (self.superview) {
        [self removeFromSuperview];
    }
    self.frame = NSZeroRect;
    
    // Remove all subviews
    for (NSView *subview in [self.subviews copy]) {
        [subview removeFromSuperview];
    }
}

- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask
{
    // Default: no-op
}

- (void)invalidate
{
    // Default: no-op
}

@end
