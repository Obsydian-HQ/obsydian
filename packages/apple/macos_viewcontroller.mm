#import "macos_viewcontroller.h"
#import <AppKit/AppKit.h>

@interface ObsidianViewControllerWrapper : NSObject
@property (nonatomic, strong) NSViewController* viewController;
@property (nonatomic, strong) NSView* container;
@property (nonatomic, weak) NSView* content;
@end

@implementation ObsidianViewControllerWrapper

- (instancetype)init {
    self = [super init];
    if (self) {
        _viewController = [[NSViewController alloc] init];
        _container = [[NSView alloc] init];
        [_container setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [_viewController setView:_container];
    }
    return self;
}

- (void)setView:(void*)viewHandle {
    if (!viewHandle) return;
    
    NSView* view = (__bridge NSView*)viewHandle;
    
    if (_content) {
        [_content removeFromSuperview];
    }
    
    _content = view;
    [view setFrame:_container.bounds];
    [view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [_container addSubview:view];
}

- (void)configureForSidebar {
    if (!_content) return;
    
    [self makeTransparent:_content];
    
    NSVisualEffectView* vibrancy = [[NSVisualEffectView alloc] initWithFrame:_container.bounds];
    [vibrancy setMaterial:NSVisualEffectMaterialSidebar];
    [vibrancy setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [vibrancy setState:NSVisualEffectStateFollowsWindowActiveState];
    [vibrancy setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    
    [_content removeFromSuperview];
    [_content setFrame:vibrancy.bounds];
    [vibrancy addSubview:_content];
    [_container addSubview:vibrancy];
}

- (void)makeTransparent:(NSView*)view {
    if ([view isKindOfClass:[NSScrollView class]]) {
        NSScrollView* sv = (NSScrollView*)view;
        [sv setDrawsBackground:NO];
        [sv setBorderType:NSNoBorder];
        [[sv contentView] setDrawsBackground:NO];
        [self makeTransparent:[sv documentView]];
    }
    if ([view isKindOfClass:[NSTableView class]]) {
        NSTableView* tv = (NSTableView*)view;
        [tv setBackgroundColor:[NSColor clearColor]];
        [tv setUsesAlternatingRowBackgroundColors:NO];
        [tv setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleSourceList];
        [tv setFocusRingType:NSFocusRingTypeNone];
    }
    for (NSView* sub in [view subviews]) {
        [self makeTransparent:sub];
    }
}

@end

extern "C" {

ObsidianViewControllerHandle obsidian_macos_create_viewcontroller(ObsidianViewControllerParams /* params */) {
    @autoreleasepool {
        ObsidianViewControllerWrapper* w = [[ObsidianViewControllerWrapper alloc] init];
        return w ? (__bridge_retained void*)w : nullptr;
    }
}

void obsidian_macos_viewcontroller_set_view(ObsidianViewControllerHandle handle, void* viewHandle) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianViewControllerWrapper*)handle setView:viewHandle];
    }
}

void* obsidian_macos_viewcontroller_get_view(ObsidianViewControllerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)((__bridge ObsidianViewControllerWrapper*)handle).container;
    }
}

void* obsidian_macos_viewcontroller_get_viewcontroller(ObsidianViewControllerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return (__bridge void*)((__bridge ObsidianViewControllerWrapper*)handle).viewController;
    }
}

bool obsidian_macos_viewcontroller_is_valid(ObsidianViewControllerHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return ((__bridge ObsidianViewControllerWrapper*)handle).viewController != nil;
    }
}

void obsidian_macos_viewcontroller_configure_for_sidebar(ObsidianViewControllerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianViewControllerWrapper*)handle configureForSidebar];
    }
}

void obsidian_macos_viewcontroller_set_preferred_content_size(ObsidianViewControllerHandle handle, double width, double height) {
    if (!handle) return;
    @autoreleasepool {
        ((__bridge ObsidianViewControllerWrapper*)handle).viewController.preferredContentSize = NSMakeSize(width, height);
    }
}

void obsidian_macos_viewcontroller_get_preferred_content_size(ObsidianViewControllerHandle handle, double* outWidth, double* outHeight) {
    if (!handle) return;
    @autoreleasepool {
        NSSize size = ((__bridge ObsidianViewControllerWrapper*)handle).viewController.preferredContentSize;
        if (outWidth) *outWidth = size.width;
        if (outHeight) *outHeight = size.height;
    }
}

void obsidian_macos_destroy_viewcontroller(ObsidianViewControllerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianViewControllerWrapper*)handle;
    }
}

}
