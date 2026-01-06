/**
 * macOS Link FFI - Objective-C++ Implementation
 * 
 * Simple navigation component - attaches click handler to child view.
 * NO wrapper view - directly uses the child view.
 * 
 * Following Apple's AppKit patterns and React Native's approach:
 * - For NSButton/NSControl: Use native target/action mechanism (no conflict)
 * - For other views: Use NSClickGestureRecognizer
 * 
 * This avoids the gesture recognizer conflict with NSButton's native click handling.
 */

#import "macos_link.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

// Associated object key for storing the original button target/action
static const void* kOriginalTargetKey = &kOriginalTargetKey;
static const void* kOriginalActionKey = &kOriginalActionKey;

// Internal link handler - attaches click behavior to any view
@interface ObsidianLinkHandler : NSObject {
    NSView* _childView;            // The child view (we don't wrap it, just use it directly)
    NSString* _href;               // Route path for navigation
    ObsidianLinkClickCallback _callback;
    void* _userData;
    ObsidianLinkReleaseCallback _releaseCallback;  // Called on dealloc to free userData
    NSClickGestureRecognizer* _clickRecognizer;  // Used only for non-button views
    BOOL _usesButtonTargetAction;                // True if child is NSButton/NSControl
}

@property (nonatomic, strong) NSView* childView;
@property (nonatomic, assign) ObsidianLinkClickCallback callback;
@property (nonatomic, assign) void* userData;
@property (nonatomic, assign) ObsidianLinkReleaseCallback releaseCallback;

- (instancetype)initWithParams:(ObsidianLinkParams)params;
- (void)dealloc;
- (void)setHref:(const char*)href;
- (const char*)getHref;
- (void)setOnClick:(ObsidianLinkClickCallback)callback 
          userData:(void*)userData 
   releaseCallback:(ObsidianLinkReleaseCallback)releaseCallback;
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
            return nil;
        }
        
        // Get the child view - NO WRAPPER, use it directly
        _childView = (__bridge NSView*)params.childView;
        _usesButtonTargetAction = NO;
        _clickRecognizer = nil;
        
        if ([_childView isKindOfClass:[NSButton class]]) {
            NSButton* button = (NSButton*)_childView;
            
            id originalTarget = [button target];
            SEL originalAction = [button action];
            if (originalTarget && originalAction) {
                objc_setAssociatedObject(button, kOriginalTargetKey, originalTarget, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
                objc_setAssociatedObject(button, kOriginalActionKey, [NSValue valueWithPointer:originalAction], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
            }
            
            [button setTarget:self];
            [button setAction:@selector(handleClick:)];
            
            _usesButtonTargetAction = YES;
        } else {
            _clickRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClick:)];
            [_childView addGestureRecognizer:_clickRecognizer];
        }
        
        if (params.href) {
            _href = [NSString stringWithUTF8String:params.href];
        } else {
            _href = @"";
        }
        
        _callback = nullptr;
        _userData = nullptr;
        _releaseCallback = nullptr;
        
        objc_setAssociatedObject(_childView, @selector(handleClick:), self, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return self;
}

- (void)dealloc {
    // Call release callback to free userData if set
    if (_releaseCallback && _userData) {
        _releaseCallback(_userData);
        _userData = nullptr;
    }
    _callback = nullptr;
    _releaseCallback = nullptr;
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

- (void)setOnClick:(ObsidianLinkClickCallback)callback 
          userData:(void*)userData 
   releaseCallback:(ObsidianLinkReleaseCallback)releaseCallback {
    if (_releaseCallback && _userData) {
        _releaseCallback(_userData);
    }
    
    _callback = callback;
    _userData = userData;
    _releaseCallback = releaseCallback;
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
    // For NSButton/NSControl: Use native enabled property
    // For other views: Disable the gesture recognizer
    if (_childView && [_childView respondsToSelector:@selector(setEnabled:)]) {
        [(id)_childView setEnabled:enabled];
    }
    if (_clickRecognizer) {
        _clickRecognizer.enabled = enabled;
    }
}

- (bool)isEnabled {
    // Check native enabled state for controls
    if (_childView && [_childView respondsToSelector:@selector(isEnabled)]) {
        return [(id)_childView isEnabled];
    }
    // For non-controls, check gesture recognizer
    if (_clickRecognizer) {
        return _clickRecognizer.enabled;
    }
    return YES; // Default to enabled if neither applies
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
    if (!_childView) return;
    
    if (_usesButtonTargetAction && [_childView isKindOfClass:[NSButton class]]) {
        NSButton* button = (NSButton*)_childView;
        id originalTarget = objc_getAssociatedObject(button, kOriginalTargetKey);
        NSValue* actionValue = objc_getAssociatedObject(button, kOriginalActionKey);
        
        if (originalTarget && actionValue) {
            [button setTarget:originalTarget];
            [button setAction:(SEL)[actionValue pointerValue]];
        } else {
            [button setTarget:nil];
            [button setAction:nil];
        }
        
        objc_setAssociatedObject(button, kOriginalTargetKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        objc_setAssociatedObject(button, kOriginalActionKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    } else if (_clickRecognizer) {
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
    
    if (_usesButtonTargetAction && [_childView isKindOfClass:[NSButton class]]) {
        NSButton* button = (NSButton*)_childView;
        id originalTarget = objc_getAssociatedObject(button, kOriginalTargetKey);
        NSValue* actionValue = objc_getAssociatedObject(button, kOriginalActionKey);
        
        if (originalTarget && actionValue) {
            SEL originalAction = (SEL)[actionValue pointerValue];
            if ([originalTarget respondsToSelector:originalAction]) {
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Warc-performSelector-leaks"
                [originalTarget performSelector:originalAction withObject:sender];
                #pragma clang diagnostic pop
            }
        }
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
                                      void* userData,
                                      ObsidianLinkReleaseCallback releaseCallback) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianLinkHandler* handler = (__bridge ObsidianLinkHandler*)handle;
        [handler setOnClick:callback userData:userData releaseCallback:releaseCallback];
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
        return (__bridge void*)handler.childView;
    }
}

} // extern "C"
