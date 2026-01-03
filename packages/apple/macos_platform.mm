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
        
        // Activate the app
        [app activateIgnoringOtherApps:YES];
        
        // Run the event loop
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

} // extern "C"

