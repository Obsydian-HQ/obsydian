/**
 * macOS SplitView FFI - Objective-C++ Implementation
 * 
 * Uses NSSplitView for split view layouts
 * NSSplitView is a special AppKit component that manages its own layout
 * We use frame-based attachment to the window
 */

#import "macos_splitview.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>

// Internal SplitView wrapper class
@interface ObsidianSplitViewWrapper : NSObject <NSSplitViewDelegate> {
    NSSplitView* _splitView;
    NSView* _primaryPaneView;
    NSView* _secondaryPaneView;
    ObsidianSplitPosition _position;
    double _minPrimaryPaneWidth;
    double _maxPrimaryPaneWidth;
    BOOL _primaryPaneCollapsed;
}

@property (nonatomic, strong) NSSplitView* splitView;

- (instancetype)initWithParams:(ObsidianSplitViewParams)params;
- (void*)getView;
- (void)setPrimaryPaneWidth:(double)width;
- (double)getPrimaryPaneWidth;
- (void)setMinimumPrimaryPaneWidth:(double)width;
- (void)setMaximumPrimaryPaneWidth:(double)width;
- (void)collapsePrimaryPane;
- (void)expandPrimaryPane;
- (void)togglePrimaryPane;
- (bool)isPrimaryPaneCollapsed;
- (void)setPrimaryPaneView:(void*)viewHandle;
- (void)setSecondaryPaneView:(void*)viewHandle;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

@end

@implementation ObsidianSplitViewWrapper

- (instancetype)initWithParams:(ObsidianSplitViewParams)params {
    self = [super init];
    if (self) {
        _position = params.position;
        _minPrimaryPaneWidth = params.minPrimaryPaneWidth > 0 ? params.minPrimaryPaneWidth : 150.0;
        _maxPrimaryPaneWidth = params.maxPrimaryPaneWidth > 0 ? params.maxPrimaryPaneWidth : 400.0;
        _primaryPaneCollapsed = NO;
        
        // Create NSSplitView with default frame
        NSRect frame = NSMakeRect(0, 0, 800, 600);
        _splitView = [[NSSplitView alloc] initWithFrame:frame];
        
        // FRAME-BASED: Use autoresizing to fill parent
        _splitView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        
        // Set vertical orientation for left/right split
        [_splitView setVertical:YES];
        [_splitView setDividerStyle:NSSplitViewDividerStyleThin];
        [_splitView setDelegate:self];
        
        // Create placeholder views for primary and secondary panes
        double initialWidth = params.initialPrimaryPaneWidth > 0 ? params.initialPrimaryPaneWidth : 200.0;
        NSRect primaryPaneFrame = NSMakeRect(0, 0, initialWidth, frame.size.height);
        _primaryPaneView = [[NSView alloc] initWithFrame:primaryPaneFrame];
        _primaryPaneView.autoresizingMask = NSViewHeightSizable;
        
        NSRect secondaryFrame = NSMakeRect(initialWidth, 0, frame.size.width - initialWidth, frame.size.height);
        _secondaryPaneView = [[NSView alloc] initWithFrame:secondaryFrame];
        _secondaryPaneView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        
        // Add views to split view (order matters for position)
        if (_position == ObsidianSplitPositionLeading) {
            [_splitView addSubview:_primaryPaneView];
            [_splitView addSubview:_secondaryPaneView];
        } else {
            [_splitView addSubview:_secondaryPaneView];
            [_splitView addSubview:_primaryPaneView];
        }
    }
    return self;
}

- (void*)getView {
    return (__bridge void*)_splitView;
}

- (void)setPrimaryPaneWidth:(double)width {
    if (!_splitView || _primaryPaneCollapsed) {
        return;
    }
    
    // Clamp width to min/max values
    width = MAX(_minPrimaryPaneWidth, MIN(_maxPrimaryPaneWidth, width));
    
    NSRect splitFrame = [_splitView frame];
    if (splitFrame.size.width == 0 || splitFrame.size.height == 0) {
        return;
    }
    
    double dividerPosition;
    if (_position == ObsidianSplitPositionLeading) {
        dividerPosition = width;
    } else {
        dividerPosition = splitFrame.size.width - width;
    }
    
    NSInteger dividerIndex = 0;
    [_splitView setPosition:dividerPosition ofDividerAtIndex:dividerIndex];
    [_splitView adjustSubviews];
}

- (double)getPrimaryPaneWidth {
    if (!_splitView || _primaryPaneCollapsed) {
        return 0.0;
    }
    
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        return [_primaryPaneView frame].size.width;
    }
    
    return 0.0;
}

