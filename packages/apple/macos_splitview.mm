/**
 * macOS SplitView FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSSplitView APIs for sidebar layouts
 * 
 * This implementation uses NSSplitView to create a native macOS sidebar with:
 * - Collapse/expand functionality
 * - Resizable divider (drag to resize)
 * - Minimum/maximum sidebar widths
 * - Sidebar position (leading/trailing)
 */

#import "macos_splitview.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal SplitView wrapper class
@interface ObsidianSplitViewWrapper : NSObject <NSSplitViewDelegate> {
    NSSplitView* _splitView;
    NSView* _sidebarView;
    NSView* _mainContentView;
    ObsidianSidebarPosition _position;
    double _minSidebarWidth;
    double _maxSidebarWidth;
    BOOL _sidebarCollapsed;
}

@property (nonatomic, strong) NSSplitView* splitView;
// Note: sidebarView and mainContentView are managed via instance variables and custom setters
// No @property declaration to avoid type mismatch with custom setter signatures

- (instancetype)initWithParams:(ObsidianSplitViewParams)params;
- (void*)getView;
- (void)setSidebarWidth:(double)width;
- (double)getSidebarWidth;
- (void)setMinimumSidebarWidth:(double)width;
- (void)setMaximumSidebarWidth:(double)width;
- (void)collapseSidebar;
- (void)expandSidebar;
- (void)toggleSidebar;
- (bool)isSidebarCollapsed;
- (void)setSidebarView:(void*)viewHandle;
- (void)setMainContentView:(void*)viewHandle;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianSplitViewWrapper

