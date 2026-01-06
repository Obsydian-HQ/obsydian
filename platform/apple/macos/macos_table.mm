/**
 * macOS Table FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSTableView APIs
 * 
 * This implementation uses NSTableView wrapped in NSScrollView for scrolling.
 * The data source and delegate are implemented to provide a clean C++ interface.
 */

#import "macos_table.h"
#import "macos_window.h"
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <memory>
#import <vector>
#import <string>

// Internal data structure to store row data
struct ObsidianTableRowData {
    std::vector<std::string> columns;
};

// Internal list wrapper class
@interface ObsidianTableWrapper : NSObject <NSTableViewDataSource, NSTableViewDelegate> {
    NSTableView* _tableView;
    NSScrollView* _scrollView;
    std::vector<ObsidianTableRowData> _rowData;
    std::vector<std::string> _columnIdentifiers;
    std::vector<std::string> _columnTitles;
    ObsidianTableSelectionCallback _callback;
    void* _userData;
}

@property (nonatomic, strong) NSTableView* tableView;
@property (nonatomic, strong) NSScrollView* scrollView;

- (instancetype)initWithParams:(ObsidianTableParams)params;
- (bool)addColumn:(const char*)identifier title:(const char*)title width:(int)width;
- (int)addRow:(const char**)rowData columnCount:(int)columnCount;
- (bool)removeRow:(int)rowIndex;
- (void)clear;
- (int)getRowCount;
- (int)getColumnCount;
- (bool)setRowData:(int)rowIndex rowData:(const char**)rowData columnCount:(int)columnCount;
- (const char*)getRowData:(int)rowIndex columnIndex:(int)columnIndex;
- (void)setOnSelection:(ObsidianTableSelectionCallback)callback userData:(void*)userData;
- (int)getSelectedRow;
- (bool)setSelectedRow:(int)rowIndex;
- (void)setVisible:(bool)visible;
- (bool)isVisible;
- (void)setEnabled:(bool)enabled;
- (bool)isEnabled;
- (void)addToWindow:(void*)windowHandle;
- (void)removeFromParent;

// NSTableViewDataSource methods
- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView;
- (id)tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row;

// NSTableViewDelegate methods
- (void)tableViewSelectionDidChange:(NSNotification*)notification;

@end

@implementation ObsidianTableWrapper

- (instancetype)initWithParams:(ObsidianTableParams)params {
    self = [super init];
    if (self) {
        // Create NSTableView
        _tableView = [[NSTableView alloc] init];
        
        // Configure table view
        [_tableView setAllowsColumnReordering:YES];
        [_tableView setAllowsColumnResizing:YES];
        [_tableView setAllowsColumnSelection:NO];
        [_tableView setAllowsMultipleSelection:NO];
        [_tableView setUsesAlternatingRowBackgroundColors:YES];
        [_tableView setGridStyleMask:NSTableViewSolidVerticalGridLineMask | NSTableViewSolidHorizontalGridLineMask];
        
        // Set data source and delegate
        [_tableView setDataSource:self];
        [_tableView setDelegate:self];
        
        // Create NSScrollView to wrap the table view (for scrolling)
        NSRect frame = NSMakeRect(params.x, params.y, params.width, params.height);
        _scrollView = [[NSScrollView alloc] initWithFrame:frame];
        [_scrollView setHasVerticalScroller:YES];
        [_scrollView setHasHorizontalScroller:YES];
        [_scrollView setAutohidesScrollers:YES];
        [_scrollView setBorderType:NSBezelBorder];
        [_scrollView setDocumentView:_tableView];
        
        // The table view will automatically size based on its content
        // The scroll view will handle scrolling when content exceeds the visible area
        
        _callback = nullptr;
        _userData = nullptr;
    }
    return self;
}

- (bool)addColumn:(const char*)identifier title:(const char*)title width:(int)width {
    if (!_tableView || !identifier || !title) {
        return false;
    }
    
    // Create table column
    NSTableColumn* column = [[NSTableColumn alloc] initWithIdentifier:[NSString stringWithUTF8String:identifier]];
    [[column headerCell] setStringValue:[NSString stringWithUTF8String:title]];
    [column setWidth:width];
    [column setMinWidth:50];
    [column setResizingMask:NSTableColumnAutoresizingMask | NSTableColumnUserResizingMask];
    
    // Add column to table view
    [_tableView addTableColumn:column];
    
    // Store identifier and title
    _columnIdentifiers.push_back(std::string(identifier));
    _columnTitles.push_back(std::string(title));
    
    return true;
}

