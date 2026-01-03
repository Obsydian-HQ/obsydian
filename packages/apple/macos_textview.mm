/**
 * macOS TextView FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSTextView APIs
 */

#import "macos_textview.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal text view wrapper class
@interface ObsidianTextViewWrapper : NSObject {
    NSTextView* _textView;
}

@property (nonatomic, strong) NSTextView* textView;

- (instancetype)initWithParams:(ObsidianTextViewParams)params;
- (void)setString:(const char*)text;
- (const char*)getString;
- (void)setEditable:(bool)editable;
- (bool)isEditable;
- (void)setSelectable:(bool)selectable;
- (bool)isSelectable;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianTextViewWrapper

- (instancetype)initWithParams:(ObsidianTextViewParams)params {
    self = [super init];
    if (self) {
        // Create NSTextView with frame
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        _textView = [[NSTextView alloc] initWithFrame:frame];
        
        // Configure text view defaults
        // NSTextView is editable by default, but we'll let the user control this
        // NSTextView is selectable by default, but we'll let the user control this
    }
    return self;
}

- (void)setString:(const char*)text {
    if (_textView && text) {
        NSString* textStr = [NSString stringWithUTF8String:text];
        [_textView setString:textStr];
    }
}

- (const char*)getString {
    if (_textView) {
        NSString* text = [_textView string];
        return [text UTF8String];
    }
    return nullptr;
}

- (void)setEditable:(bool)editable {
    if (_textView) {
        [_textView setEditable:editable];
    }
}

- (bool)isEditable {
    if (_textView) {
        return [_textView isEditable];
    }
    return false;
}

- (void)setSelectable:(bool)selectable {
    if (_textView) {
        [_textView setSelectable:selectable];
    }
}

- (bool)isSelectable {
    if (_textView) {
        return [_textView isSelectable];
    }
    return false;
}

- (void)setVisible:(bool)visible {
    if (_textView) {
        [_textView setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_textView) {
        return ![_textView isHidden];
    }
    return false;
}

- (void)setEnabled:(bool)enabled {
    if (_textView) {
        [_textView setEditable:enabled];
        // Note: NSTextView doesn't have a direct setEnabled method
        // We use setEditable as a proxy for enabled/disabled state
        // If you want to disable editing but keep it enabled for selection, 
        // you'd need to manage editable and selectable separately
    }
}

- (bool)isEnabled {
    if (_textView) {
        // NSTextView doesn't have isEnabled, so we check if it's editable
        // This is a reasonable proxy for "enabled" state
        return [_textView isEditable];
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_textView || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_textView];
    }
}

- (void)removeFromParent {
    if (!_textView) return;
    
    // Remove from superview
    if ([_textView superview]) {
        [_textView removeFromSuperview];
    }
}

- (NSTextView*)textView {
    return _textView;
}

@end

// C interface implementation
extern "C" {

ObsidianTextViewHandle obsidian_macos_create_textview(ObsidianTextViewParams params) {
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = [[ObsidianTextViewWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.textView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_textview_set_string(ObsidianTextViewHandle handle, const char* text) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setString:text];
    }
}

const char* obsidian_macos_textview_get_string(ObsidianTextViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper getString];
    }
}

void obsidian_macos_textview_set_editable(ObsidianTextViewHandle handle, bool editable) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setEditable:editable];
    }
}

bool obsidian_macos_textview_is_editable(ObsidianTextViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper isEditable];
    }
}

void obsidian_macos_textview_set_selectable(ObsidianTextViewHandle handle, bool selectable) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setSelectable:selectable];
    }
}

bool obsidian_macos_textview_is_selectable(ObsidianTextViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper isSelectable];
    }
}

void obsidian_macos_textview_set_visible(ObsidianTextViewHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_textview_is_visible(ObsidianTextViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_textview_set_enabled(ObsidianTextViewHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setEnabled:enabled];
    }
}

bool obsidian_macos_textview_is_enabled(ObsidianTextViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper isEnabled];
    }
}

void obsidian_macos_textview_add_to_window(ObsidianTextViewHandle textViewHandle,
                                            void* windowHandle) {
    if (!textViewHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* textViewWrapper = (__bridge ObsidianTextViewWrapper*)textViewHandle;
        [textViewWrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_textview_remove_from_parent(ObsidianTextViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_textview(ObsidianTextViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge_transfer ObsidianTextViewWrapper*)handle;
        
        // Remove from parent first
        [wrapper removeFromParent];
        
        // Clear the text view reference
        wrapper.textView = nil;
        
        // ARC will handle cleanup of the wrapper when it goes out of scope
    }
}

bool obsidian_macos_textview_is_valid(ObsidianTextViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return wrapper != nil && wrapper.textView != nil;
    }
}

void* obsidian_macos_textview_get_view_handle(ObsidianTextViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        if (wrapper && wrapper.textView) {
            // Return the NSTextView as an opaque handle
            // This is a borrowed reference, caller should not release it
            return (__bridge void*)wrapper.textView;
        }
    }
    return nullptr;
}

} // extern "C"

