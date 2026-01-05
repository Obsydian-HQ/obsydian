#import "macos_window.h"
#import <AppKit/AppKit.h>

@interface ObsidianWindowWrapper : NSObject
@property (nonatomic, strong) NSWindow* window;
@end

@implementation ObsidianWindowWrapper

- (instancetype)initWithParams:(ObsidianWindowParams)params {
    self = [super init];
    if (self) {
        NSRect contentRect = NSMakeRect(0, 0, params.width, params.height);
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | 
                                     NSWindowStyleMaskClosable | 
                                     NSWindowStyleMaskResizable | 
                                     NSWindowStyleMaskMiniaturizable |
                                     NSWindowStyleMaskFullSizeContentView;
        
        _window = [[NSWindow alloc] initWithContentRect:contentRect
                                               styleMask:styleMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        
        [_window setReleasedWhenClosed:NO];
        
        if (params.title) {
            [_window setTitle:[NSString stringWithUTF8String:params.title]];
        }
        
        [_window center];
    }
    return self;
}

@end

extern "C" {

ObsidianWindowHandle obsidian_macos_create_window(ObsidianWindowParams params) {
    @autoreleasepool {
        ObsidianWindowWrapper* w = [[ObsidianWindowWrapper alloc] initWithParams:params];
        return w && w.window ? (__bridge_retained void*)w : nullptr;
    }
}

void obsidian_macos_show_window(ObsidianWindowHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        [((__bridge ObsidianWindowWrapper*)handle).window makeKeyAndOrderFront:nil];
    }
}

void obsidian_macos_destroy_window(ObsidianWindowHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianWindowWrapper* w = (__bridge_transfer ObsidianWindowWrapper*)handle;
        [w.window close];
    }
}

bool obsidian_macos_window_is_valid(ObsidianWindowHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianWindowWrapper*)handle).window != nil;
    }
}

void* obsidian_macos_window_get_content_view(ObsidianWindowHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)[((__bridge ObsidianWindowWrapper*)handle).window contentView];
    }
}

void* obsidian_macos_window_get_nswindow(ObsidianWindowHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)((__bridge ObsidianWindowWrapper*)handle).window;
    }
}

void obsidian_macos_window_set_content_view_controller(ObsidianWindowHandle handle, void* vcHandle) {
    if (!handle || !vcHandle) return;
    @autoreleasepool {
        [((__bridge ObsidianWindowWrapper*)handle).window setContentViewController:(__bridge NSViewController*)vcHandle];
    }
}

void obsidian_macos_window_set_toolbar(ObsidianWindowHandle handle, void* toolbarHandle) {
    if (!handle || !toolbarHandle) return;
    @autoreleasepool {
        [((__bridge ObsidianWindowWrapper*)handle).window setToolbar:(__bridge NSToolbar*)toolbarHandle];
    }
}

void* obsidian_macos_window_get_toolbar(ObsidianWindowHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)[((__bridge ObsidianWindowWrapper*)handle).window toolbar];
    }
}

void obsidian_macos_window_set_toolbar_style(ObsidianWindowHandle handle, ObsidianWindowToolbarStyle style) {
    if (!handle) return;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            NSWindowToolbarStyle nsStyle;
            switch (style) {
                case ObsidianWindowToolbarStyleExpanded: nsStyle = NSWindowToolbarStyleExpanded; break;
                case ObsidianWindowToolbarStylePreference: nsStyle = NSWindowToolbarStylePreference; break;
                case ObsidianWindowToolbarStyleUnified: nsStyle = NSWindowToolbarStyleUnified; break;
                case ObsidianWindowToolbarStyleUnifiedCompact: nsStyle = NSWindowToolbarStyleUnifiedCompact; break;
                default: nsStyle = NSWindowToolbarStyleAutomatic; break;
            }
            [((__bridge ObsidianWindowWrapper*)handle).window setToolbarStyle:nsStyle];
        }
    }
}

