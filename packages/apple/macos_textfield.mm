/**
 * macOS TextField FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSTextField APIs
 */

#import "macos_textfield.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>
#import <cstring>

// Internal text field wrapper class
@interface ObsidianTextFieldWrapper : NSObject <NSTextFieldDelegate> {
    NSTextField* _textField;
    ObsidianTextFieldEnterCallback _callback;
    void* _userData;
}

@property (nonatomic, strong) NSTextField* textField;
@property (nonatomic, assign) ObsidianTextFieldEnterCallback callback;
@property (nonatomic, assign) void* userData;

- (instancetype)initWithParams:(ObsidianTextFieldParams)params;
- (void)setText:(const char*)text;
- (const char*)getText;
- (void)setPlaceholder:(const char*)placeholder;
- (void)setOnEnter:(ObsidianTextFieldEnterCallback)callback userData:(void*)userData;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;
- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView doCommandBySelector:(SEL)commandSelector;
- (void)controlTextDidEndEditing:(NSNotification*)notification;

@end

@implementation ObsidianTextFieldWrapper

- (instancetype)initWithParams:(ObsidianTextFieldParams)params {
    self = [super init];
    if (self) {
        // Create NSTextField
        _textField = [[NSTextField alloc] init];
        
        // Set frame (position and size)
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        [_textField setFrame:frame];
        
        // Set placeholder if provided
        if (params.placeholder) {
            NSString* placeholderStr = [NSString stringWithUTF8String:params.placeholder];
            [[_textField cell] setPlaceholderString:placeholderStr];
        }
        
        // Set delegate to handle Enter key events
        [_textField setDelegate:self];
        
        // Set target-action for Enter key (alternative approach)
        // NSTextField sends controlTextDidEndEditing when Enter is pressed
        // We'll use the delegate method instead
        
        // Retain the wrapper using associated object to ensure it stays alive
        // This prevents the delegate from being deallocated while the text field exists
        objc_setAssociatedObject(_textField, @selector(controlTextDidEndEditing:), self, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        _callback = nullptr;
        _userData = nullptr;
    }
    return self;
}

- (void)setText:(const char*)text {
    if (_textField && text) {
        NSString* textStr = [NSString stringWithUTF8String:text];
        [_textField setStringValue:textStr];
    }
}

- (const char*)getText {
    if (_textField) {
        // CRITICAL: NSTextField only updates stringValue when editing ends.
        // If the user is still typing or hasn't pressed Enter/Tab, the text
        // is in the field editor (NSTextView), not in stringValue.
        // 
        // We need to check if there's a current editor (field is being edited)
        // and get the text from there, otherwise use stringValue.
        NSWindow* window = [_textField window];
        NSTextView* fieldEditor = nil;
        
        if (window) {
            // Get the current field editor if the field is being edited
            fieldEditor = (NSTextView*)[window fieldEditor:YES forObject:_textField];
        }
        
        NSString* text = nil;
        
        if (fieldEditor && [fieldEditor isKindOfClass:[NSTextView class]]) {
            text = [fieldEditor string];
        } else {
            [_textField validateEditing];
            text = [_textField stringValue];
        }
        
        if (text) {
            return [text UTF8String];
        }
    }
    return nullptr;
}

- (void)setPlaceholder:(const char*)placeholder {
    if (_textField && placeholder) {
        NSString* placeholderStr = [NSString stringWithUTF8String:placeholder];
        [[_textField cell] setPlaceholderString:placeholderStr];
    }
}

- (void)setOnEnter:(ObsidianTextFieldEnterCallback)callback userData:(void*)userData {
    _callback = callback;
    _userData = userData;
}

- (void)setVisible:(bool)visible {
    if (_textField) {
        [_textField setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_textField) {
        return ![_textField isHidden];
    }
    return false;
}

- (void)setEnabled:(bool)enabled {
    if (_textField) {
        [_textField setEnabled:enabled];
    }
}

- (bool)isEnabled {
    if (_textField) {
        return [_textField isEnabled];
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_textField || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_textField];
    }
}

- (void)removeFromParent {
    if (!_textField) return;
    
    // Clear delegate before removing to prevent crashes
    [_textField setDelegate:nil];
    
    // Remove associated object BEFORE removing from superview
    // This breaks the retain cycle before the view hierarchy is modified
    objc_setAssociatedObject(_textField, @selector(controlTextDidEndEditing:), nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Remove from superview (this releases the text field from the parent's ownership)
    // The text field wrapper still retains the text field via the strong property
    if ([_textField superview]) {
        [_textField removeFromSuperview];
    }
}

- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView doCommandBySelector:(SEL)commandSelector {
    // Root Cause: NSTextField's default behavior when Enter is pressed:
    // 1. The field editor (NSTextView) automatically selects all text when editing ends
    // 2. This happens AFTER controlTextDidEndEditing is called
    // 3. This is standard macOS behavior for text fields
    
    // Solution: Intercept Enter key BEFORE default behavior using this delegate method
    // Return YES to prevent default selection behavior, then call our callback
    // The callback can handle text clearing if needed (e.g., textField3 in the example)
    if (commandSelector == @selector(insertNewline:)) {
        // Enter key was pressed
        // Call the callback first - it can handle clearing the text if needed
        if (_callback) {
            _callback(_userData);
        }
        
        // Return YES to prevent default behavior (prevents text selection)
        // This tells the field editor we handled the command, so it won't select all text
        return YES;
    }
    
    // Return NO to allow default behavior for other commands (like Tab)
    return NO;
}

- (void)controlTextDidEndEditing:(NSNotification*)notification {
    // This is called after editing ends (e.g., Tab key or focus loss)
    // We handle Enter key in control:textView:doCommandBySelector: above
    // This method is kept for potential future use or Tab key handling
    // For now, we don't need to do anything here since Enter is handled above
}

- (NSTextField*)textField {
    return _textField;
}

- (ObsidianTextFieldEnterCallback)callback {
    return _callback;
}

- (void*)userData {
    return _userData;
}

@end

// C interface implementation
extern "C" {

ObsidianTextFieldHandle obsidian_macos_create_textfield(ObsidianTextFieldParams params) {
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = [[ObsidianTextFieldWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.textField) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_textfield_set_text(ObsidianTextFieldHandle handle, const char* text) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper setText:text];
    }
}

const char* obsidian_macos_textfield_get_text(ObsidianTextFieldHandle handle) {
    if (!handle) return nullptr;
    
    // CRITICAL: We cannot return a pointer from within an autoreleasepool
    // because the NSString will be released when the pool drains, invalidating
    // the UTF8String pointer. We must copy the string.
    const char* result = nullptr;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        const char* temp = [wrapper getText];
        if (temp) {
            // Copy the string - caller is responsible for freeing (but C++ std::string will copy it immediately)
            result = strdup(temp);
        }
    }
    
    return result;
}

void obsidian_macos_textfield_set_placeholder(ObsidianTextFieldHandle handle, const char* placeholder) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper setPlaceholder:placeholder];
    }
}

