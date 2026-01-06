/**
 * macOS Platform FFI - Objective-C++ Implementation
 * 
 * Handles NSApplication initialization and event loop management
 */

#import "macos_platform.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

static bool g_platform_initialized = false;
static bool g_should_run = false;

extern "C" {

bool obsidian_macos_platform_init() {
    if (g_platform_initialized) {
        return true;
    }
    
    @autoreleasepool {
        // Initialize NSApplication if not already initialized
        NSApplication* app = [NSApplication sharedApplication];
        if (!app) {
            return false;
        }
        
        // Set activation policy to allow the app to appear in the dock
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        g_platform_initialized = true;
        return true;
    }
}

void obsidian_macos_platform_run() {
    if (!g_platform_initialized) {
        if (!obsidian_macos_platform_init()) {
            return;
        }
    }
    
    g_should_run = true;
    
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        
        // Activate the app and bring to foreground
        [app activateIgnoringOtherApps:YES];
        
        // Ensure any visible windows become key/main after activation
        // This is necessary for programmatic (non-NIB) apps where windows
        // may be shown before the app is activated
        for (NSWindow* window in app.windows) {
            if (![window isKindOfClass:NSClassFromString(@"TUINSWindow")] && 
                window.isVisible) {
                [window makeKeyAndOrderFront:nil];
                break;  // Only need to make one window key
            }
        }
        
        // Run the main event loop
        [app run];
    }
}

void obsidian_macos_platform_stop() {
    g_should_run = false;
    
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app stop:nil];
        
        // Post a dummy event to wake up the event loop
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSZeroPoint
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [app postEvent:event atStart:NO];
    }
}

void obsidian_macos_platform_cleanup() {
    if (!g_platform_initialized) {
        return;
    }
    
    obsidian_macos_platform_stop();
    g_platform_initialized = false;
}

void obsidian_macos_view_set_frame(void* viewHandle, 
                                    double x, double y, 
                                    double width, double height) {
    if (!viewHandle) return;
    
    @autoreleasepool {
        NSView* view = (__bridge NSView*)viewHandle;
        if (view) {
            NSLog(@"[SetFrame] %@ -> x=%.0f y=%.0f w=%.0f h=%.0f", 
                  NSStringFromClass([view class]), x, y, width, height);
            view.frame = NSMakeRect(x, y, width, height);
        }
    }
}

void obsidian_macos_view_get_bounds(void* viewHandle,
                                     double* outX, double* outY,
                                     double* outWidth, double* outHeight) {
    if (!viewHandle) {
        if (outX) *outX = 0;
        if (outY) *outY = 0;
        if (outWidth) *outWidth = 0;
        if (outHeight) *outHeight = 0;
        return;
    }
    
    @autoreleasepool {
        NSView* view = (__bridge NSView*)viewHandle;
        if (view) {
            NSRect bounds = view.bounds;
            if (outX) *outX = bounds.origin.x;
            if (outY) *outY = bounds.origin.y;
            if (outWidth) *outWidth = bounds.size.width;
            if (outHeight) *outHeight = bounds.size.height;
        }
    }
}

} // extern "C"