ObsidianWindowToolbarStyle obsidian_macos_window_get_toolbar_style(ObsidianWindowHandle handle) {
    if (!handle) return ObsidianWindowToolbarStyleAutomatic;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            switch ([((__bridge ObsidianWindowWrapper*)handle).window toolbarStyle]) {
                case NSWindowToolbarStyleExpanded: return ObsidianWindowToolbarStyleExpanded;
                case NSWindowToolbarStylePreference: return ObsidianWindowToolbarStylePreference;
                case NSWindowToolbarStyleUnified: return ObsidianWindowToolbarStyleUnified;
                case NSWindowToolbarStyleUnifiedCompact: return ObsidianWindowToolbarStyleUnifiedCompact;
                default: return ObsidianWindowToolbarStyleAutomatic;
            }
        }
        return ObsidianWindowToolbarStyleAutomatic;
    }
}

void obsidian_macos_window_set_titlebar_separator_style(ObsidianWindowHandle handle, ObsidianWindowTitlebarSeparatorStyle style) {
    if (!handle) return;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            NSTitlebarSeparatorStyle nsStyle;
            switch (style) {
                case ObsidianWindowTitlebarSeparatorStyleNone: nsStyle = NSTitlebarSeparatorStyleNone; break;
                case ObsidianWindowTitlebarSeparatorStyleLine: nsStyle = NSTitlebarSeparatorStyleLine; break;
                case ObsidianWindowTitlebarSeparatorStyleShadow: nsStyle = NSTitlebarSeparatorStyleShadow; break;
                default: nsStyle = NSTitlebarSeparatorStyleAutomatic; break;
            }
            [((__bridge ObsidianWindowWrapper*)handle).window setTitlebarSeparatorStyle:nsStyle];
        }
    }
}

ObsidianWindowTitlebarSeparatorStyle obsidian_macos_window_get_titlebar_separator_style(ObsidianWindowHandle handle) {
    if (!handle) return ObsidianWindowTitlebarSeparatorStyleAutomatic;
    @autoreleasepool {
        if (@available(macOS 11.0, *)) {
            switch ([((__bridge ObsidianWindowWrapper*)handle).window titlebarSeparatorStyle]) {
                case NSTitlebarSeparatorStyleNone: return ObsidianWindowTitlebarSeparatorStyleNone;
                case NSTitlebarSeparatorStyleLine: return ObsidianWindowTitlebarSeparatorStyleLine;
                case NSTitlebarSeparatorStyleShadow: return ObsidianWindowTitlebarSeparatorStyleShadow;
                default: return ObsidianWindowTitlebarSeparatorStyleAutomatic;
            }
        }
        return ObsidianWindowTitlebarSeparatorStyleAutomatic;
    }
}

void obsidian_macos_window_configure_for_sidebar(ObsidianWindowHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        NSWindow* window = ((__bridge ObsidianWindowWrapper*)handle).window;
        if (!([window styleMask] & NSWindowStyleMaskFullSizeContentView)) {
            [window setStyleMask:[window styleMask] | NSWindowStyleMaskFullSizeContentView];
        }
        [window setTitleVisibility:NSWindowTitleHidden];
        [window setTitlebarAppearsTransparent:YES];
    }
}

void obsidian_macos_window_set_min_size(ObsidianWindowHandle handle, double minWidth, double minHeight) {
    if (!handle) return;
    @autoreleasepool {
        NSWindow* window = ((__bridge ObsidianWindowWrapper*)handle).window;
        [window setMinSize:NSMakeSize(minWidth, minHeight)];
    }
}

void obsidian_macos_window_get_min_size(ObsidianWindowHandle handle, double* outWidth, double* outHeight) {
    if (!handle) return;
    @autoreleasepool {
        NSWindow* window = ((__bridge ObsidianWindowWrapper*)handle).window;
        NSSize minSize = [window minSize];
        if (outWidth) *outWidth = minSize.width;
        if (outHeight) *outHeight = minSize.height;
    }
}

void obsidian_macos_window_update_constraints(ObsidianWindowHandle handle) {
    // No-op - legacy function
}

}
