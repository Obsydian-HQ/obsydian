/**
 * macOS SplitView FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSSplitView APIs for split view layouts
 * 
 * This implementation uses NSSplitView to create a split view with:
 * - Collapse/expand functionality
 * - Resizable divider (drag to resize)
 * - Minimum/maximum primary pane widths
 * - Split position (leading/trailing)
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
// Note: primaryPaneView and secondaryPaneView are managed via instance variables and custom setters
// No @property declaration to avoid type mismatch with custom setter signatures

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
        
        // Create NSSplitView with zero frame (will be constrained by Auto Layout)
        NSRect frame = NSMakeRect(0, 0, 0, 0);
        _splitView = [[NSSplitView alloc] initWithFrame:frame];
        
        // Set vertical orientation for left/right split (primary + secondary panes)
        // Vertical = horizontal divider (left/right panes)
        [_splitView setVertical:YES];
        
        // Set divider style (thin divider for native macOS look)
        [_splitView setDividerStyle:NSSplitViewDividerStyleThin];
        
        // Set delegate for collapse behavior
        [_splitView setDelegate:self];
        
        // Disable autoresizing mask - we'll use Auto Layout
        [_splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Create placeholder views for primary and secondary panes
        // These will be replaced when setPrimaryPaneView/setSecondaryPaneView are called
        NSRect primaryPaneFrame = NSMakeRect(0, 0, params.initialPrimaryPaneWidth > 0 ? params.initialPrimaryPaneWidth : 200.0, 100);
        _primaryPaneView = [[NSView alloc] initWithFrame:primaryPaneFrame];
        [_primaryPaneView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        NSRect secondaryFrame = NSMakeRect(0, 0, 300, 100);
        _secondaryPaneView = [[NSView alloc] initWithFrame:secondaryFrame];
        [_secondaryPaneView setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // Add views to split view (order matters for position)
        // For leading position: primary pane first, then secondary pane
        // For trailing position: secondary pane first, then primary pane
        if (_position == ObsidianSplitPositionLeading) {
            [_splitView addSubview:_primaryPaneView];
            [_splitView addSubview:_secondaryPaneView];
        } else {
            [_splitView addSubview:_secondaryPaneView];
            [_splitView addSubview:_primaryPaneView];
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

- (void)setPrimaryPaneWidth:(double)width {
    if (!_splitView || _primaryPaneCollapsed) {
        return;
    }
    
    // Clamp width to min/max values
    width = MAX(_minPrimaryPaneWidth, MIN(_maxPrimaryPaneWidth, width));
    
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
    if (_position == ObsidianSplitPositionLeading) {
        // Primary pane is first, divider position is primary pane width
        dividerPosition = width;
    } else {
        // Primary pane is second, divider position is secondary pane width
        dividerPosition = splitFrame.size.width - width;
    }
    
    // Set divider position
    // NSSplitView uses setPosition:ofDividerAtIndex: to set divider position
    NSInteger dividerIndex = 0;  // There's one divider between two panes
    [_splitView setPosition:dividerPosition ofDividerAtIndex:dividerIndex];
    [_splitView adjustSubviews];
}

- (double)getPrimaryPaneWidth {
    if (!_splitView || _primaryPaneCollapsed) {
        return 0.0;
    }
    
    NSRect splitFrame = [_splitView frame];
    if (splitFrame.size.width == 0) {
        return 0.0;
    }
    
    // Calculate primary pane width from subview frames
    // NSSplitView doesn't have positionOfDividerAtIndex:, so we calculate from subview positions
    if (_position == ObsidianSplitPositionLeading) {
        // Primary pane is first subview, its width is the frame width
        if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
            return [_primaryPaneView frame].size.width;
        }
    } else {
        // Primary pane is second subview, its width is the frame width
        if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
            return [_primaryPaneView frame].size.width;
        }
    }
    
    return 0.0;
}

- (void)setMinimumPrimaryPaneWidth:(double)width {
    _minPrimaryPaneWidth = width > 0 ? width : 150.0;
    // Invalidate split view layout to apply new minimum
    [_splitView setNeedsLayout:YES];
}

- (void)setMaximumPrimaryPaneWidth:(double)width {
    _maxPrimaryPaneWidth = width > 0 ? width : 400.0;
    // If current width exceeds max, clamp it
    double currentWidth = [self getPrimaryPaneWidth];
    if (currentWidth > _maxPrimaryPaneWidth) {
        [self setPrimaryPaneWidth:_maxPrimaryPaneWidth];
    }
}

- (void)collapsePrimaryPane {
    if (!_splitView || _primaryPaneCollapsed) {
        return;
    }
    
    // Collapse the primary pane
    // NSSplitView provides collapse behavior via delegate methods
    // We can use setPosition:ofDividerAtIndex: to collapse
    NSInteger dividerIndex = 0;
    
    if (_position == ObsidianSplitPositionLeading) {
        // Collapse by moving divider to position 0
        [_splitView setPosition:0.0 ofDividerAtIndex:dividerIndex];
    } else {
        // Collapse by moving divider to the end
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
    
    // Expand the primary pane to its preferred width
    // Use a default width if not specified (200 points)
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
    
    // CRITICAL: Remove new primary pane view from its current parent if it has one
    // This prevents constraint conflicts when adding to SplitView
    // Views can only have one parent at a time in AppKit
    if ([newPrimaryPaneView superview]) {
        [newPrimaryPaneView removeFromSuperview];
    }
    
    // Set new primary pane view
    _primaryPaneView = newPrimaryPaneView;
    [_primaryPaneView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add new primary pane view to split view at the correct position
    // NSSplitView manages subview order based on addSubview: order
    if (_position == ObsidianSplitPositionLeading) {
        // For leading position, primary pane should be first
        // Remove existing views, then add primary pane first, then secondary pane
        if (_secondaryPaneView && [_secondaryPaneView superview] == _splitView) {
            [_secondaryPaneView removeFromSuperview];
        }
        [_splitView addSubview:_primaryPaneView];
        if (_secondaryPaneView) {
            [_splitView addSubview:_secondaryPaneView];
        }
    } else {
        // For trailing position, primary pane should be last
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
    
    // CRITICAL: Remove new secondary pane view from its current parent if it has one
    // This prevents constraint conflicts when adding to SplitView
    // Views can only have one parent at a time in AppKit
    if ([newSecondaryView superview]) {
        [newSecondaryView removeFromSuperview];
    }
    
    // Set new secondary pane view
    _secondaryPaneView = newSecondaryView;
    [_secondaryPaneView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    // Add new secondary pane view to split view at the correct position
    // NSSplitView manages subview order based on addSubview: order
    // CRITICAL: The order of addSubview: calls determines left-to-right order
    // CRITICAL: We must preserve primary pane frame when re-adding to prevent zero-size issue
    NSRect primaryPaneFrame = NSZeroRect;
    BOOL primaryPaneWasInSplitView = NO;
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        primaryPaneFrame = [_primaryPaneView frame];
        primaryPaneWasInSplitView = YES;
        [_primaryPaneView removeFromSuperview];
    }
    
    if (_position == ObsidianSplitPositionLeading) {
        // For leading position, primary pane should be first (left), then secondary pane (right)
        // Add primary pane first (will be on the left)
        if (_primaryPaneView) {
            // Restore frame if we had one, otherwise set a default
            if (primaryPaneWasInSplitView && !NSIsEmptyRect(primaryPaneFrame)) {
                [_primaryPaneView setFrame:primaryPaneFrame];
            } else {
                // Set initial frame if primary pane wasn't in split view or had zero frame
                NSRect splitFrame = [_splitView frame];
                if (NSIsEmptyRect(splitFrame)) {
                    splitFrame = NSMakeRect(0, 0, 1000, 700);  // Default
                }
                double initialWidth = 250.0;  // Default primary pane width
                [_primaryPaneView setFrame:NSMakeRect(0, 0, initialWidth, splitFrame.size.height)];
            }
            [_splitView addSubview:_primaryPaneView];
        }
        // Add secondary pane second (will be on the right)
        [_splitView addSubview:_secondaryPaneView];
    } else {
        // For trailing position, secondary pane should be first (left), then primary pane (right)
        // Add secondary pane first (will be on the left)
        [_splitView addSubview:_secondaryPaneView];
        // Add primary pane second (will be on the right)
        if (_primaryPaneView) {
            // Restore frame if we had one, otherwise set a default
            if (primaryPaneWasInSplitView && !NSIsEmptyRect(primaryPaneFrame)) {
                [_primaryPaneView setFrame:primaryPaneFrame];
            } else {
                // Set initial frame if primary pane wasn't in split view or had zero frame
                NSRect splitFrame = [_splitView frame];
                if (NSIsEmptyRect(splitFrame)) {
                    splitFrame = NSMakeRect(0, 0, 1000, 700);  // Default
                }
                double initialWidth = 250.0;  // Default primary pane width
                NSRect secondaryFrame = [_secondaryPaneView frame];
                double xPos = NSIsEmptyRect(secondaryFrame) ? 0 : secondaryFrame.size.width;
                [_primaryPaneView setFrame:NSMakeRect(xPos, 0, initialWidth, splitFrame.size.height)];
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
    
    // Get the content view from the window handle
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        NSView* contentView = (__bridge NSView*)contentViewPtr;
        
        // CRITICAL: Set a proper initial frame for SplitView before adding to window
        // NSSplitView needs a non-zero frame to properly layout its subviews
        NSRect contentBounds = [contentView bounds];
        if (contentBounds.size.width == 0 || contentBounds.size.height == 0) {
            // Content view not laid out yet, use window size
            // We'll update the frame when the window is shown
            contentBounds = NSMakeRect(0, 0, 1000, 700);  // Default size
        }
        [_splitView setFrame:contentBounds];
        
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
        
        // CRITICAL: Force layout update after adding to window
        // This ensures NSSplitView properly sizes its subviews
        [contentView setNeedsLayout:YES];
        [contentView layoutSubtreeIfNeeded];
        
        // CRITICAL: Ensure both subviews have proper frames before adjustSubviews
        // NSSplitView requires both subviews to have non-zero frames
        NSArray* subviews = [_splitView subviews];
        if ([subviews count] >= 2) {
            NSView* firstSubview = [subviews objectAtIndex:0];
            NSView* secondSubview = [subviews objectAtIndex:1];
            
            NSRect splitFrame = [_splitView frame];
            if (NSIsEmptyRect(splitFrame)) {
                splitFrame = contentBounds;
            }
            
            // If either subview has zero frame, set initial frames
            // NSSplitView will adjust these, but we need non-zero starting points
            if (NSIsEmptyRect([firstSubview frame])) {
                double initialWidth = 250.0;  // Default primary pane width
                [firstSubview setFrame:NSMakeRect(0, 0, initialWidth, splitFrame.size.height)];
            }
            if (NSIsEmptyRect([secondSubview frame])) {
                NSRect firstFrame = [firstSubview frame];
                double remainingWidth = splitFrame.size.width - firstFrame.size.width;
                [secondSubview setFrame:NSMakeRect(firstFrame.size.width, 0, remainingWidth, splitFrame.size.height)];
            }
        }
        
        [_splitView adjustSubviews];
        
        // Apply initial primary pane width if set
        if (!_primaryPaneCollapsed && _primaryPaneView) {
            double initialWidth = 250.0;  // Default
            [self setPrimaryPaneWidth:initialWidth];
        }
    }
}

- (void)removeFromParent {
    if (!_splitView) return;
    
    // Remove primary and secondary pane views first
    if (_primaryPaneView && [_primaryPaneView superview] == _splitView) {
        [_primaryPaneView removeFromSuperview];
    }
    if (_secondaryPaneView && [_secondaryPaneView superview] == _splitView) {
        [_secondaryPaneView removeFromSuperview];
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
    // Enforce minimum primary pane width
    if (_position == ObsidianSplitPositionLeading && dividerIndex == 0) {
        // Primary pane is first, minimum position is minimum width
        return _minPrimaryPaneWidth;
    } else if (_position == ObsidianSplitPositionTrailing && dividerIndex == 0) {
        // Primary pane is second, minimum position is total width minus maximum primary pane width
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _maxPrimaryPaneWidth;
    }
    return proposedMinimumPosition;
}

- (CGFloat)splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
    // Enforce maximum primary pane width
    if (_position == ObsidianSplitPositionLeading && dividerIndex == 0) {
        // Primary pane is first, maximum position is maximum width
        return _maxPrimaryPaneWidth;
    } else if (_position == ObsidianSplitPositionTrailing && dividerIndex == 0) {
        // Primary pane is second, maximum position is total width minus minimum primary pane width
        NSRect splitFrame = [splitView frame];
        return splitFrame.size.width - _minPrimaryPaneWidth;
    }
    return proposedMaximumPosition;
}

- (BOOL)splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
    // Allow collapsing the primary pane view only
    return (subview == _primaryPaneView);
}

- (BOOL)splitView:(NSSplitView*)splitView shouldCollapseSubview:(NSView*)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex {
    // Allow double-click to collapse/expand primary pane
    return (subview == _primaryPaneView);
}

- (void)splitViewDidResizeSubviews:(NSNotification*)notification {
    // Update collapsed state based on primary pane frame width
    // NSSplitView doesn't have positionOfDividerAtIndex:, so we check the primary pane frame
    if (!_splitView || !_primaryPaneView) {
        return;
    }
    
    // Check if primary pane is collapsed by examining its frame width
    // A collapsed primary pane will have a width of 0 or very small
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