- (int)addRow:(const char**)rowData columnCount:(int)columnCount {
    if (!_tableView || !rowData || columnCount <= 0) {
        return -1;
    }
    
    // Verify column count matches
    if (columnCount != (int)_columnIdentifiers.size()) {
        return -1;
    }
    
    // Create row data
    ObsidianTableRowData row;
    for (int i = 0; i < columnCount; i++) {
        if (rowData[i]) {
            row.columns.push_back(std::string(rowData[i]));
        } else {
            row.columns.push_back(std::string(""));
        }
    }
    
    // Add row
    _rowData.push_back(row);
    int rowIndex = (int)(_rowData.size() - 1);
    
    // Reload table view to show new row
    [_tableView reloadData];
    
    return rowIndex;
}

- (bool)removeRow:(int)rowIndex {
    if (rowIndex < 0 || rowIndex >= (int)_rowData.size()) {
        return false;
    }
    
    _rowData.erase(_rowData.begin() + rowIndex);
    [_tableView reloadData];
    
    return true;
}

- (void)clear {
    _rowData.clear();
    [_tableView reloadData];
}

- (int)getRowCount {
    return (int)_rowData.size();
}

- (int)getColumnCount {
    return (int)_columnIdentifiers.size();
}

- (bool)setRowData:(int)rowIndex rowData:(const char**)rowData columnCount:(int)columnCount {
    if (rowIndex < 0 || rowIndex >= (int)_rowData.size() || !rowData || columnCount <= 0) {
        return false;
    }
    
    // Verify column count matches
    if (columnCount != (int)_columnIdentifiers.size()) {
        return false;
    }
    
    // Update row data
    _rowData[rowIndex].columns.clear();
    for (int i = 0; i < columnCount; i++) {
        if (rowData[i]) {
            _rowData[rowIndex].columns.push_back(std::string(rowData[i]));
        } else {
            _rowData[rowIndex].columns.push_back(std::string(""));
        }
    }
    
    // Reload the specific row
    NSIndexSet* indexSet = [NSIndexSet indexSetWithIndex:rowIndex];
    [_tableView reloadDataForRowIndexes:indexSet columnIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, columnCount)]];
    
    return true;
}

- (const char*)getRowData:(int)rowIndex columnIndex:(int)columnIndex {
    if (rowIndex < 0 || rowIndex >= (int)_rowData.size()) {
        return nullptr;
    }
    
    if (columnIndex < 0 || columnIndex >= (int)_rowData[rowIndex].columns.size()) {
        return nullptr;
    }
    
    // Return pointer to internal string data
    // Note: This is safe as long as the row data is not modified or removed
    // The caller should not free this pointer
    return _rowData[rowIndex].columns[columnIndex].c_str();
}

- (void)setOnSelection:(ObsidianTableSelectionCallback)callback userData:(void*)userData {
    _callback = callback;
    _userData = userData;
}

- (int)getSelectedRow {
    if (!_tableView) {
        return -1;
    }
    
    NSInteger selectedRow = [_tableView selectedRow];
    if (selectedRow == -1) {
        return -1;
    }
    
    return (int)selectedRow;
}

- (bool)setSelectedRow:(int)rowIndex {
    if (!_tableView) {
        return false;
    }
    
    if (rowIndex < 0 || rowIndex >= (int)_rowData.size()) {
        // Deselect all
        [_tableView deselectAll:nil];
        return true;
    }
    
    NSIndexSet* indexSet = [NSIndexSet indexSetWithIndex:rowIndex];
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
    return (NSInteger)_rowData.size();
}

- (id)tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row {
    if (row < 0 || row >= (NSInteger)_rowData.size()) {
        return nil;
    }
    
    NSString* identifier = [tableColumn identifier];
    if (!identifier) {
        return nil;
    }
    
    // Find the column index
    std::string identifierStr = [identifier UTF8String];
    int columnIndex = -1;
    for (size_t i = 0; i < _columnIdentifiers.size(); i++) {
        if (_columnIdentifiers[i] == identifierStr) {
            columnIndex = (int)i;
            break;
        }
    }
    
    if (columnIndex < 0 || columnIndex >= (int)_rowData[row].columns.size()) {
        return nil;
    }
    
    // Return the string value for this cell
    return [NSString stringWithUTF8String:_rowData[row].columns[columnIndex].c_str()];
}

