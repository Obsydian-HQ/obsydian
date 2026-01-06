/**
 * macOS Link FFI - Objective-C++ Implementation
 * 
 * Simple navigation component - attaches click handler to child view
 * NO wrapper view - directly uses the child view
 */

#import "macos_link.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

// Internal link handler - attaches click behavior to any view
@interface ObsidianLinkHandler : NSObject {
    NSView* _childView;            // The child view (we don't wrap it, just use it directly)
    NSString* _href;               // Route path for navigation
    ObsidianLinkClickCallback _callback;
    void* _userData;
    NSClickGestureRecognizer* _clickRecognizer;
}

@property (nonatomic, strong) NSView* childView;
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
- (void)cleanup;
- (void)handleClick:(id)sender;

@end

@implementation ObsidianLinkHandler

- (instancetype)initWithParams:(ObsidianLinkParams)params {
    self = [super init];
    if (self) {
        if (!params.childView) {
            NSLog(@"[Link DEBUG] ERROR: childView is NULL!");
            return nil;
        }
        
        // Get the child view - NO WRAPPER, use it directly
        _childView = (__bridge NSView*)params.childView;
        NSLog(@"[Link DEBUG] Created link with childView: %@ frame: %@", 
              NSStringFromClass([_childView class]),
              NSStringFromRect(_childView.frame));
        
        // Attach click gesture recognizer directly to the child view
        _clickRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClick:)];
        [_childView addGestureRecognizer:_clickRecognizer];
        
        // Store href
        if (params.href) {
            _href = [NSString stringWithUTF8String:params.href];
        } else {
            _href = @"";
        }
        
        _callback = nullptr;
        _userData = nullptr;
        
        // Retain self via associated object on child view
        objc_setAssociatedObject(_childView, @selector(handleClick:), self, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
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
    if (_childView) {
        [_childView setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_childView) {
        return ![_childView isHidden];
    }
    return false;
}

- (void)setEnabled:(bool)enabled {
    if (_clickRecognizer) {
        _clickRecognizer.enabled = enabled;
    }
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
    if (!_childView || !windowHandle) {
        return;
    }
    
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_childView];
    }
}

- (void)removeFromParent {
    if (!_childView) return;
    
    // Just remove from superview - keep click gesture intact
    if ([_childView superview]) {
        [_childView removeFromSuperview];
    }
}

- (void)cleanup {
    // Full cleanup - remove gesture and from superview
    if (!_childView) return;
    
    if (_clickRecognizer) {
        [_childView removeGestureRecognizer:_clickRecognizer];
        _clickRecognizer = nil;
    }
    
    if ([_childView superview]) {
        [_childView removeFromSuperview];
    }
}

- (void)handleClick:(id)sender {
    if (_callback) {
        _callback(_userData);
    }
}

- (NSString*)hrefString {
    return _href;
}

@end

// C interface implementation
extern "C" {

ObsidianLinkHandle obsidian_macos_create_link(ObsidianLinkParams params) {
    @autoreleasepool {
        ObsidianLinkHandler* handler = [[ObsidianLinkHandler alloc] initWithParams:params];
        if (handler && handler.childView) {
            return (__bridge_retained void*)handler;
        }
        return nullptr;
    }
}

void obsidian_macos_link_set_href(ObsidianLinkHandle handle, const char* href) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler setHref:href];
    }
}

const char* obsidian_macos_link_get_href(ObsidianLinkHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        return [handler getHref];
    }
}

void obsidian_macos_link_set_on_click(ObsidianLinkHandle handle,
                                      ObsidianLinkClickCallback callback,
                                      void* userData) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler setOnClick:callback userData:userData];
    }
}

void obsidian_macos_link_set_visible(ObsidianLinkHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler setVisible:visible];
    }
}

bool obsidian_macos_link_is_visible(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        return [handler isVisible];
    }
}

void obsidian_macos_link_set_enabled(ObsidianLinkHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler setEnabled:enabled];
    }
}

bool obsidian_macos_link_is_enabled(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        return [handler isEnabled];
    }
}

void obsidian_macos_link_add_to_window(ObsidianLinkHandle linkHandle,
                                       void* windowHandle) {
    if (!linkHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)linkHandle;
        [handler addToWindow:windowHandle];
    }
}

void obsidian_macos_link_remove_from_parent(ObsidianLinkHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler removeFromParent];
    }
}

void obsidian_macos_destroy_link(ObsidianLinkHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge_transfer ObsidianLinkHandler*)handle;
        [handler cleanup];
    }
}

void obsidian_macos_release_link_handle(ObsidianLinkHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianLinkHandler*)handle;
    }
}

bool obsidian_macos_link_is_valid(ObsidianLinkHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        return handler != nil && handler.childView != nil;
    }
}

void* obsidian_macos_link_get_view(ObsidianLinkHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        // Return the child view directly - NO WRAPPER
        return (__bridge void*)handler.childView;
    }
}

} // extern "C"
