/**
 * macOS Spacer FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSView APIs for flexible spacer views
 */

#import "macos_spacer.h"
#import "macos_layout.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Custom NSView subclass for flexible spacer
// Uses low content hugging and compression resistance priorities to allow flexible sizing
@interface ObsidianSpacerView : NSView {
    double _minWidth;
    double _minHeight;
    double _maxWidth;
    double _maxHeight;
    NSLayoutConstraint* _minWidthConstraint;
    NSLayoutConstraint* _minHeightConstraint;
    NSLayoutConstraint* _maxWidthConstraint;
    NSLayoutConstraint* _maxHeightConstraint;
}

- (instancetype)initWithFrame:(NSRect)frameRect;
- (void)setMinWidth:(double)minWidth;
- (void)setMinHeight:(double)minHeight;
- (void)setMaxWidth:(double)maxWidth;
- (void)setMaxHeight:(double)maxHeight;
- (void)updateConstraints;

@end

@implementation ObsidianSpacerView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        _minWidth = 0.0;
        _minHeight = 0.0;
        _maxWidth = 0.0;
        _maxHeight = 0.0;
        _minWidthConstraint = nil;
        _minHeightConstraint = nil;
        _maxWidthConstraint = nil;
        _maxHeightConstraint = nil;
        
        // CRITICAL: Set low content hugging priority so the spacer wants to grow
        // NSLayoutPriorityDefaultLow = 250.0 (wants to grow)
        // NSLayoutPriorityDefaultHigh = 750.0 (wants to stay small)
        [self setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
        [self setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];
        
        // CRITICAL: Set low compression resistance priority so the spacer can shrink
        // NSLayoutPriorityDefaultLow = 250.0 (can be compressed)
        // NSLayoutPriorityDefaultHigh = 750.0 (resists compression)
        [self setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
        [self setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationVertical];
        
        // Spacer has no intrinsic content size (wants to expand to fill space)
        // By default, NSView returns NSViewNoIntrinsicMetric (-1.0) for both width and height
        // This is correct for a spacer - it should expand to fill available space
    }
    return self;
}

- (NSSize)intrinsicContentSize {
    // Spacer has no intrinsic content size - it wants to expand to fill available space
    // Return NSViewNoIntrinsicMetric for both dimensions
    return NSMakeSize(NSViewNoIntrinsicMetric, NSViewNoIntrinsicMetric);
}

- (void)setMinWidth:(double)minWidth {
    if (_minWidth == minWidth) {
        return;
    }
    _minWidth = minWidth;
    [self updateConstraints];
}

- (void)setMinHeight:(double)minHeight {
    if (_minHeight == minHeight) {
        return;
    }
    _minHeight = minHeight;
    [self updateConstraints];
}

- (void)setMaxWidth:(double)maxWidth {
    if (_maxWidth == maxWidth) {
        return;
    }
    _maxWidth = maxWidth;
    [self updateConstraints];
}

- (void)setMaxHeight:(double)maxHeight {
    if (_maxHeight == maxHeight) {
        return;
    }
    _maxHeight = maxHeight;
    [self updateConstraints];
}

- (void)updateConstraints {
    // CRITICAL: Must call super first - this is required by Auto Layout system
    [super updateConstraints];
    
    // Remove old constraints
    if (_minWidthConstraint) {
        [self removeConstraint:_minWidthConstraint];
        _minWidthConstraint = nil;
    }
    if (_minHeightConstraint) {
        [self removeConstraint:_minHeightConstraint];
        _minHeightConstraint = nil;
    }
    if (_maxWidthConstraint) {
        [self removeConstraint:_maxWidthConstraint];
        _maxWidthConstraint = nil;
    }
    if (_maxHeightConstraint) {
        [self removeConstraint:_maxHeightConstraint];
        _maxHeightConstraint = nil;
    }
    
    // Add new constraints if needed
    if (_minWidth > 0.0) {
        _minWidthConstraint = [NSLayoutConstraint constraintWithItem:self
                                                           attribute:NSLayoutAttributeWidth
                                                           relatedBy:NSLayoutRelationGreaterThanOrEqual
                                                              toItem:nil
                                                           attribute:NSLayoutAttributeNotAnAttribute
                                                          multiplier:1.0
                                                            constant:_minWidth];
        _minWidthConstraint.priority = NSLayoutPriorityRequired;  // 1000.0
        [self addConstraint:_minWidthConstraint];
    }
    
    if (_minHeight > 0.0) {
        _minHeightConstraint = [NSLayoutConstraint constraintWithItem:self
                                                            attribute:NSLayoutAttributeHeight
                                                            relatedBy:NSLayoutRelationGreaterThanOrEqual
                                                               toItem:nil
                                                            attribute:NSLayoutAttributeNotAnAttribute
                                                           multiplier:1.0
                                                             constant:_minHeight];
        _minHeightConstraint.priority = NSLayoutPriorityRequired;  // 1000.0
        [self addConstraint:_minHeightConstraint];
    }
    
    if (_maxWidth > 0.0) {
        _maxWidthConstraint = [NSLayoutConstraint constraintWithItem:self
                                                           attribute:NSLayoutAttributeWidth
                                                           relatedBy:NSLayoutRelationLessThanOrEqual
                                                              toItem:nil
                                                           attribute:NSLayoutAttributeNotAnAttribute
                                                          multiplier:1.0
                                                            constant:_maxWidth];
        _maxWidthConstraint.priority = NSLayoutPriorityRequired;  // 1000.0
        [self addConstraint:_maxWidthConstraint];
    }
    
    if (_maxHeight > 0.0) {
        _maxHeightConstraint = [NSLayoutConstraint constraintWithItem:self
                                                            attribute:NSLayoutAttributeHeight
                                                            relatedBy:NSLayoutRelationLessThanOrEqual
                                                               toItem:nil
                                                            attribute:NSLayoutAttributeNotAnAttribute
                                                           multiplier:1.0
                                                             constant:_maxHeight];
        _maxHeightConstraint.priority = NSLayoutPriorityRequired;  // 1000.0
        [self addConstraint:_maxHeightConstraint];
    }
}

