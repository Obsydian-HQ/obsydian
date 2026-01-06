/**
 * macOS Screen Container Implementation
 * 
 * Based on react-native-screens' RNSScreenContainerView.
 * Container owns all screens and manages their lifecycle.
 * 
 * FRAME-BASED LAYOUT - NO AUTO LAYOUT
 */

#import "macos_screen_container.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>

#pragma mark - ObsidianScreenView

@interface ObsidianScreenView : NSView
@property (nonatomic, copy) NSString* routePath;
@property (nonatomic, assign) BOOL isActive;
@end

@implementation ObsidianScreenView

- (instancetype)initWithRoutePath:(NSString*)routePath {
    self = [super initWithFrame:NSZeroRect];
    if (self) {
        _routePath = [routePath copy];
        _isActive = NO;
        // FRAME-BASED: Use autoresizing to fill parent
        self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        [self setWantsLayer:YES];
    }
    return self;
}

- (void)clearContent {
    NSArray* subviews = [self.subviews copy];
    for (NSView* subview in subviews) {
        [subview removeFromSuperview];
    }
}

// FRAME-BASED: Set children to fill bounds on layout
- (void)layout {
    [super layout];
    
    for (NSView* subview in self.subviews) {
        subview.frame = self.bounds;
    }
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldSize {
    [super resizeSubviewsWithOldSize:oldSize];
    [self setNeedsLayout:YES];
}

@end

#pragma mark - ObsidianScreenContainerView

@interface ObsidianScreenContainerView : NSView
@property (nonatomic, strong) NSMutableDictionary<NSString*, ObsidianScreenView*>* screens;
@property (nonatomic, weak) ObsidianScreenView* activeScreen;
@end

@implementation ObsidianScreenContainerView

- (instancetype)init {
    self = [super initWithFrame:NSZeroRect];
    if (self) {
        _screens = [[NSMutableDictionary alloc] init];
        _activeScreen = nil;
        // FRAME-BASED: Use autoresizing to fill parent
        self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        [self setWantsLayer:YES];
    }
    return self;
}

- (ObsidianScreenView*)createScreenForPath:(NSString*)routePath {
    // Check if screen already exists
    ObsidianScreenView* existing = _screens[routePath];
    if (existing) {
        return existing;
    }
    
    // Create new screen with frame filling container
    ObsidianScreenView* screen = [[ObsidianScreenView alloc] initWithRoutePath:routePath];
    screen.frame = self.bounds;
    _screens[routePath] = screen;
    
    // Add as subview but hidden
    [self addSubview:screen];
    [screen setHidden:YES];
    
    return screen;
}

- (ObsidianScreenView*)screenForPath:(NSString*)routePath {
    return _screens[routePath];
}

- (void)setActiveScreen:(ObsidianScreenView*)screen {
    if (_activeScreen == screen) {
        return;
    }
    
    // Hide previous active screen
    if (_activeScreen) {
        [_activeScreen setHidden:YES];
        _activeScreen.isActive = NO;
    }
    
    // Show new active screen
    _activeScreen = screen;
    if (screen) {
        screen.frame = self.bounds;
        [screen setHidden:NO];
        screen.isActive = YES;
        [self setNeedsLayout:YES];
    }
}

- (void)removeScreen:(ObsidianScreenView*)screen {
    if (!screen) return;
    
    NSString* pathToRemove = nil;
    for (NSString* path in _screens) {
        if (_screens[path] == screen) {
            pathToRemove = path;
            break;
        }
    }
    
    if (pathToRemove) {
        if (_activeScreen == screen) {
            _activeScreen = nil;
        }
        [screen removeFromSuperview];
        [_screens removeObjectForKey:pathToRemove];
    }
}

- (void)clearAll {
    for (ObsidianScreenView* screen in _screens.allValues) {
        [screen removeFromSuperview];
    }
    [_screens removeAllObjects];
    _activeScreen = nil;
}

// FRAME-BASED: Position all screens to fill container
- (void)layout {
    [super layout];
    
    NSRect bounds = self.bounds;
    for (ObsidianScreenView* screen in _screens.allValues) {
        screen.frame = bounds;
    }
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldSize {
    [super resizeSubviewsWithOldSize:oldSize];
    [self setNeedsLayout:YES];
}

@end

#pragma mark - C Interface

extern "C" {

ObsidianScreenContainerHandle obsidian_macos_create_screen_container(ObsidianScreenContainerParams params) {
    (void)params;
    @autoreleasepool {
        ObsidianScreenContainerView* container = [[ObsidianScreenContainerView alloc] init];
        return (__bridge_retained void*)container;
    }
}

void obsidian_macos_destroy_screen_container(ObsidianScreenContainerHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge_transfer ObsidianScreenContainerView*)handle;
        [container clearAll];
        [container removeFromSuperview];
    }
}

void obsidian_macos_screen_container_attach_to_window(ObsidianScreenContainerHandle handle, void* windowHandle) {
    if (!handle || !windowHandle) return;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)handle;
        void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
        if (contentViewPtr) {
            NSView* contentView = (__bridge NSView*)contentViewPtr;
            
            // FRAME-BASED: Set frame to fill content view
            container.frame = contentView.bounds;
            [contentView addSubview:container];
            
            // Trigger layout
            [container setNeedsLayout:YES];
        }
    }
}

