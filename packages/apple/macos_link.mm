/**
 * macOS Link FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C to create clickable wrappers around any view
 */

#import "macos_link.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

// Internal link wrapper class - wraps any view and makes it clickable
@interface ObsidianLinkWrapper : NSObject {
    NSView* _wrapperView;      // Container view that wraps the child
    NSView* _childView;        // The child view being wrapped
    NSString* _href;           // Route path for navigation
    ObsidianLinkClickCallback _callback;
    void* _userData;
    NSClickGestureRecognizer* _clickRecognizer;
}

@property (nonatomic, strong) NSView* wrapperView;
@property (nonatomic, strong) NSView* childView;
@property (nonatomic, strong) NSString* href;
@property (nonatomic, assign) ObsidianLinkClickCallback callback;
@property (nonatomic, assign) void* userData;

- (instancetype)initWithParams:(ObsidianLinkParams)params;
- (void)setHref:(const char*)href;
- (const char*)getHref;
- (void)setOnClick:(ObsidianLinkClickCallback)callback userData:(void*)userData;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;
- (void)handleClick:(id)sender;

@end

@implementation ObsidianLinkWrapper

- (instancetype)initWithParams:(ObsidianLinkParams)params {
    self = [super init];
    if (self) {
        if (!params.childView) {
            return nil;
        }
        
        // Get the child view
        _childView = (__bridge NSView*)params.childView;
        
        // Create a wrapper view that will contain the child
        // The wrapper will handle clicks and forward them
        _wrapperView = [[NSView alloc] init];
        _wrapperView.wantsLayer = YES;
        
        // Set the wrapper's frame to match the child's frame
        _wrapperView.frame = _childView.frame;
        
        // Add child view to wrapper
        [_wrapperView addSubview:_childView];
        
        // Set up auto-layout constraints to make child fill wrapper
        _childView.translatesAutoresizingMaskIntoConstraints = NO;
        [NSLayoutConstraint activateConstraints:@[
            [_childView.topAnchor constraintEqualToAnchor:_wrapperView.topAnchor],
            [_childView.leadingAnchor constraintEqualToAnchor:_wrapperView.leadingAnchor],
            [_childView.trailingAnchor constraintEqualToAnchor:_wrapperView.trailingAnchor],
            [_childView.bottomAnchor constraintEqualToAnchor:_wrapperView.bottomAnchor]
        ]];
        
        // Create click gesture recognizer
        _clickRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClick:)];
        [_wrapperView addGestureRecognizer:_clickRecognizer];
        
        // Store href
        if (params.href) {
            _href = [NSString stringWithUTF8String:params.href];
        } else {
            _href = @"";
        }
        
        _callback = nullptr;
        _userData = nullptr;
    }
    return self;
}

- (void)setHref:(const char*)href {
    if (href) {
        _href = [NSString stringWithUTF8String:href];
    } else {
        _href = @"";
    }
}

- (const char*)getHref {
    if (_href) {
        return [_href UTF8String];
    }
    return nullptr;
}

- (void)setOnClick:(ObsidianLinkClickCallback)callback userData:(void*)userData {
    _callback = callback;
    _userData = userData;
}

- (void)setVisible:(bool)visible {
    if (_wrapperView) {
        [_wrapperView setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_wrapperView) {
        return ![_wrapperView isHidden];
    }
    return false;
}

- (void)setEnabled:(bool)enabled {
    if (_clickRecognizer) {
        _clickRecognizer.enabled = enabled;
    }
    // Also disable/enable child view if it supports it
    if (_childView && [_childView respondsToSelector:@selector(setEnabled:)]) {
        [(id)_childView setEnabled:enabled];
    }
}

- (bool)isEnabled {
    if (_clickRecognizer) {
        return _clickRecognizer.enabled;
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_wrapperView || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_wrapperView];
    }
}

- (void)removeFromParent {
    if (!_wrapperView) return;
    
    // Remove gesture recognizer
    if (_clickRecognizer) {
        [_wrapperView removeGestureRecognizer:_clickRecognizer];
        _clickRecognizer = nil;
    }
    
    // Remove from superview
    if ([_wrapperView superview]) {
        [_wrapperView removeFromSuperview];
    }
}

- (void)handleClick:(id)sender {
    if (_callback) {
        _callback(_userData);
    }
}

- (NSView*)wrapperView {
    return _wrapperView;
}

- (NSView*)childView {
    return _childView;
}

- (NSString*)href {
    return _href;
}

- (ObsidianLinkClickCallback)callback {
    return _callback;
}

- (void*)userData {
    return _userData;
}

@end

// C interface implementation
extern "C" {

ObsidianLinkHandle obsidian_macos_create_link(ObsidianLinkParams params) {
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = [[ObsidianLinkWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.wrapperView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_link_set_href(ObsidianLinkHandle handle, const char* href) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        [wrapper setHref:href];
    }
}

const char* obsidian_macos_link_get_href(ObsidianLinkHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        return [wrapper getHref];
    }
}

void obsidian_macos_link_set_on_click(ObsidianLinkHandle handle,
                                      ObsidianLinkClickCallback callback,
                                      void* userData) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        [wrapper setOnClick:callback userData:userData];
    }
}

void obsidian_macos_link_set_visible(ObsidianLinkHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_link_is_visible(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_link_set_enabled(ObsidianLinkHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        [wrapper setEnabled:enabled];
    }
}

bool obsidian_macos_link_is_enabled(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        return [wrapper isEnabled];
    }
}

void obsidian_macos_link_add_to_window(ObsidianLinkHandle linkHandle,
                                       void* windowHandle) {
    if (!linkHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)linkHandle;
        [wrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_link_remove_from_parent(ObsidianLinkHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_link(ObsidianLinkHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge_transfer ObsidianLinkWrapper*)handle;
        [wrapper removeFromParent];
        // wrapper will be deallocated when autoreleasepool drains
    }
}

bool obsidian_macos_link_is_valid(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        return wrapper != nil && wrapper.wrapperView != nil;
    }
}

void* obsidian_macos_link_get_view(ObsidianLinkHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianLinkWrapper* wrapper = (__bridge ObsidianLinkWrapper*)handle;
        return (__bridge void*)wrapper.wrapperView;
    }
}

} // extern "C"
