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
    
    NSToolbarItemIdentifier toggleId = NSToolbarToggleSidebarItemIdentifier;
    if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
        [_itemIdentifiers insertObject:toggleId atIndex:index];
    } else {
        [_itemIdentifiers addObject:toggleId];
    }
    [_toolbar insertItemWithItemIdentifier:toggleId atIndex:index];
}

- (void)insertSidebarTrackingSeparatorAtIndex:(int)index {
    if (!_toolbar || !_itemIdentifiers) return;
    
    if (@available(macOS 11.0, *)) {
        NSToolbarItemIdentifier separatorId = NSToolbarSidebarTrackingSeparatorItemIdentifier;
        if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
            [_itemIdentifiers insertObject:separatorId atIndex:index];
        } else {
            [_itemIdentifiers addObject:separatorId];
        }
        [_toolbar insertItemWithItemIdentifier:separatorId atIndex:index];
    }
}

- (void)insertFlexibleSpaceAtIndex:(int)index {
    if (!_toolbar || !_itemIdentifiers) return;
    
    NSToolbarItemIdentifier flexibleSpaceId = NSToolbarFlexibleSpaceItemIdentifier;
    if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
        [_itemIdentifiers insertObject:flexibleSpaceId atIndex:index];
    } else {
        [_itemIdentifiers addObject:flexibleSpaceId];
    }
    [_toolbar insertItemWithItemIdentifier:flexibleSpaceId atIndex:index];
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