// NSTableViewDelegate methods
- (void)tableViewSelectionDidChange:(NSNotification*)notification {
    if (_callback) {
        NSInteger selectedRow = [_tableView selectedRow];
        if (selectedRow >= 0) {
            _callback((int)selectedRow, _userData);
        }
    }
}

- (NSTableView*)tableView {
    return _tableView;
}

- (NSScrollView*)scrollView {
    return _scrollView;
}

@end

// C interface implementation
extern "C" {

ObsidianTableHandle obsidian_macos_create_table(ObsidianTableParams params) {
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = [[ObsidianTableWrapper alloc] initWithParams:params];
        if (wrapper && wrapper.tableView && wrapper.scrollView) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

bool obsidian_macos_table_add_column(ObsidianTableHandle handle, const char* identifier, const char* title, int width) {
    if (!handle || !identifier || !title) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper addColumn:identifier title:title width:width];
    }
}

int obsidian_macos_table_add_row(ObsidianTableHandle handle, const char** rowData, int columnCount) {
    if (!handle || !rowData || columnCount <= 0) {
        return -1;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper addRow:rowData columnCount:columnCount];
    }
}

bool obsidian_macos_table_remove_row(ObsidianTableHandle handle, int rowIndex) {
    if (!handle) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper removeRow:rowIndex];
    }
}

void obsidian_macos_table_clear(ObsidianTableHandle handle) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        [wrapper clear];
    }
}

int obsidian_macos_table_get_row_count(ObsidianTableHandle handle) {
    if (!handle) {
        return 0;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper getRowCount];
    }
}

int obsidian_macos_table_get_column_count(ObsidianTableHandle handle) {
    if (!handle) {
        return 0;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper getColumnCount];
    }
}

bool obsidian_macos_table_set_row_data(ObsidianTableHandle handle, int rowIndex, const char** rowData, int columnCount) {
    if (!handle || !rowData || columnCount <= 0) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper setRowData:rowIndex rowData:rowData columnCount:columnCount];
    }
}

const char* obsidian_macos_table_get_row_data(ObsidianTableHandle handle, int rowIndex, int columnIndex) {
    if (!handle) {
        return nullptr;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper getRowData:rowIndex columnIndex:columnIndex];
    }
}

void obsidian_macos_table_set_on_selection(ObsidianTableHandle handle,
                                          ObsidianTableSelectionCallback callback,
                                          void* userData) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        [wrapper setOnSelection:callback userData:userData];
    }
}

int obsidian_macos_table_get_selected_row(ObsidianTableHandle handle) {
    if (!handle) {
        return -1;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper getSelectedRow];
    }
}

bool obsidian_macos_table_set_selected_row(ObsidianTableHandle handle, int rowIndex) {
    if (!handle) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper setSelectedRow:rowIndex];
    }
}

void obsidian_macos_table_set_visible(ObsidianTableHandle handle, bool visible) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        [wrapper setVisible:visible];
    }
}

bool obsidian_macos_table_is_visible(ObsidianTableHandle handle) {
    if (!handle) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper isVisible];
    }
}

void obsidian_macos_table_set_enabled(ObsidianTableHandle handle, bool enabled) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        [wrapper setEnabled:enabled];
    }
}

bool obsidian_macos_table_is_enabled(ObsidianTableHandle handle) {
    if (!handle) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return [wrapper isEnabled];
    }
}

void obsidian_macos_table_add_to_window(ObsidianTableHandle listHandle, void* windowHandle) {
    if (!listHandle || !windowHandle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* listWrapper = (__bridge ObsidianTableWrapper*)listHandle;
        [listWrapper addToWindow:windowHandle];
    }
}

void obsidian_macos_table_remove_from_parent(ObsidianTableHandle handle) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        [wrapper removeFromParent];
    }
}

void obsidian_macos_destroy_table(ObsidianTableHandle handle) {
    if (!handle) {
        return;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge_transfer ObsidianTableWrapper*)handle;
        
        // Remove from parent first (clears data source/delegate, removes from view hierarchy)
        [wrapper removeFromParent];
        
        // Clear the table view and scroll view references
        wrapper.tableView = nil;
        wrapper.scrollView = nil;
        
        // ARC will handle cleanup of the wrapper when it goes out of scope
    }
}

bool obsidian_macos_table_is_valid(ObsidianTableHandle handle) {
    if (!handle) {
        return false;
    }
    
    @autoreleasepool {
        ObsidianTableWrapper* wrapper = (__bridge ObsidianTableWrapper*)handle;
        return wrapper != nil && wrapper.tableView != nil && wrapper.scrollView != nil;
    }
}

} // extern "C"

