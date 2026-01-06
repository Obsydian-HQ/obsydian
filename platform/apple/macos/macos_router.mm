/**
 * macOS Router Integration Implementation
 * 
 * Integrates Obsydian Router with macOS.
 * 
 * DESIGN NOTE:
 * The router does NOT create its own view hierarchy. Route components
 * are responsible for adding their content to the window using window.setContent().
 * The router's role is to:
 * 1. Track navigation state
 * 2. Clear old content before rendering new routes
 * 3. Coordinate with the C++ Router for navigation callbacks
 */

#import "macos_router.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <string>

@interface ObsidianRouterWrapper : NSObject
@property (nonatomic, weak) NSWindow* window;
@property (nonatomic, assign) void* windowHandle;  // ObsidianWindowHandle
@property (nonatomic, assign) void* routerHandle;  // C++ Router* pointer
@property (nonatomic, strong) NSString* currentPath;
@end

@implementation ObsidianRouterWrapper

- (instancetype)initWithParams:(ObsidianMacOSRouterParams)params {
    self = [super init];
    if (self) {
        _routerHandle = params.routerHandle;
        _windowHandle = params.windowHandle;
        
        // Get window from handle
        if (params.windowHandle) {
            void* windowPtr = obsidian_macos_window_get_nswindow(params.windowHandle);
            if (windowPtr) {
                _window = (__bridge NSWindow*)windowPtr;
            }
        }
        
        _currentPath = @"";
    }
    return self;
}

- (void)handleNavigationToPath:(const char*)path {
    if (!path) return;
    
    _currentPath = [NSString stringWithUTF8String:path];
    
    // NOTE: We no longer clear window content here.
    // The ScreenContainer now manages route content:
    // - ScreenContainer stays attached to the window
    // - Individual screens are shown/hidden via setActiveScreen()
    // - Screen content is cleared via screen->clear() before rendering
    // 
    // This follows the react-native-screens pattern where the container
    // persists and only manages child screen visibility.
}

@end

extern "C" {

ObsidianMacOSRouterHandle obsidian_macos_router_create(ObsidianMacOSRouterParams params) {
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = [[ObsidianRouterWrapper alloc] initWithParams:params];
        return wrapper ? (__bridge_retained void*)wrapper : nullptr;
    }
}

void obsidian_macos_router_destroy(ObsidianMacOSRouterHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianRouterWrapper*)handle;
    }
}

bool obsidian_macos_router_is_valid(ObsidianMacOSRouterHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = (__bridge ObsidianRouterWrapper*)handle;
        return wrapper != nil && wrapper.window != nil;
    }
}

void* obsidian_macos_router_get_current_viewcontroller(ObsidianMacOSRouterHandle handle) {
    // No longer managing view controllers - return nullptr
    (void)handle;
    return nullptr;
}

void obsidian_macos_router_handle_navigation(ObsidianMacOSRouterHandle handle, const char* path) {
    if (!handle || !path) return;
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = (__bridge ObsidianRouterWrapper*)handle;
        [wrapper handleNavigationToPath:path];
    }
}

void obsidian_macos_router_setup_navigation_callbacks(ObsidianMacOSRouterHandle handle) {
    // No-op - navigation is handled by C++ Router calling obsidian_macos_router_handle_navigation
    (void)handle;
}

} // extern "C"