- (void)setMinimumPrimaryPaneWidth:(double)width {
    _minPrimaryPaneWidth = width > 0 ? width : 150.0;
}

- (void)setMaximumPrimaryPaneWidth:(double)width {
    _maxPrimaryPaneWidth = width > 0 ? width : 400.0;
    double currentWidth = [self getPrimaryPaneWidth];
    if (currentWidth > _maxPrimaryPaneWidth) {
        [self setPrimaryPaneWidth:_maxPrimaryPaneWidth];
    }
}

- (void)collapsePrimaryPane {
    if (!_splitView || _primaryPaneCollapsed) {
        return;
    }
    
    NSInteger dividerIndex = 0;
    
    if (_position == ObsidianSplitPositionLeading) {
        [_splitView setPosition:0.0 ofDividerAtIndex:dividerIndex];
    } else {
        NSRect splitFrame = [_splitView frame];
        [_splitView setPosition:splitFrame.size.width ofDividerAtIndex:dividerIndex];
    }
    
    _primaryPaneCollapsed = YES;
    [_splitView adjustSubviews];
}

- (void)expandPrimaryPane {
    if (!_splitView || !_primaryPaneCollapsed) {
        return;
    }
    
    double preferredWidth = 200.0;
    [self setPrimaryPaneWidth:preferredWidth];
    
    _primaryPaneCollapsed = NO;
    [_splitView adjustSubviews];
}

- (void)togglePrimaryPane {
    if (_primaryPaneCollapsed) {
        [self expandPrimaryPane];
    } else {
        [self collapsePrimaryPane];
    }
}

- (bool)isPrimaryPaneCollapsed {
    return _primaryPaneCollapsed;
}

- (void)setPrimaryPaneView:(void*)viewHandle {
    if (!viewHandle) {
        return;
    }
    
    NSView* newPrimaryPaneView = (__bridge NSView*)viewHandle;
    if (!newPrimaryPaneView || newPrimaryPaneView == _primaryPaneView) {
        return;
    }
    
    // Remove old primary pane view if it exists
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        [_primaryPaneView removeFromSuperview];
    }
    
    // Remove new primary pane view from its current parent if it has one
    if ([newPrimaryPaneView superview]) {
        [newPrimaryPaneView removeFromSuperview];
    }
    
    // Set new primary pane view
    _primaryPaneView = newPrimaryPaneView;
    _primaryPaneView.autoresizingMask = NSViewHeightSizable;
    
    // Re-add views in correct order
    if (_position == ObsidianSplitPositionLeading) {
        if (_secondaryPaneView && [_secondaryPaneView superview] == _splitView) {
            [_secondaryPaneView removeFromSuperview];
        }
        [_splitView addSubview:_primaryPaneView];
        if (_secondaryPaneView) {
            [_splitView addSubview:_secondaryPaneView];
        }
    } else {
        [_splitView addSubview:_primaryPaneView];
    }
    
    [_splitView adjustSubviews];
}

- (void)setSecondaryPaneView:(void*)viewHandle {
    if (!viewHandle) {
        return;
    }
    
    NSView* newSecondaryView = (__bridge NSView*)viewHandle;
    if (!newSecondaryView || newSecondaryView == _secondaryPaneView) {
        return;
    }
    
    // Remove old secondary pane view if it exists
    if (_secondaryPaneView && [_secondaryPaneView superview] == _splitView) {
        [_secondaryPaneView removeFromSuperview];
    }
    
    // Remove new secondary pane view from its current parent if it has one
    if ([newSecondaryView superview]) {
        [newSecondaryView removeFromSuperview];
    }
    
    // Set new secondary pane view
    _secondaryPaneView = newSecondaryView;
    _secondaryPaneView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    
    // Preserve primary pane frame
    NSRect primaryPaneFrame = NSZeroRect;
    BOOL primaryPaneWasInSplitView = NO;
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        primaryPaneFrame = [_primaryPaneView frame];
        primaryPaneWasInSplitView = YES;
        [_primaryPaneView removeFromSuperview];
    }
    
    // Re-add views in correct order
    if (_position == ObsidianSplitPositionLeading) {
        if (_primaryPaneView) {
            if (primaryPaneWasInSplitView && !NSIsEmptyRect(primaryPaneFrame)) {
                [_primaryPaneView setFrame:primaryPaneFrame];
            }
            [_splitView addSubview:_primaryPaneView];
        }
        [_splitView addSubview:_secondaryPaneView];
    } else {
        [_splitView addSubview:_secondaryPaneView];
        if (_primaryPaneView) {
            if (primaryPaneWasInSplitView && !NSIsEmptyRect(primaryPaneFrame)) {
                [_primaryPaneView setFrame:primaryPaneFrame];
            }
            [_splitView addSubview:_primaryPaneView];
        }
    }
    
    [_splitView adjustSubviews];
}

