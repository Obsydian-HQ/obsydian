/**
 * macOS Button FFI - Objective-C++ Implementation
 * 
 * Frame-based button component
 * NO AUTO LAYOUT - Pure frame-based positioning
 */

#import "macos_button.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal button wrapper class
@interface ObsidianButtonWrapper : NSObject {
    NSButton* _button;
    ObsidianButtonClickCallback _callback;
    void* _userData;
}

@property (nonatomic, strong) NSButton* button;
@property (nonatomic, assign) ObsidianButtonClickCallback callback;
@property (nonatomic, assign) void* userData;

- (instancetype)initWithParams:(ObsidianButtonParams)params;
- (void)setTitle:(const char*)title;
- (const char*)getTitle;
- (void)setOnClick:(ObsidianButtonClickCallback)callback userData:(void*)userData;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;
- (void)handleClick:(id)sender;

@end

@implementation ObsidianButtonWrapper

- (instancetype)initWithParams:(ObsidianButtonParams)params {
    self = [super init];
    if (self) {
        // Create NSButton
        _button = [[NSButton alloc] init];
        
        // FRAME-BASED: Leave translatesAutoresizingMaskIntoConstraints = YES (default)
        // The button will be positioned by its parent container via setFrame
        
        // Set button style
        [_button setButtonType:NSButtonTypeMomentaryPushIn];
        [_button setBezelStyle:NSBezelStyleRounded];
        
        // Set initial frame
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        [_button setFrame:frame];
        
        // Size to fit content if no explicit size given
        if (params.width == 0 || params.height == 0) {
            [_button sizeToFit];
        }
        
        // Set title if provided
        if (params.title) {
            NSString* titleStr = [NSString stringWithUTF8String:params.title];
            [_button setTitle:titleStr];
            // Re-size to fit the new title
            [_button sizeToFit];
        }
        
        // Set target-action for click events
        [_button setTarget:self];
        [_button setAction:@selector(handleClick:)];
        
        // Retain the wrapper using associated object
        objc_setAssociatedObject(_button, @selector(handleClick:), self, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        _callback = nullptr;
        _userData = nullptr;
    }
    return self;
}

- (void)setTitle:(const char*)title {
    if (_button && title) {
        NSString* titleStr = [NSString stringWithUTF8String:title];
        [_button setTitle:titleStr];
        [_button sizeToFit];
    }
}

- (const char*)getTitle {
    if (_button) {
        NSString* title = [_button title];
        return [title UTF8String];
    }
    return nullptr;
}

- (void)setOnClick:(ObsidianButtonClickCallback)callback userData:(void*)userData {
    _callback = callback;
    _userData = userData;
}

- (void)setVisible:(bool)visible {
    if (_button) {
        [_button setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_button) {
        return ![_button isHidden];
    }
    return false;
}

- (void)setEnabled:(bool)enabled {
    if (_button) {
        [_button setEnabled:enabled];
    }
}

- (bool)isEnabled {
    if (_button) {
        return [_button isEnabled];
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_button || !windowHandle) {
        return;
    }
    
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_button];
    }
}

- (void)removeFromParent {
    if (!_button) return;
    
    [_button setTarget:nil];
    [_button setAction:nil];
    objc_setAssociatedObject(_button, @selector(handleClick:), nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    if ([_button superview]) {
        [_button removeFromSuperview];
    }
}

- (void)handleClick:(id)sender {
    if (_callback) {
        _callback(_userData);
    }
}

- (NSButton*)button {
    return _button;
}

- (ObsidianButtonClickCallback)callback {
    return _callback;
}

- (void*)userData {
    return _userData;
}

@end

// C interface implementation
extern "C" {

ObsidianButtonHandle obsidian_macos_create_button(ObsidianButtonParams params) {
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = [[ObsidianButtonWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.button) {
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_button_set_title(ObsidianButtonHandle handle, const char* title) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        [wrapper setTitle:title];
    }
}

const char* obsidian_macos_button_get_title(ObsidianButtonHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        return [wrapper getTitle];
    }
}

void obsidian_macos_button_set_on_click(ObsidianButtonHandle handle,
                                        ObsidianButtonClickCallback callback,
                                        void* userData) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        [wrapper setOnClick:callback userData:userData];
    }
}

void obsidian_macos_button_set_visible(ObsidianButtonHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_button_is_visible(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_button_set_enabled(ObsidianButtonHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        [wrapper setEnabled:enabled];
    }
}

bool obsidian_macos_button_is_enabled(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        return [wrapper isEnabled];
    }
}

void obsidian_macos_button_add_to_window(ObsidianButtonHandle buttonHandle,
                                        void* windowHandle) {
    if (!buttonHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* buttonWrapper = (__bridge ObsidianButtonWrapper*)buttonHandle;
        [buttonWrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_button_remove_from_parent(ObsidianButtonHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_button(ObsidianButtonHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge_transfer ObsidianButtonWrapper*)handle;
        [wrapper removeFromParent];
        wrapper.callback = nullptr;
        wrapper.userData = nullptr;
        wrapper.button = nil;
    }
}

bool obsidian_macos_button_is_valid(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        return wrapper != nil && wrapper.button != nil;
    }
}

void* obsidian_macos_button_get_view(ObsidianButtonHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianButtonWrapper* wrapper = (__bridge ObsidianButtonWrapper*)handle;
        if (wrapper && wrapper.button) {
            return (__bridge void*)wrapper.button;
        }
    }
    return nullptr;
}

} // extern "C"
