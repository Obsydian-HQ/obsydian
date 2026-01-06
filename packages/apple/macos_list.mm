/**
 * macOS List FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSTableView APIs configured as a single-column list
 * 
 * This implementation uses NSTableView wrapped in NSScrollView for scrolling.
 * The table view is configured as a single-column list with no headers, matching
 * how SwiftUI's List view works on macOS.
 */

#import "macos_list.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>
#import <vector>
#import <string>

// Internal data structure to store list items
struct ObsidianListItem {
    std::string text;
};

// Internal list wrapper class
@interface ObsidianListWrapper : NSObject <NSTableViewDataSource, NSTableViewDelegate> {
    NSTableView* _tableView;
    NSScrollView* _scrollView;
    NSTableColumn* _column;
    std::vector<ObsidianListItem> _items;
    ObsidianListSelectionCallback _callback;
    void* _userData;
}

@property (nonatomic, strong) NSTableView* tableView;
@property (nonatomic, strong) NSScrollView* scrollView;

- (instancetype)initWithParams:(ObsidianListParams)params;
- (int)addItem:(const char*)itemText;
- (bool)removeItem:(int)itemIndex;
- (void)clear;
- (int)getItemCount;
- (const char*)getItem:(int)itemIndex;
- (bool)setItem:(int)itemIndex text:(const char*)itemText;
- (void)setOnSelection:(ObsidianListSelectionCallback)callback userData:(void*)userData;
- (int)getSelectedIndex;
- (bool)setSelectedIndex:(int)itemIndex;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

// NSTableViewDataSource methods
- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView;

// NSTableViewDelegate methods (VIEW-BASED)
- (NSView*)tableView:(NSTableView*)tableView viewForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row;
- (void)tableViewSelectionDidChange:(NSNotification*)notification;

@end

@implementation ObsidianListWrapper

