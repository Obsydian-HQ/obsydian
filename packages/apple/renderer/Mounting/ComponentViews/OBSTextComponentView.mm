/**
 * OBSTextComponentView Implementation
 *
 * NSTextField configured as a label.
 */

#import "OBSTextComponentView.h"

@implementation OBSTextComponentView

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        // Configure as non-editable label
        self.editable = NO;
        self.selectable = NO;
        self.bezeled = NO;
        self.drawsBackground = NO;
        self.lineBreakMode = NSLineBreakByTruncatingTail;
    }
    return self;
}

#pragma mark - Public API

- (void)setTextContent:(NSString *)text {
    self.stringValue = text ?: @"";
}

- (void)setTextAlignment:(NSTextAlignment)alignment {
    self.alignment = alignment;
}

- (void)setTextDisplayColor:(nullable NSColor *)color {
    self.textColor = color;
}

- (void)setTextFont:(NSFont *)font {
    self.font = font;
}

#pragma mark - OBSComponentViewProtocol

- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index {
    // Text views don't have children
    NSLog(@"[OBS] Warning: Attempt to mount child into text view - not supported");
}

- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index {
    // Text views don't have children
    NSLog(@"[OBS] Warning: Attempt to unmount child from text view - not supported");
}

- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics {
    if (!isfinite(layoutMetrics.x) || !isfinite(layoutMetrics.y) ||
        !isfinite(layoutMetrics.width) || !isfinite(layoutMetrics.height)) {
        NSLog(@"[OBS] Warning: Invalid layout metrics for text view");
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
    // Clear text
    self.stringValue = @"";
    
    // Reset frame
    self.frame = NSZeroRect;
    
    // Reset styling to defaults
    self.font = [NSFont systemFontOfSize:NSFont.systemFontSize];
    self.textColor = NSColor.labelColor;
    self.alignment = NSTextAlignmentNatural;
    
    // Remove from superview
    if (self.superview) {
        [self removeFromSuperview];
    }
}

- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask {
    // No-op
}

- (void)invalidate {
    // No-op
}

@end
