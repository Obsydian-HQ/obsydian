/**
 * macOS Spacer FFI - Objective-C++ Implementation
 * 
 * Frame-based flexible spacer view
 * Spacers work by being given a frame by their parent container
 * NO AUTO LAYOUT - Pure frame-based positioning
 */

#import "macos_spacer.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Simple spacer view - its size is determined by the parent layout container
@interface ObsidianSpacerView : NSView {
    double _minWidth;
    double _minHeight;
    double _maxWidth;
    double _maxHeight;
}

- (instancetype)initWithFrame:(NSRect)frameRect;
- (void)setMinWidth:(double)minWidth;
- (double)getMinWidth;
- (void)setMinHeight:(double)minHeight;
- (double)getMinHeight;
- (void)setMaxWidth:(double)maxWidth;
- (double)getMaxWidth;
- (void)setMaxHeight:(double)maxHeight;
- (double)getMaxHeight;

@end

@implementation ObsidianSpacerView

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        _minWidth = 0.0;
        _minHeight = 0.0;
        _maxWidth = 0.0;  // 0 means no max
        _maxHeight = 0.0; // 0 means no max
        
        // Spacer is transparent
        self.wantsLayer = NO;
    }
    return self;
}

// FRAME-BASED: Spacer returns its min size for fittingSize
// The parent container will use this to calculate layout
- (NSSize)fittingSize {
    return NSMakeSize(_minWidth, _minHeight);
}

- (void)setMinWidth:(double)minWidth {
    _minWidth = minWidth;
}

- (double)getMinWidth {
    return _minWidth;
}

- (void)setMinHeight:(double)minHeight {
    _minHeight = minHeight;
}

- (double)getMinHeight {
    return _minHeight;
}

- (void)setMaxWidth:(double)maxWidth {
    _maxWidth = maxWidth;
}

- (double)getMaxWidth {
    return _maxWidth;
}

- (void)setMaxHeight:(double)maxHeight {
    _maxHeight = maxHeight;
}

- (double)getMaxHeight {
    return _maxHeight;
}

@end

// Internal Spacer wrapper class
@interface ObsidianSpacerWrapper : NSObject {
    ObsidianSpacerView* _spacerView;
}

@property (nonatomic, strong) ObsidianSpacerView* spacerView;

- (instancetype)initWithParams:(ObsidianSpacerParams)params;
- (void*)getView;

@end

@implementation ObsidianSpacerWrapper

- (instancetype)initWithParams:(ObsidianSpacerParams)params {
    self = [super init];
    if (self) {
        // Create spacer view with initial frame based on min size
        NSRect frame = NSMakeRect(0, 0, params.minWidth, params.minHeight);
        _spacerView = [[ObsidianSpacerView alloc] initWithFrame:frame];
        
        // Set size constraints
        [_spacerView setMinWidth:params.minWidth];
        [_spacerView setMinHeight:params.minHeight];
        [_spacerView setMaxWidth:params.maxWidth];
        [_spacerView setMaxHeight:params.maxHeight];
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_spacerView;
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
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_spacer_set_min_width(ObsidianSpacerHandle handle, double minWidth) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper.spacerView setMinWidth:minWidth];
    }
}

double obsidian_macos_spacer_get_min_width(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper.spacerView getMinWidth];
    }
}

void obsidian_macos_spacer_set_min_height(ObsidianSpacerHandle handle, double minHeight) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper.spacerView setMinHeight:minHeight];
    }
}

double obsidian_macos_spacer_get_min_height(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper.spacerView getMinHeight];
    }
}

void obsidian_macos_spacer_set_max_width(ObsidianSpacerHandle handle, double maxWidth) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper.spacerView setMaxWidth:maxWidth];
    }
}

double obsidian_macos_spacer_get_max_width(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper.spacerView getMaxWidth];
    }
}

void obsidian_macos_spacer_set_max_height(ObsidianSpacerHandle handle, double maxHeight) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        [wrapper.spacerView setMaxHeight:maxHeight];
    }
}

double obsidian_macos_spacer_get_max_height(ObsidianSpacerHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSpacerWrapper* wrapper = (__bridge ObsidianSpacerWrapper*)handle;
        return [wrapper.spacerView getMaxHeight];
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
        ObsidianSpacerWrapper* wrapper = (__bridge_transfer ObsidianSpacerWrapper*)handle;
        if (wrapper && wrapper.spacerView) {
            [wrapper.spacerView removeFromSuperview];
        }
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
