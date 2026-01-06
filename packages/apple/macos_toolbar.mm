#import "macos_toolbar.h"
#import <AppKit/AppKit.h>

@interface ObsidianToolbarWrapper : NSObject <NSToolbarDelegate>
@property (nonatomic, strong, readonly) NSToolbar* toolbar;
@property (nonatomic, strong, readonly) NSMutableArray<NSToolbarItemIdentifier>* itemIdentifiers;
@end

@implementation ObsidianToolbarWrapper {
    NSToolbar* _toolbar;
    NSMutableArray<NSToolbarItemIdentifier>* _itemIdentifiers;
}

- (instancetype)initWithParams:(ObsidianToolbarParams)params {
    self = [super init];
    if (self) {
        NSString* identifier = params.identifier 
            ? [NSString stringWithUTF8String:params.identifier] 
            : @"ObsidianMainToolbar";
        
        _toolbar = [[NSToolbar alloc] initWithIdentifier:identifier];
        _itemIdentifiers = [[NSMutableArray alloc] init];
        [_toolbar setDelegate:self];
        
        switch (params.displayMode) {
            case ObsidianToolbarDisplayModeIconAndLabel:
                [_toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel]; break;
            case ObsidianToolbarDisplayModeIconOnly:
                [_toolbar setDisplayMode:NSToolbarDisplayModeIconOnly]; break;
            case ObsidianToolbarDisplayModeLabelOnly:
                [_toolbar setDisplayMode:NSToolbarDisplayModeLabelOnly]; break;
            default:
                [_toolbar setDisplayMode:NSToolbarDisplayModeDefault]; break;
        }
        
        [_toolbar setAllowsUserCustomization:params.allowsUserCustomization];
        [_toolbar setAutosavesConfiguration:params.autosavesConfiguration];
    }
    return self;
}

- (void)insertSidebarToggleItemAtIndex:(int)index {
    if (!_toolbar || !_itemIdentifiers) return;
    
    // CRITICAL: To position the toggle button in the TOP RIGHT corner of the titlebar:
    // 1. NSToolbarFlexibleSpaceItemIdentifier (MUST come FIRST to push button to the right)
    // 2. NSToolbarToggleSidebarItemIdentifier (the collapse/expand button)
    // 3. NSToolbarSidebarTrackingSeparatorItemIdentifier (positions button in sidebar area)
    // The FlexibleSpace MUST come BEFORE the toggle button to push it to the right!
    
    NSUInteger insertIndex = (index >= 0 && index <= (int)[_itemIdentifiers count]) ? (NSUInteger)index : [_itemIdentifiers count];
    
    // 1. Insert flexible space FIRST - this pushes the toggle button to the right
    NSToolbarItemIdentifier flexibleSpaceId = NSToolbarFlexibleSpaceItemIdentifier;
    [_itemIdentifiers insertObject:flexibleSpaceId atIndex:insertIndex];
    
    // 2. Insert toggle button AFTER the flexible space
    NSToolbarItemIdentifier toggleId = NSToolbarToggleSidebarItemIdentifier;
    [_itemIdentifiers insertObject:toggleId atIndex:insertIndex + 1];
    
    // 3. Insert sidebar tracking separator after toggle button (macOS 11+)
    if (@available(macOS 11.0, *)) {
        NSToolbarItemIdentifier separatorId = NSToolbarSidebarTrackingSeparatorItemIdentifier;
        [_itemIdentifiers insertObject:separatorId atIndex:insertIndex + 2];
    }
    
    // Note: We don't need to call insertItemWithItemIdentifier: here because
    // the toolbar will use toolbarDefaultItemIdentifiers: to get the items
    // and will create them automatically when the toolbar is attached to the window.
    // The toolbar delegate handles this through toolbarDefaultItemIdentifiers:.
}

- (void)insertSidebarTrackingSeparatorAtIndex:(int)index {
    if (!_toolbar || !_itemIdentifiers) return;
    
    if (@available(macOS 11.0, *)) {
        NSToolbarItemIdentifier separatorId = NSToolbarSidebarTrackingSeparatorItemIdentifier;
        
        // Don't insert if already present (insertSidebarToggleItemAtIndex: adds this automatically)
        if ([_itemIdentifiers containsObject:separatorId]) return;
        
        NSUInteger insertIndex = (index >= 0 && index <= (int)[_itemIdentifiers count]) ? (NSUInteger)index : [_itemIdentifiers count];
        [_itemIdentifiers insertObject:separatorId atIndex:insertIndex];
    }
}

- (void)insertFlexibleSpaceAtIndex:(int)index {
    if (!_toolbar || !_itemIdentifiers) return;
    
    // Flexible space can be added multiple times (unlike toggle and separator)
    NSToolbarItemIdentifier flexibleSpaceId = NSToolbarFlexibleSpaceItemIdentifier;
    if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
        [_itemIdentifiers insertObject:flexibleSpaceId atIndex:index];
    } else {
        [_itemIdentifiers addObject:flexibleSpaceId];
    }
    // Don't insert directly - let the delegate handle it
}

