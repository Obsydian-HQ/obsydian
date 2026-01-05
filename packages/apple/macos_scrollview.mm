/**
 * macOS ScrollView FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSScrollView APIs
 */

#import "macos_scrollview.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal scroll view wrapper class
@interface ObsidianScrollViewWrapper : NSObject {
    NSScrollView* _scrollView;
}

@property (nonatomic, strong) NSScrollView* scrollView;

- (instancetype)initWithParams:(ObsidianScrollViewParams)params;
- (void)setDocumentView:(void*)documentViewHandle;
- (void*)getDocumentView;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianScrollViewWrapper

- (instancetype)initWithParams:(ObsidianScrollViewParams)params {
    self = [super init];
    if (self) {
        // Create NSScrollView with frame
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        _scrollView = [[NSScrollView alloc] initWithFrame:frame];
        
        // Configure scroll view defaults
        // Enable automatic scrolling
        [_scrollView setHasVerticalScroller:YES];
        [_scrollView setHasHorizontalScroller:YES];
        [_scrollView setAutohidesScrollers:YES];
    }
    return self;
}

- (void)setDocumentView:(void*)documentViewHandle {
    if (!_scrollView) return;
    
    if (documentViewHandle) {
        // The documentViewHandle should be a direct NSView* handle
        // (obtained via obsidian_macos_textview_get_view_handle or similar)
        NSView* documentView = (__bridge NSView*)documentViewHandle;
        if (documentView) {
            [_scrollView setDocumentView:documentView];
        }
    } else {
        // Clear the document view
        [_scrollView setDocumentView:nil];
    }
}

- (void*)getDocumentView {
    if (!_scrollView) return nullptr;
    
    NSView* documentView = [_scrollView documentView];
    if (!documentView) return nullptr;
    
    // Return the document view as an opaque handle
    // Note: This is a borrowed reference, caller should not release it
    return (__bridge void*)documentView;
}

- (void)setVisible:(bool)visible {
    if (_scrollView) {
        [_scrollView setHidden:!visible];
    }
}

- (bool)isVisible {
    if (_scrollView) {
        return ![_scrollView isHidden];
    }
    return false;
}

- (void)addToWindow:(void*)windowHandle {
    if (!_scrollView || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_scrollView];
    }
}

- (void)removeFromParent {
    if (!_scrollView) return;
    
    // Clear document view first to break any retain cycles
    [_scrollView setDocumentView:nil];
    
    // Remove from superview
    if ([_scrollView superview]) {
        [_scrollView removeFromSuperview];
    }
}

- (NSScrollView*)scrollView {
    return _scrollView;
}

@end

// C interface implementation
extern "C" {

ObsidianScrollViewHandle obsidian_macos_create_scrollview(ObsidianScrollViewParams params) {
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = [[ObsidianScrollViewWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.scrollView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_scrollview_set_document_view(ObsidianScrollViewHandle scrollViewHandle,
                                                  void* documentViewHandle) {
    if (!scrollViewHandle) return;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* scrollViewWrapper = (__bridge ObsidianScrollViewWrapper*)scrollViewHandle;
        [scrollViewWrapper setDocumentView:documentViewHandle];
    }
}

void* obsidian_macos_scrollview_get_document_view(ObsidianScrollViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        return [wrapper getDocumentView];
    }
}

void obsidian_macos_scrollview_set_visible(ObsidianScrollViewHandle handle, bool visible) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_scrollview_is_visible(ObsidianScrollViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_scrollview_add_to_window(ObsidianScrollViewHandle scrollViewHandle,
                                              void* windowHandle) {
    if (!scrollViewHandle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* scrollViewWrapper = (__bridge ObsidianScrollViewWrapper*)scrollViewHandle;
        [scrollViewWrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_scrollview_remove_from_parent(ObsidianScrollViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_scrollview(ObsidianScrollViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge_transfer ObsidianScrollViewWrapper*)handle;
        
        // Remove from parent first (clears document view, removes from view hierarchy)
        [wrapper removeFromParent];
        
        // Clear the scroll view reference
        wrapper.scrollView = nil;
        
        // ARC will handle cleanup of the wrapper when it goes out of scope
    }
}

bool obsidian_macos_scrollview_is_valid(ObsidianScrollViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        return wrapper != nil && wrapper.scrollView != nil;
    }
}

void* obsidian_macos_scrollview_get_view(ObsidianScrollViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianScrollViewWrapper* wrapper = (__bridge ObsidianScrollViewWrapper*)handle;
        if (wrapper && wrapper.scrollView) {
            return (__bridge void*)wrapper.scrollView;
        }
    }
    return nullptr;
}

} // extern "C"

