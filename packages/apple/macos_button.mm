/**
 * macOS Button FFI - Objective-C++ Implementation
 * 
 * REFACTORED: No wrapper object. The NSButton IS the component.
 * Following React Native's pattern where UIView IS the component view.
 * 
 * Callbacks are stored using associated objects on the NSButton itself.
 */

#import "macos_button.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

// Associated object keys for storing callback data
static const void* kCallbackKey = &kCallbackKey;
static const void* kUserDataKey = &kUserDataKey;
static const void* kTargetKey = &kTargetKey;

// Target object for handling button clicks (stored as associated object)
@interface ObsidianButtonTarget : NSObject
@property (nonatomic, assign) ObsidianButtonClickCallback callback;
@property (nonatomic, assign) void* userData;
- (void)handleClick:(id)sender;
@end

@implementation ObsidianButtonTarget
- (void)handleClick:(id)sender {
    (void)sender;
    if (self.callback) {
        self.callback(self.userData);
    }
}
@end

// Helper to get/create target for a button
static ObsidianButtonTarget* getOrCreateTarget(NSButton* button) {
    ObsidianButtonTarget* target = objc_getAssociatedObject(button, kTargetKey);
    if (!target) {
        target = [[ObsidianButtonTarget alloc] init];
        objc_setAssociatedObject(button, kTargetKey, target, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        [button setTarget:target];
        [button setAction:@selector(handleClick:)];
    }
    return target;
}

// C interface implementation
extern "C" {

/**
 * Create a button. Returns NSButton* directly (NO WRAPPER).
 */
ObsidianButtonHandle obsidian_macos_create_button(ObsidianButtonParams params) {
    @autoreleasepool {
        NSButton* button = [[NSButton alloc] init];
        
        // Set button style
        [button setButtonType:NSButtonTypeMomentaryPushIn];
        [button setBezelStyle:NSBezelStyleRounded];
        
        // Set initial frame
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        [button setFrame:frame];
        
        // Size to fit content if no explicit size given
        if (params.width == 0 || params.height == 0) {
            [button sizeToFit];
        }
        
        // Set title if provided
        if (params.title) {
            NSString* titleStr = [NSString stringWithUTF8String:params.title];
            [button setTitle:titleStr];
            [button sizeToFit];
        }
        
        // Return NSButton directly - it IS the handle
        return (__bridge_retained void*)button;
    }
}

void obsidian_macos_button_set_title(ObsidianButtonHandle handle, const char* title) {
    if (!handle || !title) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        NSString* titleStr = [NSString stringWithUTF8String:title];
        [button setTitle:titleStr];
        [button sizeToFit];
    }
}

const char* obsidian_macos_button_get_title(ObsidianButtonHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        return [[button title] UTF8String];
    }
}

void obsidian_macos_button_set_on_click(ObsidianButtonHandle handle,
                                        ObsidianButtonClickCallback callback,
                                        void* userData) {
    if (!handle) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        ObsidianButtonTarget* target = getOrCreateTarget(button);
        target.callback = callback;
        target.userData = userData;
    }
}

void obsidian_macos_button_set_visible(ObsidianButtonHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        [button setHidden:!visible];
    }
}

bool obsidian_macos_button_is_visible(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        return ![button isHidden];
    }
}

void obsidian_macos_button_set_enabled(ObsidianButtonHandle handle, bool enabled) {
    if (!handle) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        [button setEnabled:enabled];
    }
}

bool obsidian_macos_button_is_enabled(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        return [button isEnabled];
    }
}

void obsidian_macos_button_add_to_window(ObsidianButtonHandle buttonHandle,
                                        void* windowHandle) {
    if (!buttonHandle || !windowHandle) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)buttonHandle;
        void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
        if (contentViewPtr) {
            NSView* contentView = (__bridge NSView*)contentViewPtr;
            [contentView addSubview:button];
        }
    }
}

void obsidian_macos_button_remove_from_parent(ObsidianButtonHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        if ([button superview]) {
            [button removeFromSuperview];
        }
    }
}

void obsidian_macos_destroy_button(ObsidianButtonHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Transfer ownership back and let ARC release
        NSButton* button = (__bridge_transfer NSButton*)handle;
        
        // Clear target/action
        [button setTarget:nil];
        [button setAction:nil];
        
        // Clear associated objects
        objc_setAssociatedObject(button, kTargetKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        
        // Remove from superview
        if ([button superview]) {
            [button removeFromSuperview];
        }
        
        // button will be released by ARC when this scope ends
    }
}

bool obsidian_macos_button_is_valid(ObsidianButtonHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        NSButton* button = (__bridge NSButton*)handle;
        return button != nil;
    }
}

/**
 * Get the NSButton view. Since handle IS the NSButton, just return it.
 */
void* obsidian_macos_button_get_view(ObsidianButtonHandle handle) {
    // Handle IS the NSButton - no wrapper indirection
    return handle;
}

} // extern "C"