- (bool)isValid {
    return _toolbar != nil;
}

#pragma mark - NSToolbarDelegate

- (NSArray<NSToolbarItemIdentifier>*)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar {
    NSMutableArray* allowed = [NSMutableArray arrayWithArray:_itemIdentifiers];
    [allowed addObject:NSToolbarToggleSidebarItemIdentifier];
    [allowed addObject:NSToolbarFlexibleSpaceItemIdentifier];
    [allowed addObject:NSToolbarSpaceItemIdentifier];
    if (@available(macOS 11.0, *)) {
        [allowed addObject:NSToolbarSidebarTrackingSeparatorItemIdentifier];
    }
    return allowed;
}

- (NSArray<NSToolbarItemIdentifier>*)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar {
    return _itemIdentifiers;
}

- (NSToolbarItem*)toolbar:(NSToolbar*)toolbar itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
    return nil;
}

@end

extern "C" {

ObsidianToolbarHandle obsidian_macos_create_toolbar(ObsidianToolbarParams params) {
    @autoreleasepool {
        ObsidianToolbarWrapper* w = [[ObsidianToolbarWrapper alloc] initWithParams:params];
        return w && [w isValid] ? (__bridge_retained void*)w : nullptr;
    }
}

void obsidian_macos_toolbar_set_display_mode(ObsidianToolbarHandle handle, ObsidianToolbarDisplayMode displayMode) {
    if (!handle) return;
    @autoreleasepool {
        ObsidianToolbarWrapper* w = (__bridge ObsidianToolbarWrapper*)handle;
        if (!w.toolbar) return;
        switch (displayMode) {
            case ObsidianToolbarDisplayModeIconAndLabel:
                [w.toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel]; break;
            case ObsidianToolbarDisplayModeIconOnly:
                [w.toolbar setDisplayMode:NSToolbarDisplayModeIconOnly]; break;
            case ObsidianToolbarDisplayModeLabelOnly:
                [w.toolbar setDisplayMode:NSToolbarDisplayModeLabelOnly]; break;
            default:
                [w.toolbar setDisplayMode:NSToolbarDisplayModeDefault]; break;
        }
    }
}

ObsidianToolbarDisplayMode obsidian_macos_toolbar_get_display_mode(ObsidianToolbarHandle handle) {
    if (!handle) return ObsidianToolbarDisplayModeDefault;
    @autoreleasepool {
        ObsidianToolbarWrapper* w = (__bridge ObsidianToolbarWrapper*)handle;
        if (!w.toolbar) return ObsidianToolbarDisplayModeDefault;
        switch ([w.toolbar displayMode]) {
            case NSToolbarDisplayModeIconAndLabel: return ObsidianToolbarDisplayModeIconAndLabel;
            case NSToolbarDisplayModeIconOnly: return ObsidianToolbarDisplayModeIconOnly;
            case NSToolbarDisplayModeLabelOnly: return ObsidianToolbarDisplayModeLabelOnly;
            default: return ObsidianToolbarDisplayModeDefault;
        }
    }
}

void obsidian_macos_toolbar_insert_sidebar_toggle_item(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianToolbarWrapper*)handle insertSidebarToggleItemAtIndex:index];
    }
}

void obsidian_macos_toolbar_insert_sidebar_tracking_separator(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianToolbarWrapper*)handle insertSidebarTrackingSeparatorAtIndex:index];
    }
}

void obsidian_macos_toolbar_insert_flexible_space(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    @autoreleasepool {
        [(__bridge ObsidianToolbarWrapper*)handle insertFlexibleSpaceAtIndex:index];
    }
}

int obsidian_macos_toolbar_get_item_count(ObsidianToolbarHandle handle) {
    if (!handle) return 0;
    @autoreleasepool {
        ObsidianToolbarWrapper* w = (__bridge ObsidianToolbarWrapper*)handle;
        return w.toolbar ? (int)[[w.toolbar items] count] : 0;
    }
}

bool obsidian_macos_toolbar_is_valid(ObsidianToolbarHandle handle) {
    if (!handle) return false;
    @autoreleasepool {
        return [(__bridge ObsidianToolbarWrapper*)handle isValid];
    }
}

void* obsidian_macos_toolbar_get_nstoolbar(ObsidianToolbarHandle handle) {
    if (!handle) return nullptr;
    @autoreleasepool {
        ObsidianToolbarWrapper* w = (__bridge ObsidianToolbarWrapper*)handle;
        return w.toolbar ? (__bridge void*)w.toolbar : nullptr;
    }
}

void obsidian_macos_destroy_toolbar(ObsidianToolbarHandle handle) {
    if (!handle) return;
    @autoreleasepool {
        (void)(__bridge_transfer ObsidianToolbarWrapper*)handle;
    }
}

}
