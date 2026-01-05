/**
 * macOS Window FFI - Objective-C++ Implementation
 */

#import "macos_window.h"
#import <AppKit/AppKit.h>

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
        
        [_window setReleasedWhenClosed:NO];
        
        if (params.title) {
            NSString* title = [NSString stringWithUTF8String:params.title];
            [_window setTitle:title];
        }
        
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

void obsidian_macos_window_set_content_view_controller(ObsidianWindowHandle handle, void* viewControllerHandle) {
    if (!handle || !viewControllerHandle) return;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            NSViewController* viewController = (__bridge NSViewController*)viewControllerHandle;
            if (viewController) {
                // Simply set the content view controller - that's what real apps do
                [wrapper.window setContentViewController:viewController];
            }
        }
    }
}

void obsidian_macos_window_set_toolbar(ObsidianWindowHandle handle, void* toolbarHandle) {
    if (!handle || !toolbarHandle) return;
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            NSToolbar* toolbar = (__bridge NSToolbar*)toolbarHandle;
            [wrapper.window setToolbar:toolbar];
        }
    }
}

void* obsidian_macos_window_get_toolbar(ObsidianWindowHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            NSToolbar* toolbar = [wrapper.window toolbar];
            if (toolbar) {
                return (__bridge void*)toolbar;
            }
        }
        return nullptr;
    }
}

void obsidian_macos_window_set_toolbar_style(ObsidianWindowHandle handle, ObsidianWindowToolbarStyle style) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            if (@available(macOS 11.0, *)) {
                NSWindowToolbarStyle nsStyle;
                switch (style) {
                    case ObsidianWindowToolbarStyleExpanded: nsStyle = NSWindowToolbarStyleExpanded; break;
                    case ObsidianWindowToolbarStylePreference: nsStyle = NSWindowToolbarStylePreference; break;
                    case ObsidianWindowToolbarStyleUnified: nsStyle = NSWindowToolbarStyleUnified; break;
                    case ObsidianWindowToolbarStyleUnifiedCompact: nsStyle = NSWindowToolbarStyleUnifiedCompact; break;
                    default: nsStyle = NSWindowToolbarStyleAutomatic; break;
                }
                [wrapper.window setToolbarStyle:nsStyle];
            }
        }
    }
}

ObsidianWindowToolbarStyle obsidian_macos_window_get_toolbar_style(ObsidianWindowHandle handle) {
    if (!handle) return ObsidianWindowToolbarStyleAutomatic;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            if (@available(macOS 11.0, *)) {
                NSWindowToolbarStyle nsStyle = [wrapper.window toolbarStyle];
                switch (nsStyle) {
                    case NSWindowToolbarStyleExpanded:
                        return ObsidianWindowToolbarStyleExpanded;
                    case NSWindowToolbarStylePreference:
                        return ObsidianWindowToolbarStylePreference;
                    case NSWindowToolbarStyleUnified:
                        return ObsidianWindowToolbarStyleUnified;
                    case NSWindowToolbarStyleUnifiedCompact:
                        return ObsidianWindowToolbarStyleUnifiedCompact;
                    case NSWindowToolbarStyleAutomatic:
                    default:
                        return ObsidianWindowToolbarStyleAutomatic;
                }
            }
        }
        return ObsidianWindowToolbarStyleAutomatic;
    }
}

void obsidian_macos_window_set_titlebar_separator_style(ObsidianWindowHandle handle, ObsidianWindowTitlebarSeparatorStyle style) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            if (@available(macOS 11.0, *)) {
                NSTitlebarSeparatorStyle nsStyle;
                switch (style) {
                    case ObsidianWindowTitlebarSeparatorStyleNone: nsStyle = NSTitlebarSeparatorStyleNone; break;
                    case ObsidianWindowTitlebarSeparatorStyleLine: nsStyle = NSTitlebarSeparatorStyleLine; break;
                    case ObsidianWindowTitlebarSeparatorStyleShadow: nsStyle = NSTitlebarSeparatorStyleShadow; break;
                    default: nsStyle = NSTitlebarSeparatorStyleAutomatic; break;
                }
                [wrapper.window setTitlebarSeparatorStyle:nsStyle];
            }
        }
    }
}

ObsidianWindowTitlebarSeparatorStyle obsidian_macos_window_get_titlebar_separator_style(ObsidianWindowHandle handle) {
    if (!handle) return ObsidianWindowTitlebarSeparatorStyleAutomatic;
    
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            if (@available(macOS 11.0, *)) {
                NSTitlebarSeparatorStyle nsStyle = [wrapper.window titlebarSeparatorStyle];
                switch (nsStyle) {
                    case NSTitlebarSeparatorStyleNone:
                        return ObsidianWindowTitlebarSeparatorStyleNone;
                    case NSTitlebarSeparatorStyleLine:
                        return ObsidianWindowTitlebarSeparatorStyleLine;
                    case NSTitlebarSeparatorStyleShadow:
                        return ObsidianWindowTitlebarSeparatorStyleShadow;
                    case NSTitlebarSeparatorStyleAutomatic:
                    default:
                        return ObsidianWindowTitlebarSeparatorStyleAutomatic;
                }
            }
        }
        return ObsidianWindowTitlebarSeparatorStyleAutomatic;
    }
}

void obsidian_macos_window_configure_for_sidebar(ObsidianWindowHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianWindowWrapper* wrapper = (__bridge ObsidianWindowWrapper*)handle;
        if (wrapper && wrapper.window) {
            NSWindow* window = wrapper.window;
            // Add FullSizeContentView so content extends behind titlebar
            [window setStyleMask:[window styleMask] | NSWindowStyleMaskFullSizeContentView];
            // Hide title for cleaner sidebar look
            [window setTitleVisibility:NSWindowTitleHidden];
            // Make titlebar transparent
            [window setTitlebarAppearsTransparent:YES];
        }
    }
}

} // extern "C"