void* obsidian_macos_screen_container_get_view(ObsidianScreenContainerHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)handle;
        return (__bridge void*)container;
    }
}

ObsidianScreenHandle obsidian_macos_screen_container_create_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenParams params) {
    if (!containerHandle) return nullptr;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)containerHandle;
        NSString* routePath = params.routePath ? [NSString stringWithUTF8String:params.routePath] : @"/";
        ObsidianScreenView* screen = [container createScreenForPath:routePath];
        return (__bridge void*)screen;
    }
}

ObsidianScreenHandle obsidian_macos_screen_container_get_screen(
    ObsidianScreenContainerHandle containerHandle,
    const char* routePath) {
    if (!containerHandle || !routePath) return nullptr;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)containerHandle;
        NSString* path = [NSString stringWithUTF8String:routePath];
        ObsidianScreenView* screen = [container screenForPath:path];
        return screen ? (__bridge void*)screen : nullptr;
    }
}

void obsidian_macos_screen_container_set_active_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenHandle screenHandle) {
    if (!containerHandle) return;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)containerHandle;
        ObsidianScreenView* screen = screenHandle ? (__bridge ObsidianScreenView*)screenHandle : nil;
        [container setActiveScreen:screen];
        
        // Force layout
        [container layoutSubtreeIfNeeded];
    }
}

void obsidian_macos_screen_container_remove_screen(
    ObsidianScreenContainerHandle containerHandle,
    ObsidianScreenHandle screenHandle) {
    if (!containerHandle || !screenHandle) return;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)containerHandle;
        ObsidianScreenView* screen = (__bridge ObsidianScreenView*)screenHandle;
        [container removeScreen:screen];
    }
}

void obsidian_macos_screen_container_clear_all(ObsidianScreenContainerHandle containerHandle) {
    if (!containerHandle) return;
    @autoreleasepool {
        ObsidianScreenContainerView* container = (__bridge ObsidianScreenContainerView*)containerHandle;
        [container clearAll];
    }
}

void* obsidian_macos_screen_get_content_view(ObsidianScreenHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianScreenView* screen = (__bridge ObsidianScreenView*)handle;
        return (__bridge void*)screen;
    }
}

void obsidian_macos_screen_clear_content(ObsidianScreenHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianScreenView* screen = (__bridge ObsidianScreenView*)handle;
        [screen clearContent];
    }
}

void obsidian_macos_screen_add_subview(ObsidianScreenHandle handle, void* viewHandle) {
    if (!handle || !viewHandle) return;
    @autoreleasepool {
        ObsidianScreenView* screen = (__bridge ObsidianScreenView*)handle;
        NSView* view = (__bridge NSView*)viewHandle;
        
        // FRAME-BASED: Set view frame to fill screen
        view.frame = screen.bounds;
        [screen addSubview:view];
        [screen setNeedsLayout:YES];
    }
}

const char* obsidian_macos_screen_get_route_path(ObsidianScreenHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianScreenView* screen = (__bridge ObsidianScreenView*)handle;
        return [screen.routePath UTF8String];
    }
}

} // extern "C"
