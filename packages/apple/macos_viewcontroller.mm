/**
 * macOS ViewController FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSViewController APIs
 */

#import "macos_viewcontroller.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal view controller wrapper class
@interface ObsidianViewControllerWrapper : NSObject {
    NSViewController* _viewController;
}

@property (nonatomic, strong) NSViewController* viewController;

- (instancetype)initWithParams:(ObsidianViewControllerParams)params;
- (void)setView:(void*)viewHandle;
- (void*)getView;
- (bool)isValid;

@end

@implementation ObsidianViewControllerWrapper

- (instancetype)initWithParams:(ObsidianViewControllerParams)params {
    self = [super init];
    if (self) {
        // Create NSViewController
        _viewController = [[NSViewController alloc] init];
        
        // Initially, the view controller has no view
        // The view will be set via setView:
    }
    return self;
}

- (void)setView:(void*)viewHandle {
    if (!_viewController) return;
    
    if (viewHandle) {
        // The viewHandle should be a direct NSView* handle
        // (obtained via obsidian_macos_vstack_get_view or similar)
        NSView* view = (__bridge NSView*)viewHandle;
        if (view) {
            // CRITICAL: Remove view from its current parent if it has one
            // Views can only have one parent at a time in AppKit
            if ([view superview]) {
                [view removeFromSuperview];
            }
            
            // Set the view controller's view
            [_viewController setView:view];
        }
    }
    // Note: We don't explicitly clear the view with setView:nil
    // because NSViewController manages its own view lifecycle
    // If viewHandle is null, we simply don't set a new view
}

- (void*)getView {
    if (!_viewController) return nullptr;
    
    NSView* view = [_viewController view];
    if (!view) return nullptr;
    
    // Return the view as an opaque handle
    // Note: This is a borrowed reference, caller should not release it
    return (__bridge void*)view;
}

- (bool)isValid {
    return _viewController != nil;
}

- (NSViewController*)viewController {
    return _viewController;
}

@end

// C interface implementation
extern "C" {

ObsidianViewControllerHandle obsidian_macos_create_viewcontroller(ObsidianViewControllerParams params) {
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = [[ObsidianViewControllerWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.viewController) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_viewcontroller_set_view(ObsidianViewControllerHandle viewControllerHandle,
                                            void* viewHandle) {
    if (!viewControllerHandle) return;
    
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)viewControllerHandle;
        [wrapper setView:viewHandle];
    }
}

void* obsidian_macos_viewcontroller_get_view(ObsidianViewControllerHandle viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)viewControllerHandle;
        return [wrapper getView];
    }
}

void* obsidian_macos_viewcontroller_get_viewcontroller(ObsidianViewControllerHandle viewControllerHandle) {
    if (!viewControllerHandle) return nullptr;
    
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)viewControllerHandle;
        if (!wrapper || !wrapper.viewController) return nullptr;
        
        // Return the actual NSViewController* as an opaque handle
        // This is a borrowed reference, caller should not release it
        return (__bridge void*)wrapper.viewController;
    }
}

bool obsidian_macos_viewcontroller_is_valid(ObsidianViewControllerHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)handle;
        return [wrapper isValid];
    }
}

void obsidian_macos_destroy_viewcontroller(ObsidianViewControllerHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release the wrapper (ARC will handle cleanup)
        ObsidianViewControllerWrapper* wrapper = (__bridge_transfer ObsidianViewControllerWrapper*)handle;
        // Clear the view before releasing
        [wrapper setView:nullptr];
        wrapper = nil;
    }
}

} // extern "C"
