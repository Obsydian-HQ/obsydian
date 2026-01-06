/**
 * macOS TextView FFI - Objective-C++ Implementation
 * 
 * Frame-based text view component
 * NO AUTO LAYOUT - Pure frame-based positioning
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
- (void)setFontSize:(double)size;
- (double)fontSize;
- (void)setFontWeightValue:(int)weight;
- (int)fontWeightValue;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianTextViewWrapper

- (instancetype)initWithParams:(ObsidianTextViewParams)params {
    self = [super init];
    if (self) {
        // Create NSTextView with initial frame
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        if (frame.size.width == 0) frame.size.width = 100;
        if (frame.size.height == 0) frame.size.height = 24;
        
        _textView = [[NSTextView alloc] initWithFrame:frame];
        
        // FRAME-BASED: Leave translatesAutoresizingMaskIntoConstraints = YES (default)
        // The text view will be positioned by its parent container via setFrame
        
        // Configure for use as a label-like view by default
        _textView.drawsBackground = NO;
        _textView.editable = NO;
        _textView.selectable = YES;
        
        // Allow the text to expand to fit
        _textView.textContainer.widthTracksTextView = NO;
        _textView.textContainer.containerSize = NSMakeSize(FLT_MAX, FLT_MAX);
        
        // NO autoresizingMask - layout engine sets frames directly
    }
    return self;
}

- (void)setString:(const char*)text {
    if (_textView && text) {
        NSString* textStr = [NSString stringWithUTF8String:text];
        [_textView setString:textStr];
        
        // Resize to fit text content
        [_textView sizeToFit];
    }
}

- (void)setFontSize:(double)size {
    if (_textView && size > 0) {
        NSFont* currentFont = [_textView font];
        if (!currentFont) {
            currentFont = [NSFont systemFontOfSize:size];
        } else {
            currentFont = [NSFont fontWithName:currentFont.fontName size:size];
        }
        if (currentFont) {
            [_textView setFont:currentFont];
        }
    }
}

- (double)fontSize {
    if (_textView) {
        NSFont* font = [_textView font];
        if (font) {
            return font.pointSize;
        }
    }
    return 0.0;
}

- (void)setFontWeightValue:(int)weight {
    if (_textView) {
        double size = [self fontSize];
        if (size <= 0) {
            size = 13.0;
        }
        
        NSFont* font = nil;
        switch (weight) {
            case 1: // Bold
                font = [NSFont boldSystemFontOfSize:size];
                break;
            case 2: // Semibold
                font = [NSFont systemFontOfSize:size weight:NSFontWeightSemibold];
                break;
            case 3: // Medium
                font = [NSFont systemFontOfSize:size weight:NSFontWeightMedium];
                break;
            case 4: // Light
                font = [NSFont systemFontOfSize:size weight:NSFontWeightLight];
                break;
            case 5: // Thin
                font = [NSFont systemFontOfSize:size weight:NSFontWeightThin];
                break;
            case 0: // Regular
            default:
                font = [NSFont systemFontOfSize:size];
                break;
        }
        
        if (font) {
            [_textView setFont:font];
        }
    }
}

- (int)fontWeightValue {
    if (_textView) {
        NSFont* font = [_textView font];
        if (font) {
            NSFontDescriptor* descriptor = font.fontDescriptor;
            NSDictionary* traits = [descriptor objectForKey:NSFontTraitsAttribute];
            NSNumber* weightNumber = [traits objectForKey:NSFontWeightTrait];
            if (weightNumber) {
                CGFloat weight = [weightNumber floatValue];
                if (weight >= 0.6) return 1; // Bold
                if (weight >= 0.4) return 2; // Semibold
                if (weight >= 0.23) return 3; // Medium
                if (weight >= 0.0) return 0; // Regular
                return 0;
            }
        }
    }
    return 0;
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

- (void)setEnabled:(bool)enabled {
    if (_textView) {
        [_textView setEditable:enabled];
    }
}

- (bool)isEnabled {
    if (_textView) {
        return [_textView isEditable];
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

- (void)addToWindow:(void*)windowHandle {
    if (!_textView || !windowHandle) {
        return;
    }
    
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_textView];
    }
}

- (void)removeFromParent {
    if (!_textView) return;
    
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
        [wrapper removeFromParent];
        wrapper.textView = nil;
    }
}

void obsidian_macos_release_textview_handle(ObsidianTextViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianTextViewWrapper*)handle;
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
            return (__bridge void*)wrapper.textView;
        }
    }
    return nullptr;
}

void obsidian_macos_textview_set_font_size(ObsidianTextViewHandle handle, double size) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setFontSize:size];
    }
}

double obsidian_macos_textview_get_font_size(ObsidianTextViewHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return [wrapper fontSize];
    }
}

void obsidian_macos_textview_set_font_weight(ObsidianTextViewHandle handle, ObsidianFontWeight weight) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        [wrapper setFontWeightValue:(int)weight];
    }
}

ObsidianFontWeight obsidian_macos_textview_get_font_weight(ObsidianTextViewHandle handle) {
    if (!handle) return ObsidianFontWeightRegular;
    
    @autoreleasepool {
        ObsidianTextViewWrapper* wrapper = (__bridge ObsidianTextViewWrapper*)handle;
        return (ObsidianFontWeight)[wrapper fontWeightValue];
    }
}

} // extern "C"
