/**
 * macOS Router Integration Implementation
 * 
 * Integrates Obsydian Router with macOS NSViewController system.
 * 
 * IMPORTANT DESIGN NOTE:
 * The router adds its root view as a subview of the window's existing contentView,
 * rather than using setContentViewController:. This is intentional because:
 * 
 * 1. setContentViewController: replaces the window's contentView entirely, which
 *    causes timing issues in programmatic (non-NIB) apps when called before
 *    NSApplication is activated - the window may not appear on screen.
 * 
 * 2. Using addSubview: to the existing contentView is the standard approach
 *    used by other C/C++ frameworks (SDL, GLFW, webui, sokol_app, etc.)
 * 
 * 3. This allows multiple components (Router, Sidebar, SplitView) to coexist
 *    by adding their views as siblings in the window's content view hierarchy.
 */

#import "macos_router.h"
#import "macos_viewcontroller.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <string>
#import <map>

@interface ObsidianRouterWrapper : NSObject
@property (nonatomic, strong) NSViewController* rootViewController;
@property (nonatomic, strong) NSViewController* currentViewController;
@property (nonatomic, weak) NSWindow* window;
@property (nonatomic, assign) void* routerHandle;  // C++ Router* pointer
@property (nonatomic, strong) NSMutableDictionary<NSString*, NSViewController*>* viewControllers;
@property (nonatomic, strong) NSMutableArray<NSString*>* navigationStack;
@end

@implementation ObsidianRouterWrapper

- (instancetype)initWithParams:(ObsidianMacOSRouterParams)params {
    self = [super init];
    if (self) {
        _routerHandle = params.routerHandle;
        
        // Get window from handle
        if (params.windowHandle) {
            void* windowPtr = obsidian_macos_window_get_nswindow(params.windowHandle);
            if (windowPtr) {
                _window = (__bridge NSWindow*)windowPtr;
            }
        }
        
        // Create root view controller
        _rootViewController = [[NSViewController alloc] init];
        NSView* rootView = [[NSView alloc] init];
        [rootView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [_rootViewController setView:rootView];
        
        // Add router's root view as a subview of window's content view
        // NOTE: We do NOT use setContentViewController: as it prevents the window from displaying
        // when called before the app is activated. Adding as subview works correctly.
        if (_window) {
            NSView* contentView = _window.contentView;
            if (contentView) {
                [rootView setFrame:contentView.bounds];
                [rootView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
                [contentView addSubview:rootView];
            }
        }
        
        _viewControllers = [[NSMutableDictionary alloc] init];
        _navigationStack = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)handleNavigationToPath:(const char*)path {
    if (!path) return;
    
    NSString* pathString = [NSString stringWithUTF8String:path];
    
    // Get or create view controller for this route
    NSViewController* viewController = _viewControllers[pathString];
    
    if (!viewController) {
        // Create new view controller for this route
        viewController = [[NSViewController alloc] init];
        NSView* view = [[NSView alloc] init];
        [view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [viewController setView:view];
        
        _viewControllers[pathString] = viewController;
    }
    
    // Update navigation stack
    if (![_navigationStack containsObject:pathString]) {
        [_navigationStack addObject:pathString];
    }
    
    // Transition to new view controller
    [self transitionToViewController:viewController];
    
    _currentViewController = viewController;
}

- (void)transitionToViewController:(NSViewController*)viewController {
    if (!viewController || !_rootViewController) return;
    
    // Remove current child view controller
    if (_currentViewController && _currentViewController.parentViewController == _rootViewController) {
        [_currentViewController.view removeFromSuperview];
        [_currentViewController removeFromParentViewController];
    }
    
    // Add new child view controller
    [_rootViewController addChildViewController:viewController];
    [viewController.view setFrame:_rootViewController.view.bounds];
    [_rootViewController.view addSubview:viewController.view];
    
    // Animate transition (optional - can be enhanced with custom animations)
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext* context) {
        context.duration = 0.2;
        context.allowsImplicitAnimation = YES;
        viewController.view.alphaValue = 1.0;
    } completionHandler:nil];
}

- (void)dealloc {
    // Clean up view controllers
    for (NSViewController* vc in _viewControllers.allValues) {
        [vc.view removeFromSuperview];
        [vc removeFromParentViewController];
    }
    [_viewControllers removeAllObjects];
    [_navigationStack removeAllObjects];
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
        return wrapper != nil && wrapper.rootViewController != nil;
    }
}

void* obsidian_macos_router_get_current_viewcontroller(ObsidianMacOSRouterHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = (__bridge ObsidianRouterWrapper*)handle;
        if (wrapper.currentViewController) {
            return (__bridge void*)wrapper.currentViewController;
        }
        return (__bridge void*)wrapper.rootViewController;
    }
}

void obsidian_macos_router_handle_navigation(ObsidianMacOSRouterHandle handle, const char* path) {
    if (!handle || !path) return;
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = (__bridge ObsidianRouterWrapper*)handle;
        [wrapper handleNavigationToPath:path];
    }
}

void obsidian_macos_router_setup_navigation_callbacks(ObsidianMacOSRouterHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianRouterWrapper* wrapper = (__bridge ObsidianRouterWrapper*)handle;
        
        // Get Router pointer
        void* routerPtr = wrapper.routerHandle;
        if (!routerPtr) return;
        
        // Note: In a real implementation, we would set up a callback here
        // that calls obsidian_macos_router_handle_navigation when Router navigates.
        // This requires integration with the C++ Router class's setOnNavigation callback.
        // For now, this is a placeholder that can be called from C++ code.
    }
}

} // extern "C"