- (instancetype)initWithParams:(ObsidianListParams)params {
    self = [super init];
    if (self) {
        // Create NSTableView
        _tableView = [[NSTableView alloc] init];
        
        // NATIVE SIDEBAR STYLE (macOS 11+)
        // Based on iTerm2, Finder, and other native apps:
        // - Use source list style
        // - Use automatic row heights
        // - Disable focus ring
        // - Hide header view
        if (@available(macOS 11.0, *)) {
            [_tableView setStyle:NSTableViewStyleSourceList];
        }
        
        // Configure table view for native sidebar behavior
        [_tableView setAllowsColumnReordering:NO];
        [_tableView setAllowsColumnResizing:NO];
        [_tableView setAllowsColumnSelection:NO];
        [_tableView setAllowsMultipleSelection:NO];
        
        // CRITICAL: Use default row size to match system sidebar icon size preference
        // This respects the user's System Preferences > General > Sidebar icon size
        [_tableView setRowSizeStyle:NSTableViewRowSizeStyleDefault];
        
        // Source list style handles backgrounds automatically - no alternating colors
        [_tableView setUsesAlternatingRowBackgroundColors:NO];
        
        // No grid lines for sidebar list appearance
        [_tableView setGridStyleMask:NSTableViewGridNone];
        
        // CRITICAL: Remove header view completely (native sidebars don't have headers)
        [_tableView setHeaderView:nil];
        
        // Source list selection highlight style for native sidebar look
        [_tableView setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleSourceList];
        
        // No focus ring (native sidebars don't show focus rings)
        [_tableView setFocusRingType:NSFocusRingTypeNone];
        
        // CRITICAL: Use automatic row heights (from iTerm2 implementation)
        // This allows cells to size properly based on content
        [_tableView setUsesAutomaticRowHeights:YES];
        
        // CRITICAL: Disable floating group rows (from iTerm2 implementation)
        [_tableView setFloatsGroupRows:NO];
        
        // Create a single column for the list
        _column = [[NSTableColumn alloc] initWithIdentifier:@"SidebarItem"];
        
        // CRITICAL: Let column resize automatically with table view width
        // Don't set a fixed width - this causes horizontal scrolling issues
        [_column setResizingMask:NSTableColumnAutoresizingMask];
        
        // Add the single column to the table view
        [_tableView addTableColumn:_column];
        
        // Set data source and delegate
        [_tableView setDataSource:self];
        [_tableView setDelegate:self];
        
        // Create NSScrollView to wrap the table view (for scrolling)
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        _scrollView = [[NSScrollView alloc] initWithFrame:frame];
        
        // CRITICAL: Only vertical scrolling, never horizontal
        // This is how native sidebars work - they truncate text, not scroll horizontally
        [_scrollView setHasVerticalScroller:YES];
        [_scrollView setHasHorizontalScroller:NO];
        [_scrollView setAutohidesScrollers:YES];
        
        // No border for native sidebar look
        [_scrollView setBorderType:NSNoBorder];
        
        // Transparent backgrounds for vibrancy effect
        [_scrollView setDrawsBackground:NO];
        [[_scrollView contentView] setDrawsBackground:NO];
        
        // Set document view
        [_scrollView setDocumentView:_tableView];
        
        // CRITICAL: Scroll view must resize with parent for split view integration
        [_scrollView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        
        // CRITICAL: Also set table view to resize with scroll view
        [_tableView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        
        // Ensure the table view background is transparent
        [_tableView setBackgroundColor:[NSColor clearColor]];
        
        _callback = nullptr;
        _userData = nullptr;
    }
    return self;
}

- (int)addItem:(const char*)itemText {
    if (!_tableView || !itemText) {
        return -1;
    }
    
    // Create item
    ObsidianListItem item;
    item.text = std::string(itemText);
    
    // Add item
    _items.push_back(item);
    int itemIndex = (int)(_items.size() - 1);
    
    // Reload table view to show new item
    [_tableView reloadData];
    
    return itemIndex;
}

- (bool)removeItem:(int)itemIndex {
    if (itemIndex < 0 || itemIndex >= (int)_items.size()) {
        return false;
    }
    
    _items.erase(_items.begin() + itemIndex);
    [_tableView reloadData];
    
    return true;
}

- (void)clear {
    _items.clear();
    [_tableView reloadData];
}

- (int)getItemCount {
    return (int)_items.size();
}

- (const char*)getItem:(int)itemIndex {
    if (itemIndex < 0 || itemIndex >= (int)_items.size()) {
        return nullptr;
    }
    return _items[itemIndex].text.c_str();
}

- (bool)setItem:(int)itemIndex text:(const char*)itemText {
    if (itemIndex < 0 || itemIndex >= (int)_items.size() || !itemText) {
        return false;
    }
    
    _items[itemIndex].text = std::string(itemText);
    [_tableView reloadData];
    
    return true;
}

- (void)setOnSelection:(ObsidianListSelectionCallback)callback userData:(void*)userData {
    _callback = callback;
    _userData = userData;
}

- (int)getSelectedIndex {
    if (!_tableView) {
        return -1;
    }
    
    NSInteger selectedRow = [_tableView selectedRow];
    if (selectedRow < 0 || selectedRow >= (NSInteger)_items.size()) {
        return -1;
    }
    
    return (int)selectedRow;
}

- (bool)setSelectedIndex:(int)itemIndex {
    if (!_tableView) {
        return false;
    }
    
    if (itemIndex < 0 || itemIndex >= (int)_items.size()) {
        // Deselect all
        [_tableView deselectAll:nil];
        return true;
    }
    
    NSIndexSet* indexSet = [NSIndexSet indexSetWithIndex:itemIndex];
    [_tableView selectRowIndexes:indexSet byExtendingSelection:NO];
    
    return true;
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

- (void)setEnabled:(bool)enabled {
    if (_tableView) {
        [_tableView setEnabled:enabled];
    }
}

- (bool)isEnabled {
    if (_tableView) {
        return [_tableView isEnabled];
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
    
    // Clear data source and delegate before removing
    [_tableView setDataSource:nil];
    [_tableView setDelegate:nil];
    
    // Clear callback
    _callback = nullptr;
    _userData = nullptr;
    
    // Remove from superview
    if ([_scrollView superview]) {
        [_scrollView removeFromSuperview];
    }
}

// NSTableViewDataSource methods
- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView {
    return (NSInteger)_items.size();
}

// NSTableViewDelegate methods - VIEW-BASED TABLE VIEW
// Based on iTerm2 SSHFilePanelSidebar implementation
- (NSView*)tableView:(NSTableView*)tableView viewForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row {
    if (row < 0 || row >= (NSInteger)_items.size()) {
        return nil;
    }
    
    static NSUserInterfaceItemIdentifier cellID = @"SidebarCellView";
    
    // Try to reuse an existing cell
    NSTableCellView* cellView = [tableView makeViewWithIdentifier:cellID owner:self];
    
    if (!cellView) {
        // Create a new cell view with proper native sidebar configuration
        cellView = [[NSTableCellView alloc] init];
        [cellView setIdentifier:cellID];
        
        // Create text field for the cell
        NSTextField* textField = [[NSTextField alloc] init];
        [textField setTranslatesAutoresizingMaskIntoConstraints:NO];
        
        // CRITICAL: Configure text field for native sidebar appearance
        // Based on iTerm2 and other native Mac apps:
        [textField setBordered:NO];
        [textField setEditable:NO];
        [textField setSelectable:NO];
        [textField setDrawsBackground:NO];
        [textField setBackgroundColor:[NSColor clearColor]];
        
        // Use system font for native look
        [textField setFont:[NSFont systemFontOfSize:[NSFont systemFontSize]]];
        
        // Text color adapts to selection state automatically with source list style
        [textField setTextColor:[NSColor labelColor]];
        
        // CRITICAL: Line break mode to truncate text instead of causing horizontal scroll
        [[textField cell] setLineBreakMode:NSLineBreakByTruncatingTail];
        [[textField cell] setTruncatesLastVisibleLine:YES];
        
        // Single line for sidebar items
        [textField setMaximumNumberOfLines:1];
        [textField setAllowsDefaultTighteningForTruncation:NO];
        
        // Don't allow text field to compress - it should fill available width
        [textField setContentCompressionResistancePriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
        [textField setContentHuggingPriority:NSLayoutPriorityDefaultLow forOrientation:NSLayoutConstraintOrientationHorizontal];
        
        // Add text field to cell view
        [cellView addSubview:textField];
        [cellView setTextField:textField];
        
        // Set up constraints for proper layout
        // Native sidebar has specific padding: leading ~12pt, trailing ~8pt
        NSLayoutConstraint* leadingConstraint = [textField.leadingAnchor constraintEqualToAnchor:cellView.leadingAnchor constant:3.0];
        NSLayoutConstraint* trailingConstraint = [textField.trailingAnchor constraintLessThanOrEqualToAnchor:cellView.trailingAnchor constant:-3.0];
        NSLayoutConstraint* centerYConstraint = [textField.centerYAnchor constraintEqualToAnchor:cellView.centerYAnchor];
        
        // Make trailing constraint not required so text can expand
        [trailingConstraint setPriority:NSLayoutPriorityDefaultHigh];
        
        [NSLayoutConstraint activateConstraints:@[
            leadingConstraint,
            trailingConstraint,
            centerYConstraint
        ]];
    }
    
    // Set the text value
    NSString* text = [NSString stringWithUTF8String:_items[row].text.c_str()];
    [cellView.textField setStringValue:text];
    
    return cellView;
}

- (void)tableViewSelectionDidChange:(NSNotification*)notification {
    if (_callback) {
        NSInteger selectedRow = [_tableView selectedRow];
        if (selectedRow >= 0 && selectedRow < (NSInteger)_items.size()) {
            _callback((int)selectedRow, _userData);
        }
    }
}

@end

// C interface implementation
ObsidianListHandle obsidian_macos_create_list(ObsidianListParams params) {
    ObsidianListWrapper* wrapper = [[ObsidianListWrapper alloc] initWithParams:params];
    if (!wrapper) {
        return nullptr;
    }
    
    // Return as bridged pointer (retained)
    return (__bridge_retained void*)wrapper;
}

int obsidian_macos_list_add_item(ObsidianListHandle handle, const char* itemText) {
    if (!handle || !itemText) {
        return -1;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper addItem:itemText];
}

bool obsidian_macos_list_remove_item(ObsidianListHandle handle, int itemIndex) {
    if (!handle) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper removeItem:itemIndex];
}

void obsidian_macos_list_clear(ObsidianListHandle handle) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    [wrapper clear];
}

int obsidian_macos_list_get_item_count(ObsidianListHandle handle) {
    if (!handle) {
        return 0;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper getItemCount];
}

const char* obsidian_macos_list_get_item(ObsidianListHandle handle, int itemIndex) {
    if (!handle) {
        return nullptr;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper getItem:itemIndex];
}

bool obsidian_macos_list_set_item(ObsidianListHandle handle, int itemIndex, const char* itemText) {
    if (!handle || !itemText) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper setItem:itemIndex text:itemText];
}

void obsidian_macos_list_set_on_selection(ObsidianListHandle handle, 
                                         ObsidianListSelectionCallback callback,
                                         void* userData) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    [wrapper setOnSelection:callback userData:userData];
}

int obsidian_macos_list_get_selected_index(ObsidianListHandle handle) {
    if (!handle) {
        return -1;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper getSelectedIndex];
}

bool obsidian_macos_list_set_selected_index(ObsidianListHandle handle, int itemIndex) {
    if (!handle) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper setSelectedIndex:itemIndex];
}