@end

// Internal Spacer wrapper class
@interface ObsidianSpacerWrapper : NSObject {
    ObsidianSpacerView* _spacerView;
    double _minWidth;
    double _minHeight;
    double _maxWidth;
    double _maxHeight;
}

@property (nonatomic, strong) ObsidianSpacerView* spacerView;

- (instancetype)initWithParams:(ObsidianSpacerParams)params;
- (void*)getView;
- (void)setMinWidth:(double)minWidth;
- (double)getMinWidth;
- (void)setMinHeight:(double)minHeight;
- (double)getMinHeight;
- (void)setMaxWidth:(double)maxWidth;
- (double)getMaxWidth;
- (void)setMaxHeight:(double)maxHeight;
- (double)getMaxHeight;

@end

@implementation ObsidianSpacerWrapper

- (instancetype)initWithParams:(ObsidianSpacerParams)params {
    self = [super init];
    if (self) {
        // Create spacer view with zero frame (layout will determine size)
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _spacerView = [[ObsidianSpacerView alloc] initWithFrame:frame];
        
        // Set initial constraints
        _minWidth = params.minWidth;
        _minHeight = params.minHeight;
        _maxWidth = params.maxWidth;
        _maxHeight = params.maxHeight;
        
        [_spacerView setMinWidth:_minWidth];
        [_spacerView setMinHeight:_minHeight];
        [_spacerView setMaxWidth:_maxWidth];
        [_spacerView setMaxHeight:_maxHeight];
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_spacerView;
}

- (void)setMinWidth:(double)minWidth {
    _minWidth = minWidth;
    [_spacerView setMinWidth:minWidth];
}

- (double)getMinWidth {
    return _minWidth;
}

- (void)setMinHeight:(double)minHeight {
    _minHeight = minHeight;
    [_spacerView setMinHeight:minHeight];
}

- (double)getMinHeight {
    return _minHeight;
}

- (void)setMaxWidth:(double)maxWidth {
    _maxWidth = maxWidth;
    [_spacerView setMaxWidth:maxWidth];
}

- (double)getMaxWidth {
    return _maxWidth;
}

- (void)setMaxHeight:(double)maxHeight {
    _maxHeight = maxHeight;
    [_spacerView setMaxHeight:maxHeight];
}

- (double)getMaxHeight {
    return _maxHeight;
}

- (ObsidianSpacerView*)spacerView {
    return _spacerView;
}

@end

// C interface implementation
extern "C" {

ObsidianSpacerHandle obsidian_macos_create_spacer(ObsidianSpacerParams params) {
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = [[ObsidianSpacerWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.spacerView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_spacer_set_min_width(ObsidianSpacerHandle handle, double minWidth) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper setMinWidth:minWidth];
    }
}

double obsidian_macos_spacer_get_min_width(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper getMinWidth];
    }
}

void obsidian_macos_spacer_set_min_height(ObsidianSpacerHandle handle, double minHeight) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper setMinHeight:minHeight];
    }
}

double obsidian_macos_spacer_get_min_height(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper getMinHeight];
    }
}

void obsidian_macos_spacer_set_max_width(ObsidianSpacerHandle handle, double maxWidth) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper setMaxWidth:maxWidth];
    }
}

double obsidian_macos_spacer_get_max_width(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper getMaxWidth];
    }
}

void obsidian_macos_spacer_set_max_height(ObsidianSpacerHandle handle, double maxHeight) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper setMaxHeight:maxHeight];
    }
}

double obsidian_macos_spacer_get_max_height(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper getMaxHeight];
    }
}

void* obsidian_macos_spacer_get_view(ObsidianSpacerHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_destroy_spacer(ObsidianSpacerHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Get wrapper and remove view from parent if needed
        ObsidianSpacerWrapper* wrapper = (__bridge_transfer ObsidianSpacerWrapper*)handle;
        if (wrapper && wrapper.spacerView) {
            [wrapper.spacerView removeFromSuperview];
        }
        // wrapper is automatically released via __bridge_transfer
    }
}

bool obsidian_macos_spacer_is_valid(ObsidianSpacerHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return wrapper != nil && wrapper.spacerView != nil;
    }
}

} // extern "C"