void obsidian_macos_textfield_set_on_enter(ObsidianTextFieldHandle handle,
                                           ObsidianTextFieldEnterCallback callback,
                                           void* userData) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper setOnEnter:callback userData:userData];
    }
}

void obsidian_macos_textfield_set_visible(ObsidianTextFieldHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_textfield_is_visible(ObsidianTextFieldHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_textfield_set_enabled(ObsidianTextFieldHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper setEnabled:enabled];
    }
}

bool obsidian_macos_textfield_is_enabled(ObsidianTextFieldHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        return [wrapper isEnabled];
    }
}

void obsidian_macos_textfield_add_to_window(ObsidianTextFieldHandle textFieldHandle,
                                            void* windowHandle) {
    if (!textFieldHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* textFieldWrapper = (__bridge ObsidianTextFieldWrapper*)textFieldHandle;
        [textFieldWrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_textfield_remove_from_parent(ObsidianTextFieldHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_textfield(ObsidianTextFieldHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge_transfer ObsidianTextFieldWrapper*)handle;
        
        // Remove from parent first (clears delegate, removes associated object, removes from view hierarchy)
        // This must be done before clearing callbacks to ensure proper cleanup order
        [wrapper removeFromParent];
        
        // Clear callback to prevent use after free
        wrapper.callback = nullptr;
        wrapper.userData = nullptr;
        
        // Clear the text field reference to break the strong reference
        // This ensures the text field can be deallocated if it's no longer needed
        // We do this AFTER removeFromParent to ensure the text field is properly removed from hierarchy first
        wrapper.textField = nil;
        
        // ARC will handle cleanup of the wrapper when it goes out of scope
    }
}

bool obsidian_macos_textfield_is_valid(ObsidianTextFieldHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianTextFieldWrapper* wrapper = (__bridge ObsidianTextFieldWrapper*)handle;
        return wrapper != nil && wrapper.textField != nil;
    }
}

} // extern "C"

