/**
 * OBSButtonComponentView Implementation
 *
 * NSButton subclass with C callback support.
 * Stores callback as associated objects on self.
 */

#import "OBSButtonComponentView.h"
#import <objc/runtime.h>

// Associated object keys
static const void *kActionCallbackKey = &kActionCallbackKey;
static const void *kUserDataKey = &kUserDataKey;

@implementation OBSButtonComponentView

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        // Configure as push button
        self.bezelStyle = NSBezelStyleRounded;
        self.target = self;
        self.action = @selector(_buttonClicked:);
    }
    return self;
}

#pragma mark - Public API

- (void)setActionCallback:(nullable OBSButtonActionCallback)callback userData:(nullable void *)userData {
    // Store callback as NSValue (function pointer)
    if (callback) {
        NSValue *callbackValue = [NSValue valueWithPointer:(void *)callback];
        objc_setAssociatedObject(self, kActionCallbackKey, callbackValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    } else {
        objc_setAssociatedObject(self, kActionCallbackKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    
    // Store userData as NSValue
    if (userData) {
        NSValue *userDataValue = [NSValue valueWithPointer:userData];
        objc_setAssociatedObject(self, kUserDataKey, userDataValue, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    } else {
        objc_setAssociatedObject(self, kUserDataKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

- (void)setButtonTitle:(NSString *)title {
    self.title = title ?: @"";
}

#pragma mark - Action Handler

- (void)_buttonClicked:(id)sender {
    NSValue *callbackValue = objc_getAssociatedObject(self, kActionCallbackKey);
    NSValue *userDataValue = objc_getAssociatedObject(self, kUserDataKey);
    
    if (callbackValue) {
        OBSButtonActionCallback callback = (OBSButtonActionCallback)[callbackValue pointerValue];
        void *userData = userDataValue ? [userDataValue pointerValue] : NULL;
        
        if (callback) {
            callback(userData);
        }
    }
}

#pragma mark - OBSComponentViewProtocol

- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index {
    // Buttons don't have children in our implementation
    NSLog(@"[OBS] Warning: Attempt to mount child into button - not supported");
}

- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index {
    // Buttons don't have children
    NSLog(@"[OBS] Warning: Attempt to unmount child from button - not supported");
}

- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics {
    if (!isfinite(layoutMetrics.x) || !isfinite(layoutMetrics.y) ||
        !isfinite(layoutMetrics.width) || !isfinite(layoutMetrics.height)) {
        NSLog(@"[OBS] Warning: Invalid layout metrics for button");
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
    // Clear callback
    [self setActionCallback:nil userData:nil];
    
    // Reset title
    self.title = @"";
    
    // Reset frame
    self.frame = NSZeroRect;
    
    // Remove from superview
    if (self.superview) {
        [self removeFromSuperview];
    }
    
    // Reset enabled state
    self.enabled = YES;
}

- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask {
    // No-op for button
}

- (void)invalidate {
    // Clear callback on invalidation
    [self setActionCallback:nil userData:nil];
}

@end
