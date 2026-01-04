/**
 * macOS Window FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSWindow APIs
 */

#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <memory>
#import <float.h>

// Internal window wrapper class
@interface ObsidianWindowWrapper : NSObject {
    NSWindow* _window;
}

@property (nonatomic, strong) NSWindow* window;

- (instancetype)initWithParams:(ObsidianWindowParams)params;
- (void)show;
- (void)close;

@end

@implementation ObsidianWindowWrapper

- (instancetype)initWithParams:(ObsidianWindowParams)params {
    self = [super init];
    if (self) {
        NSRect contentRect = NSMakeRect(0, 0, params.width, params.height);
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | 
                                     NSWindowStyleMaskClosable | 
                                     NSWindowStyleMaskResizable | 
                                     NSWindowStyleMaskMiniaturizable;
        
        _window = [[NSWindow alloc] initWithContentRect:contentRect
                                               styleMask:styleMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        
        // REQUIRED: Set isReleasedWhenClosed to NO for proper memory management with ARC
        // When managing windows programmatically (not with NSWindowController), we must
        // explicitly control the window lifecycle. Setting this to NO prevents automatic
        // release on close, allowing our wrapper to manage the window's lifetime.
        [_window setReleasedWhenClosed:NO];
        
        if (params.title) {
            NSString* title = [NSString stringWithUTF8String:params.title];
            [_window setTitle:title];
        }
        
        // Set minimum and maximum content size for window resizing
        // Minimum size allows testing resize behavior while keeping content visible
        // 300x300 is a reasonable minimum for most UI content
        NSSize minSize = NSMakeSize(300, 300);
        NSSize maxSize = NSMakeSize(FLT_MAX, FLT_MAX);
        [_window setContentMinSize:minSize];
        [_window setContentMaxSize:maxSize];
        
        // Set background color for window content view to visualize padding
        NSView* contentView = [_window contentView];
        [contentView setWantsLayer:YES];
        NSColor* windowColor = [NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.25 alpha:1.0];
        [contentView.layer setBackgroundColor:[windowColor CGColor]];
        
        // Center the window on screen
        [_window center];
    }
    return self;
}

- (void)show {
    if (_window) {
        [_window makeKeyAndOrderFront:nil];
    }
}

- (void)close {
    if (!_window) return;
    
    // NSWindow's close method is idempotent - safe to call multiple times
    // Since we set isReleasedWhenClosed to NO in initialization, the window
    // will not auto-release, allowing us to manage its lifecycle explicitly
    [_window close];
    
    // Set to nil to release our strong reference
    // ARC will handle the actual release when the wrapper is deallocated
    _window = nil;
}

@end

// C interface implementation
extern "C" {

ObsidianWindowHandle obsidian_macos_create_window(ObsidianWindowParams params) {
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = [[ObsidianWindowWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.window) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_show_window(ObsidianWindowHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        [wrapper show];
    }
}

void obsidian_macos_destroy_window(ObsidianWindowHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge_transfer ObsidianWindowWrapper*)handle;
        // Close the window - this will handle cleanup and set _window = nil internally
        // The wrapper will be deallocated when the autorelease pool pops
        // ARC will handle releasing any remaining references
        [wrapper close];
    }
}

bool obsidian_macos_window_is_valid(ObsidianWindowHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        return wrapper != nil && wrapper.window != nil;
    }
}

void* obsidian_macos_window_get_content_view(ObsidianWindowHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            NSView* contentView = [wrapper.window contentView];
            return (__bridge void*)contentView;
        }
    }
    return nullptr;
}

void obsidian_macos_window_update_constraints(ObsidianWindowHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            // CRITICAL: In macOS 15+ (Sequoia), layout is computed more lazily
            // We must explicitly update constraints to prevent window from shrinking
            // based on contentView's fittingSize
            // updateConstraintsIfNeeded is a method on NSWindow, not NSView
            [wrapper.window updateConstraintsIfNeeded];
            [wrapper.window.contentView layoutSubtreeIfNeeded];
        }
    }
}

} // extern "C"