void obsidian_macos_list_set_visible(ObsidianListHandle handle, bool visible) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    [wrapper setVisible:visible];
}

bool obsidian_macos_list_is_visible(ObsidianListHandle handle) {
    if (!handle) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper isVisible];
}

void obsidian_macos_list_set_enabled(ObsidianListHandle handle, bool enabled) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    [wrapper setEnabled:enabled];
}

bool obsidian_macos_list_is_enabled(ObsidianListHandle handle) {
    if (!handle) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return [wrapper isEnabled];
}

void obsidian_macos_list_add_to_window(ObsidianListHandle listHandle, void* windowHandle) {
    if (!listHandle || !windowHandle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)listHandle;
    [wrapper addToWindow:windowHandle];
}

void obsidian_macos_list_remove_from_parent(ObsidianListHandle handle) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    [wrapper removeFromParent];
}

void obsidian_macos_destroy_list(ObsidianListHandle handle) {
    if (!handle) {
        return;
    }
    
    ObsidianListWrapper* wrapper = (__bridge_transfer ObsidianListWrapper*)handle;
    [wrapper removeFromParent];
    // ARC will release the wrapper when it goes out of scope
}

bool obsidian_macos_list_is_valid(ObsidianListHandle handle) {
    if (!handle) {
        return false;
    }
    
    ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
    return wrapper != nil;
}

void* obsidian_macos_list_get_view(ObsidianListHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianListWrapper* wrapper = (__bridge ObsidianListWrapper*)handle;
        if (wrapper && wrapper.scrollView) {
            return (__bridge void*)wrapper.scrollView;
        }
    }
    return nullptr;
}