- (void)addToWindow:(void*)windowHandle {
    if (!_splitView || !windowHandle) {
        return;
    }
    
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        
        // FRAME-BASED: Set frame to fill content view
        _splitView.frame = contentView.bounds;
        [contentView addSubview:_splitView];
        
        [_splitView adjustSubviews];
        
        // Apply initial primary pane width
        if (!_primaryPaneCollapsed && _primaryPaneView) {
            double initialWidth = 250.0;
            [self setPrimaryPaneWidth:initialWidth];
        }
    }
}

- (void)removeFromParent {
    if (!_splitView) return;
    
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        [_primaryPaneView removeFromSuperview];
    }
    if (_secondaryPaneView && [_secondaryPaneView superview] == _splitView) {
        [_secondaryPaneView removeFromSuperview];
    }
    
    if ([_splitView superview]) {
        [_splitView removeFromSuperview];
    }
}

// NSSplitViewDelegate methods

- (CGFloat)splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex {
    if (_position == ObsidianSplitPositionLeading && dividerIndex == 0) {
        return _minPrimaryPaneWidth;
    } else if (_position == ObsidianSplitPositionTrailing && dividerIndex == 0) {
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _maxPrimaryPaneWidth;
    }
    return proposedMinimumPosition;
}

- (CGFloat)splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
    if (_position == ObsidianSplitPositionLeading && dividerIndex == 0) {
        return _maxPrimaryPaneWidth;
    } else if (_position == ObsidianSplitPositionTrailing && dividerIndex == 0) {
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _minPrimaryPaneWidth;
    }
    return proposedMaximumPosition;
}

- (BOOL)splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
    return (subview == _primaryPaneView);
}

- (BOOL)splitView:(NSSplitView*)splitView shouldCollapseSubview:(NSView*)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex {
    return (subview == _primaryPaneView);
}

- (void)splitViewDidResizeSubviews:(NSNotification*)notification {
    if (!_splitView || !_primaryPaneView) {
        return;
    }
    
    double primaryPaneWidth = [_primaryPaneView frame].size.width;
    _primaryPaneCollapsed = (primaryPaneWidth <= 1.0);
}

@end

// C interface implementation
extern "C" {

ObsidianSplitViewHandle obsidian_macos_create_splitview(ObsidianSplitViewParams params) {
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = [[ObsidianSplitViewWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.splitView) {
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_destroy_splitview(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge_transfer ObsidianSplitViewWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void* obsidian_macos_splitview_get_view(ObsidianSplitViewHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper getView];
    }
}

void obsidian_macos_splitview_set_primary_pane_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setPrimaryPaneWidth:width];
    }
}

double obsidian_macos_splitview_get_primary_pane_width(ObsidianSplitViewHandle handle) {
    if (!handle) return 0.0;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper getPrimaryPaneWidth];
    }
}

void obsidian_macos_splitview_set_minimum_primary_pane_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setMinimumPrimaryPaneWidth:width];
    }
}

void obsidian_macos_splitview_set_maximum_primary_pane_width(ObsidianSplitViewHandle handle, double width) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setMaximumPrimaryPaneWidth:width];
    }
}

void obsidian_macos_splitview_collapse_primary_pane(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper collapsePrimaryPane];
    }
}

void obsidian_macos_splitview_expand_primary_pane(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper expandPrimaryPane];
    }
}

void obsidian_macos_splitview_toggle_primary_pane(ObsidianSplitViewHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper togglePrimaryPane];
    }
}

bool obsidian_macos_splitview_is_primary_pane_collapsed(ObsidianSplitViewHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        return [wrapper isPrimaryPaneCollapsed];
    }
}

void obsidian_macos_splitview_set_primary_pane_view(ObsidianSplitViewHandle handle, void* viewHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setPrimaryPaneView:viewHandle];
    }
}

void obsidian_macos_splitview_set_secondary_pane_view(ObsidianSplitViewHandle handle, void* viewHandle) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianSplitViewWrapper* wrapper = (__bridge ObsidianSplitViewWrapper*)handle;
        [wrapper setSecondaryPaneView:viewHandle];
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
