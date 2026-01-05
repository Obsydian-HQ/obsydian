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
        // Use the initial window size as minimum to prevent shrinking below creation size
        // This ensures the window maintains at least its initial dimensions
        NSSize minSize = NSMakeSize(params.width, params.height);
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
        // DEBUG: Log window state before showing
        NSRect beforeShowFrame = [_window frame];
        NSRect beforeShowContentRect = [_window contentRectForFrameRect:beforeShowFrame];
        NSSize beforeContentMinSize = [_window contentMinSize];
        NSLog(@"DEBUG: Before makeKeyAndOrderFront: - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
              beforeShowFrame.origin.x, beforeShowFrame.origin.y, beforeShowFrame.size.width, beforeShowFrame.size.height,
              beforeShowContentRect.origin.x, beforeShowContentRect.origin.y, beforeShowContentRect.size.width, beforeShowContentRect.size.height);
        NSLog(@"DEBUG: - contentMinSize: (%.0f, %.0f)", beforeContentMinSize.width, beforeContentMinSize.height);
        
        // DEBUG: Check if there's a content view controller
        NSViewController* contentViewController = [_window contentViewController];
        if (contentViewController) {
            NSView* contentView = [contentViewController view];
            NSSize vcPreferredSize = [contentViewController preferredContentSize];
            NSSize vcViewIntrinsicSize = contentView ? [contentView intrinsicContentSize] : NSMakeSize(0, 0);
            NSLog(@"DEBUG: - Content view controller preferredContentSize: (%.0f, %.0f)", vcPreferredSize.width, vcPreferredSize.height);
            if (contentView) {
                NSLog(@"DEBUG: - Content view controller view intrinsicContentSize: (%.0f, %.0f)", vcViewIntrinsicSize.width, vcViewIntrinsicSize.height);
                NSLog(@"DEBUG: - Content view controller view frame: (%.0f, %.0f, %.0f, %.0f)",
                      contentView.frame.origin.x, contentView.frame.origin.y, contentView.frame.size.width, contentView.frame.size.height);
            }
        }
        
        [_window makeKeyAndOrderFront:nil];
        
        // DEBUG: Log window state immediately after makeKeyAndOrderFront:
        NSRect afterShowFrame = [_window frame];
        NSRect afterShowContentRect = [_window contentRectForFrameRect:afterShowFrame];
        NSLog(@"DEBUG: After makeKeyAndOrderFront: - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
              afterShowFrame.origin.x, afterShowFrame.origin.y, afterShowFrame.size.width, afterShowFrame.size.height,
              afterShowContentRect.origin.x, afterShowContentRect.origin.y, afterShowContentRect.size.width, afterShowContentRect.size.height);
        
        // If the window was resized, restore it asynchronously to let macOS finish its internal operations
        if (afterShowContentRect.size.width != beforeShowContentRect.size.width ||
            afterShowContentRect.size.height != beforeShowContentRect.size.height) {
            NSLog(@"DEBUG: Window was resized during makeKeyAndOrderFront: from (%.0f, %.0f) to (%.0f, %.0f) - Restoring asynchronously",
                  beforeShowContentRect.size.width, beforeShowContentRect.size.height,
                  afterShowContentRect.size.width, afterShowContentRect.size.height);
            
            // Store the desired frame for async restoration
            NSRect desiredFrame = beforeShowFrame;
            
            // Restore asynchronously to let macOS finish its internal resize operations
            dispatch_async(dispatch_get_main_queue(), ^{
                // Check current size again (might have changed)
                NSRect currentFrame = [self->_window frame];
                NSRect currentContentRect = [self->_window contentRectForFrameRect:currentFrame];
                
                // Only restore if still wrong
                if (currentContentRect.size.width != desiredFrame.size.width ||
                    currentContentRect.size.height != desiredFrame.size.height) {
                    NSLog(@"DEBUG: Restoring window frame asynchronously from (%.0f, %.0f) to (%.0f, %.0f)",
                          currentContentRect.size.width, currentContentRect.size.height,
                          desiredFrame.size.width, desiredFrame.size.height);
                    
                    [self->_window setFrame:desiredFrame display:YES];
                    
                    // Verify restoration
                    NSRect restoredFrame = [self->_window frame];
                    NSRect restoredContentRect = [self->_window contentRectForFrameRect:restoredFrame];
                    NSLog(@"DEBUG: After async restoration - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
                          restoredFrame.origin.x, restoredFrame.origin.y, restoredFrame.size.width, restoredFrame.size.height,
                          restoredContentRect.origin.x, restoredContentRect.origin.y, restoredContentRect.size.width, restoredContentRect.size.height);
                }
            });
        }
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
                // DEBUG: Log window state before setContentViewController
                NSRect beforeFrame = [wrapper.window frame];
                NSRect beforeContentRect = [wrapper.window contentRectForFrameRect:beforeFrame];
                NSSize beforeContentMinSize = [wrapper.window contentMinSize];
                NSLog(@"DEBUG: Before setContentViewController - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
                      beforeFrame.origin.x, beforeFrame.origin.y, beforeFrame.size.width, beforeFrame.size.height,
                      beforeContentRect.origin.x, beforeContentRect.origin.y, beforeContentRect.size.width, beforeContentRect.size.height);
                NSLog(@"DEBUG: - contentMinSize: (%.0f, %.0f)", beforeContentMinSize.width, beforeContentMinSize.height);
                
                // DEBUG: Log view controller state
                NSSize vcPreferredSize = [viewController preferredContentSize];
                NSView* vcView = [viewController view];
                NSSize vcViewIntrinsicSize = vcView ? [vcView intrinsicContentSize] : NSMakeSize(0, 0);
                NSLog(@"DEBUG: - viewController preferredContentSize BEFORE: (%.0f, %.0f)", vcPreferredSize.width, vcPreferredSize.height);
                if (vcView) {
                    NSLog(@"DEBUG: - viewController view intrinsicContentSize: (%.0f, %.0f)", vcViewIntrinsicSize.width, vcViewIntrinsicSize.height);
                    NSLog(@"DEBUG: - viewController view frame BEFORE: (%.0f, %.0f, %.0f, %.0f)", vcView.frame.origin.x, vcView.frame.origin.y, vcView.frame.size.width, vcView.frame.size.height);
                }
                
                // CRITICAL: Before setting the content view controller, ensure:
                // 1. The view controller's view frame matches the window's content size
                // 2. The view controller's preferredContentSize matches the window's content size
                // macOS uses BOTH of these to determine window size during makeKeyAndOrderFront:
                NSRect currentFrame = [wrapper.window frame];
                NSRect currentContentRect = [wrapper.window contentRectForFrameRect:currentFrame];
                NSSize currentContentSize = currentContentRect.size;
                
                // CRITICAL: Set the view's frame to match the window's content size BEFORE setContentViewController:
                // macOS uses the view's frame to determine window size. If the view has a smaller frame,
                // macOS will shrink the window to match it (constrained by contentMinSize).
                if (vcView) {
                    NSRect currentViewFrame = vcView.frame;
                    NSRect desiredViewFrame = NSMakeRect(0, 0, currentContentSize.width, currentContentSize.height);
                    if (currentViewFrame.size.width != desiredViewFrame.size.width ||
                        currentViewFrame.size.height != desiredViewFrame.size.height) {
                        NSLog(@"DEBUG: - Setting view controller view frame from (%.0f, %.0f) to (%.0f, %.0f)",
                              currentViewFrame.size.width, currentViewFrame.size.height,
                              desiredViewFrame.size.width, desiredViewFrame.size.height);
                        [vcView setFrame:desiredViewFrame];
                        
                        // CRITICAL: After setting the frame, we need to ensure the view's constraints
                        // don't override it. Disable autoresizing mask and add explicit constraints
                        // to enforce minimum size if needed.
                        [vcView setTranslatesAutoresizingMaskIntoConstraints:NO];
                        
                        // Add minimum width and height constraints to prevent shrinking
                        NSLayoutConstraint* minWidth = [NSLayoutConstraint constraintWithItem:vcView
                                                                                    attribute:NSLayoutAttributeWidth
                                                                                    relatedBy:NSLayoutRelationGreaterThanOrEqual
                                                                                       toItem:nil
                                                                                    attribute:NSLayoutAttributeNotAnAttribute
                                                                                   multiplier:1.0
                                                                                     constant:currentContentSize.width];
                        NSLayoutConstraint* minHeight = [NSLayoutConstraint constraintWithItem:vcView
                                                                                     attribute:NSLayoutAttributeHeight
                                                                                     relatedBy:NSLayoutRelationGreaterThanOrEqual
                                                                                        toItem:nil
                                                                                     attribute:NSLayoutAttributeNotAnAttribute
                                                                                    multiplier:1.0
                                                                                      constant:currentContentSize.height];
                        [minWidth setPriority:NSLayoutPriorityRequired];
                        [minHeight setPriority:NSLayoutPriorityRequired];
                        [vcView addConstraint:minWidth];
                        [vcView addConstraint:minHeight];
                        NSLog(@"DEBUG: - Added minimum size constraints: width >= %.0f, height >= %.0f",
                              currentContentSize.width, currentContentSize.height);
                    }
                }
                
                // Update the view controller's preferredContentSize to match the window's content size
                // This ensures macOS doesn't resize the window to a different size during makeKeyAndOrderFront:
                if (vcPreferredSize.width != currentContentSize.width || 
                    vcPreferredSize.height != currentContentSize.height) {
                    [viewController setPreferredContentSize:currentContentSize];
                    NSLog(@"DEBUG: - Updated viewController preferredContentSize from (%.0f, %.0f) to (%.0f, %.0f)",
                          vcPreferredSize.width, vcPreferredSize.height,
                          currentContentSize.width, currentContentSize.height);
                }
                
                // CRITICAL: Use setContentViewController: instead of addSubview:
                // This is REQUIRED for native sidebar behavior:
                // 1. Collapse button appears automatically
                // 2. Proper window resizing behavior (vertical and horizontal)
                // 3. Correct view controller lifecycle management
                // 4. Window size constraints work properly
                [wrapper.window setContentViewController:viewController];
                
                // DEBUG: Log window state after setContentViewController
                NSRect afterFrame = [wrapper.window frame];
                NSRect afterContentRect = [wrapper.window contentRectForFrameRect:afterFrame];
                NSLog(@"DEBUG: After setContentViewController - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
                      afterFrame.origin.x, afterFrame.origin.y, afterFrame.size.width, afterFrame.size.height,
                      afterContentRect.origin.x, afterContentRect.origin.y, afterContentRect.size.width, afterContentRect.size.height);
                
                // If the window was resized, restore it
                if (afterContentRect.size.width != beforeContentRect.size.width ||
                    afterContentRect.size.height != beforeContentRect.size.height) {
                    NSLog(@"DEBUG: Window was resized during setContentViewController: from (%.0f, %.0f) to (%.0f, %.0f) - Restoring",
                          beforeContentRect.size.width, beforeContentRect.size.height,
                          afterContentRect.size.width, afterContentRect.size.height);
                    
                    // Restore the original frame
                    [wrapper.window setFrame:beforeFrame display:NO];
                    
                    // Log after restoration
                    NSRect restoredFrame = [wrapper.window frame];
                    NSRect restoredContentRect = [wrapper.window contentRectForFrameRect:restoredFrame];
                    NSLog(@"DEBUG: After restoration - Frame: (%.0f, %.0f, %.0f, %.0f), Content: (%.0f, %.0f, %.0f, %.0f)",
                          restoredFrame.origin.x, restoredFrame.origin.y, restoredFrame.size.width, restoredFrame.size.height,
                          restoredContentRect.origin.x, restoredContentRect.origin.y, restoredContentRect.size.width, restoredContentRect.size.height);
                }
            }
        }
    }
}

} // extern "C"

