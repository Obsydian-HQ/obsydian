/**
 * macOS ViewController FFI - Objective-C++ Implementation
 */

#import "macos_viewcontroller.h"
#import <AppKit/AppKit.h>

@interface ObsidianViewControllerWrapper : NSObject
@property (nonatomic, strong) NSViewController* viewController;
@property (nonatomic, strong) NSVisualEffectView* visualEffectView;
@property (nonatomic, strong) NSView* contentView;
@property (nonatomic, assign) BOOL configuredForSidebar;
@end

@implementation ObsidianViewControllerWrapper

- (instancetype)init {
    self = [super init];
    if (self) {
        _viewController = [[NSViewController alloc] init];
        _configuredForSidebar = NO;
    }
    return self;
}

- (void)setView:(void*)viewHandle {
    if (!_viewController || !viewHandle) return;
    
    NSView* view = (__bridge NSView*)viewHandle;
    if ([view superview]) {
        [view removeFromSuperview];
    }
    [_viewController setView:view];
}

- (void*)getView {
    return _viewController ? (__bridge void*)[_viewController view] : nullptr;
}

- (void)configureForSidebar {
    if (!_viewController || _configuredForSidebar) return;
    
    NSView* currentView = [_viewController view];
    if (!currentView) return;
    
    _contentView = currentView;
    [self makeViewTransparentForSidebar:_contentView];
    
    // Create vibrancy view with sidebar material
    _visualEffectView = [[NSVisualEffectView alloc] init];
    [_visualEffectView setMaterial:NSVisualEffectMaterialSidebar];
    [_visualEffectView setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [_visualEffectView setState:NSVisualEffectStateFollowsWindowActiveState];
    
    [_contentView removeFromSuperview];
    [_visualEffectView addSubview:_contentView];
    
    // Use constraints to fill
    [_contentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [NSLayoutConstraint activateConstraints:@[
        [_contentView.leadingAnchor constraintEqualToAnchor:_visualEffectView.leadingAnchor],
        [_contentView.trailingAnchor constraintEqualToAnchor:_visualEffectView.trailingAnchor],
        [_contentView.topAnchor constraintEqualToAnchor:_visualEffectView.topAnchor],
        [_contentView.bottomAnchor constraintEqualToAnchor:_visualEffectView.bottomAnchor]
    ]];
    
    [_viewController setView:_visualEffectView];
    _configuredForSidebar = YES;
}

- (void)makeViewTransparentForSidebar:(NSView*)view {
    if (!view) return;
    
    if ([view isKindOfClass:[NSScrollView class]]) {
        NSScrollView* scrollView = (NSScrollView*)view;
        [scrollView setDrawsBackground:NO];
        [scrollView setBorderType:NSNoBorder];
        [[scrollView contentView] setDrawsBackground:NO];
        [self makeViewTransparentForSidebar:[scrollView documentView]];
    }
    
    if ([view isKindOfClass:[NSTableView class]]) {
        NSTableView* tableView = (NSTableView*)view;
        [tableView setBackgroundColor:[NSColor clearColor]];
        [tableView setUsesAlternatingRowBackgroundColors:NO];
        [tableView setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleSourceList];
        [tableView setFocusRingType:NSFocusRingTypeNone];
    }
    
    for (NSView* subview in [view subviews]) {
        [self makeViewTransparentForSidebar:subview];
    }
}

@end

// C interface
extern "C" {

ObsidianViewControllerHandle obsidian_macos_create_viewcontroller(ObsidianViewControllerParams params) {
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = [[ObsidianViewControllerWrapper alloc] init];
        return wrapper.viewController ? (__bridge_retained void*)wrapper : nullptr;
    }
}

void obsidian_macos_viewcontroller_set_view(ObsidianViewControllerHandle handle, void* viewHandle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)handle;
        [wrapper setView:viewHandle];
    }
}

void* obsidian_macos_viewcontroller_get_view(ObsidianViewControllerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        return [(__bridge ObsidianViewControllerWrapper*)handle getView];
    }
}

void* obsidian_macos_viewcontroller_get_viewcontroller(ObsidianViewControllerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)handle;
        return wrapper.viewController ? (__bridge void*)wrapper.viewController : nullptr;
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

void obsidian_macos_viewcontroller_set_preferred_content_size(ObsidianViewControllerHandle handle,
                                                               double width, double height) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)handle;
        if (wrapper.viewController) {
            [wrapper.viewController setPreferredContentSize:NSMakeSize(width, height)];
        }
    }
}

void obsidian_macos_viewcontroller_get_preferred_content_size(ObsidianViewControllerHandle handle,
                                                               double* outWidth, double* outHeight) {
    if (!handle || !outWidth || !outHeight) return;
    @autoreleasepool {
        ObsidianViewControllerWrapper* wrapper = (__bridge ObsidianViewControllerWrapper*)handle;
        if (wrapper.viewController) {
            NSSize size = [wrapper.viewController preferredContentSize];
            *outWidth = size.width;
            *outHeight = size.height;
        } else {
            *outWidth = *outHeight = 0;
        }
    }
}

void obsidian_macos_destroy_viewcontroller(ObsidianViewControllerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianViewControllerWrapper*)handle;
    }
}

} // extern "C"