- (instancetype)initWithParams:(ObsidianSplitViewParams)params {
    self = [super init];
    if (self) {
        _position = params.position;
        _minSidebarWidth = params.minSidebarWidth > 0 ? params.minSidebarWidth : 150.0;
        _maxSidebarWidth = params.maxSidebarWidth > 0 ? params.maxSidebarWidth : 400.0;
        _sidebarCollapsed = NO;
        
        // Create NSSplitView with zero frame (will be constrained by Auto Layout)
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _splitView = [[NSSplitView alloc] initWithFrame:frame];
        
        // Set vertical orientation for left/right split (sidebar + main content)
        // Vertical = horizontal divider (left/right panes)
        [_splitView setVertical:YES];
        
        // Set divider style (thin divider for native macOS look)
        [_splitView setDividerStyle:NSSplitViewDividerStyleThin];
        
        // Set delegate for collapse behavior
        [_splitView setDelegate:self];
        
        // Disable autoresizing mask - we'll use Auto Layout
        [_splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Create placeholder views for sidebar and main content
        // These will be replaced when setSidebarView/setMainContentView are called
        NSRect sidebarFrame = NSMakeRect(0, 0, params.initialSidebarWidth > 0 ? params.initialSidebarWidth : 200.0, 100);
        _sidebarView = [[NSView alloc] initWithFrame:sidebarFrame];
        [_sidebarView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        NSRect mainFrame = NSMakeRect(0, 0, 300, 100);
        _mainContentView = [[NSView alloc] initWithFrame:mainFrame];
        [_mainContentView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Add views to split view (order matters for position)
        // For leading position: sidebar first, then main content
        // For trailing position: main content first, then sidebar
        if (_position == ObsidianSidebarPositionLeading) {
            [_splitView addSubview:_sidebarView];
            [_splitView addSubview:_mainContentView];
        } else {
            [_splitView addSubview:_mainContentView];
            [_splitView addSubview:_sidebarView];
        }
        
        // Set minimum sizes for split view panes
        // NSSplitView uses setMinimumSize: on the views themselves
        // But we need to handle this via the delegate method splitView:constrainMinCoordinate:ofSubviewAt:
        // For now, we'll store the min/max values and use them in the delegate methods
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_splitView;
}

- (void)setSidebarWidth:(double)width {
    if (!_splitView || _sidebarCollapsed) {
        return;
    }
    
    // Clamp width to min/max values
    width = MAX(_minSidebarWidth, MIN(_maxSidebarWidth, width));
    
    // Get current split view frame
    NSRect splitFrame = [_splitView frame];
    if (splitFrame.size.width == 0 || splitFrame.size.height == 0) {
        // Split view not laid out yet, store width for later
        // The width will be applied when the split view is laid out
        return;
    }
    
    // Calculate divider position
    // For vertical split view, divider position is the x-coordinate
    double dividerPosition;
    if (_position == ObsidianSidebarPositionLeading) {
        // Sidebar is first, divider position is sidebar width
        dividerPosition = width;
    } else {
        // Sidebar is second, divider position is main content width
        dividerPosition = splitFrame.size.width - width;
    }
    
    // Set divider position
    // NSSplitView uses setPosition:ofDividerAtIndex: to set divider position
    NSInteger dividerIndex = 0;  // There's one divider between two panes
    [_splitView setPosition:dividerPosition ofDividerAtIndex:dividerIndex];
    [_splitView adjustSubviews];
}

- (double)getSidebarWidth {
    if (!_splitView || _sidebarCollapsed) {
        return 0.0;
    }
    
    NSRect splitFrame = [_splitView frame];
    if (splitFrame.size.width == 0) {
        return 0.0;
    }
    
    // Calculate sidebar width from subview frames
    // NSSplitView doesn't have positionOfDividerAtIndex:, so we calculate from subview positions
    if (_position == ObsidianSidebarPositionLeading) {
        // Sidebar is first subview, its width is the frame width
        if (_sidebarView && [_sidebarView superview] == _splitView) {
            return [_sidebarView frame].size.width;
        }
    } else {
        // Sidebar is second subview, its width is the frame width
        if (_sidebarView && [_sidebarView superview] == _splitView) {
            return [_sidebarView frame].size.width;
        }
    }
    
    return 0.0;
}

- (void)setMinimumSidebarWidth:(double)width {
    _minSidebarWidth = width > 0 ? width : 150.0;
    // Invalidate split view layout to apply new minimum
    [_splitView setNeedsLayout:YES];
}

- (void)setMaximumSidebarWidth:(double)width {
    _maxSidebarWidth = width > 0 ? width : 400.0;
    // If current width exceeds max, clamp it
    double currentWidth = [self getSidebarWidth];
    if (currentWidth > _maxSidebarWidth) {
        [self setSidebarWidth:_maxSidebarWidth];
    }
}

- (void)collapseSidebar {
    if (!_splitView || _sidebarCollapsed) {
        return;
    }
    
    // Collapse the sidebar pane
    // NSSplitView provides collapse behavior via delegate methods
    // We can use setPosition:ofDividerAtIndex: to collapse
    NSInteger dividerIndex = 0;
    
    if (_position == ObsidianSidebarPositionLeading) {
        // Collapse by moving divider to position 0
        [_splitView setPosition:0.0 ofDividerAtIndex:dividerIndex];
    } else {
        // Collapse by moving divider to the end
        NSRect splitFrame = [_splitView frame];
        [_splitView setPosition:splitFrame.size.width ofDividerAtIndex:dividerIndex];
    }
    
    _sidebarCollapsed = YES;
    [_splitView adjustSubviews];
}

- (void)expandSidebar {
    if (!_splitView || !_sidebarCollapsed) {
        return;
    }
    
    // Expand the sidebar pane to its preferred width
    // Use a default width if not specified (200 points)
    double preferredWidth = 200.0;
    [self setSidebarWidth:preferredWidth];
    
    _sidebarCollapsed = NO;
    [_splitView adjustSubviews];
}

- (void)toggleSidebar {
    if (_sidebarCollapsed) {
        [self expandSidebar];
    } else {
        [self collapseSidebar];
    }
}

- (bool)isSidebarCollapsed {
    return _sidebarCollapsed;
}

- (void)setSidebarView:(void*)viewHandle {
    if (!viewHandle) {
        return;
    }
    
    NSView* newSidebarView = (__bridge NSView*)viewHandle;
    if (!newSidebarView || newSidebarView == _sidebarView) {
        return;
    }
    
    // Remove old sidebar view if it exists
    if (_sidebarView && [_sidebarView superview] == _splitView) {
        [_sidebarView removeFromSuperview];
    }
    
    // Set new sidebar view
    _sidebarView = newSidebarView;
    [_sidebarView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add new sidebar view to split view at the correct position
    // NSSplitView manages subview order based on addSubview: order
    if (_position == ObsidianSidebarPositionLeading) {
        // For leading position, sidebar should be first
        // Remove existing views, then add sidebar first, then main content
        if (_mainContentView && [_mainContentView superview] == _splitView) {
            [_mainContentView removeFromSuperview];
        }
        [_splitView addSubview:_sidebarView];
        if (_mainContentView) {
            [_splitView addSubview:_mainContentView];
        }
    } else {
        // For trailing position, sidebar should be last
        [_splitView addSubview:_sidebarView];
    }
    
    [_splitView adjustSubviews];
}

- (void)setMainContentView:(void*)viewHandle {
    if (!viewHandle) {
        return;
    }
    
    NSView* newMainView = (__bridge NSView*)viewHandle;
    if (!newMainView || newMainView == _mainContentView) {
        return;
    }
    
    // Remove old main content view if it exists
    if (_mainContentView && [_mainContentView superview] == _splitView) {
        [_mainContentView removeFromSuperview];
    }
    
    // Set new main content view
    _mainContentView = newMainView;
    [_mainContentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add new main content view to split view at the correct position
    // NSSplitView manages subview order based on addSubview: order
    if (_position == ObsidianSidebarPositionLeading) {
        // For leading position, sidebar should be first, then main content
        // Remove sidebar temporarily, add main content, then re-add sidebar
        if (_sidebarView && [_sidebarView superview] == _splitView) {
            [_sidebarView removeFromSuperview];
        }
        [_splitView addSubview:_mainContentView];
        if (_sidebarView) {
            [_splitView addSubview:_sidebarView];
        }
    } else {
        // For trailing position, main content should be first, then sidebar
        // Remove sidebar temporarily, add main content, then re-add sidebar
        if (_sidebarView && [_sidebarView superview] == _splitView) {
            [_sidebarView removeFromSuperview];
        }
        [_splitView addSubview:_mainContentView];
        if (_sidebarView) {
            [_splitView addSubview:_sidebarView];
        }
    }
    
    [_splitView adjustSubviews];
}

- (void)addToWindow:(void*)windowHandle {
    if (!_splitView || !windowHandle) {
        return;
    }
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        [contentView addSubview:_splitView];
        
        // Pin split view to content view edges using Auto Layout
        [_splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
        NSArray* constraints = @[
            [NSLayoutConstraint constraintWithItem:_splitView
                                         attribute:NSLayoutAttributeLeading
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:contentView
                                         attribute:NSLayoutAttributeLeading
                                        multiplier:1.0
                                          constant:0.0],
            [NSLayoutConstraint constraintWithItem:_splitView
                                         attribute:NSLayoutAttributeTrailing
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:contentView
                                         attribute:NSLayoutAttributeTrailing
                                        multiplier:1.0
                                          constant:0.0],
            [NSLayoutConstraint constraintWithItem:_splitView
                                         attribute:NSLayoutAttributeTop
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:contentView
                                         attribute:NSLayoutAttributeTop
                                        multiplier:1.0
                                          constant:0.0],
            [NSLayoutConstraint constraintWithItem:_splitView
                                         attribute:NSLayoutAttributeBottom
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:contentView
                                         attribute:NSLayoutAttributeBottom
                                        multiplier:1.0
                                          constant:0.0]
        ];
        [contentView addConstraints:constraints];
    }
}

- (void)removeFromParent {
    if (!_splitView) return;
    
    // Remove sidebar and main content views first
    if (_sidebarView && [_sidebarView superview] == _splitView) {
        [_sidebarView removeFromSuperview];
    }
    if (_mainContentView && [_mainContentView superview] == _splitView) {
        [_mainContentView removeFromSuperview];
    }
    
    // Remove from superview
    if ([_splitView superview]) {
        NSView* superview = [_splitView superview];
        // Remove Auto Layout constraints
        [superview removeConstraints:[superview constraints]];
        [_splitView removeFromSuperview];
    }
}

// NSSplitViewDelegate methods for handling minimum/maximum sizes and collapse behavior

- (CGFloat)splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex {
    // Enforce minimum sidebar width
    if (_position == ObsidianSidebarPositionLeading && dividerIndex == 0) {
        // Sidebar is first, minimum position is minimum width
        return _minSidebarWidth;
    } else if (_position == ObsidianSidebarPositionTrailing && dividerIndex == 0) {
        // Sidebar is second, minimum position is total width minus maximum sidebar width
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _maxSidebarWidth;
    }
    return proposedMinimumPosition;
}

- (CGFloat)splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
    // Enforce maximum sidebar width
    if (_position == ObsidianSidebarPositionLeading && dividerIndex == 0) {
        // Sidebar is first, maximum position is maximum width
        return _maxSidebarWidth;
    } else if (_position == ObsidianSidebarPositionTrailing && dividerIndex == 0) {
        // Sidebar is second, maximum position is total width minus minimum sidebar width
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _minSidebarWidth;
    }
    return proposedMaximumPosition;
}

- (BOOL)splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
    // Allow collapsing the sidebar view only
    return (subview == _sidebarView);
}

- (BOOL)splitView:(NSSplitView*)splitView shouldCollapseSubview:(NSView*)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex {
    // Allow double-click to collapse/expand sidebar
    return (subview == _sidebarView);
}

- (void)splitViewDidResizeSubviews:(NSNotification*)notification {
    // Update collapsed state based on sidebar frame width
    // NSSplitView doesn't have positionOfDividerAtIndex:, so we check the sidebar frame
    if (!_splitView || !_sidebarView) {
        return;
    }
    
    // Check if sidebar is collapsed by examining its frame width
    // A collapsed sidebar will have a width of 0 or very small
    double sidebarWidth = [_sidebarView frame].size.width;
    _sidebarCollapsed = (sidebarWidth <= 1.0);
}

@end

// C interface implementation
extern "C" {

ObsidianSplitViewHandle obsidian_macos_create_splitview(ObsidianSplitViewParams params) {
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = [[ObsidianSplitViewWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.splitView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_destroy_splitview(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge_transfer ObsidianSplitViewWrapper*)handle;
        // Remove from parent and clean up children
        [wrapper removeFromParent];
        // wrapper is automatically released due to __bridge_transfer
    }
}

void* obsidian_macos_splitview_get_view(ObsidianSplitViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_splitview_set_sidebar_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setSidebarWidth:width];
    }
}

double obsidian_macos_splitview_get_sidebar_width(ObsidianSplitViewHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper getSidebarWidth];
    }
}

void obsidian_macos_splitview_set_minimum_sidebar_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setMinimumSidebarWidth:width];
    }
}

void obsidian_macos_splitview_set_maximum_sidebar_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setMaximumSidebarWidth:width];
    }
}

void obsidian_macos_splitview_collapse_sidebar(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper collapseSidebar];
    }
}

void obsidian_macos_splitview_expand_sidebar(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper expandSidebar];
    }
}

void obsidian_macos_splitview_toggle_sidebar(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper toggleSidebar];
    }
}

bool obsidian_macos_splitview_is_sidebar_collapsed(ObsidianSplitViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper isSidebarCollapsed];
    }
}

void obsidian_macos_splitview_set_sidebar_view(ObsidianSplitViewHandle handle, void* viewHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setSidebarView:viewHandle];
    }
}

void obsidian_macos_splitview_set_main_view(ObsidianSplitViewHandle handle, void* viewHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setMainContentView:viewHandle];
    }
}

void obsidian_macos_splitview_add_to_window(ObsidianSplitViewHandle handle, void* windowHandle) {
    if (!handle || !windowHandle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_splitview_remove_from_parent(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper removeFromParent];
    }
}

bool obsidian_macos_splitview_is_valid(ObsidianSplitViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return wrapper != nil && wrapper.splitView != nil;
    }
}

} // extern "C"
