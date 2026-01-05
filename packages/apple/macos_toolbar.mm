/**
 * macOS Toolbar FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSToolbar APIs.
 * 
 * The toolbar is REQUIRED for native sidebar collapse button functionality.
 * NSToolbarToggleSidebarItemIdentifier provides the native collapse/expand button.
 */

#import "macos_toolbar.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

// Internal toolbar wrapper class
@interface ObsidianToolbarWrapper : NSObject <NSToolbarDelegate> {
    NSToolbar* _toolbar;
    NSMutableArray<NSToolbarItemIdentifier>* _itemIdentifiers;
}

@property (nonatomic, strong, readonly) NSToolbar* toolbar;
@property (nonatomic, strong, readonly) NSMutableArray<NSToolbarItemIdentifier>* itemIdentifiers;

- (instancetype)initWithParams:(ObsidianToolbarParams)params;
- (void)insertSidebarToggleItemAtIndex:(int)index;
- (void)insertSidebarTrackingSeparatorAtIndex:(int)index;
- (void)insertFlexibleSpaceAtIndex:(int)index;
- (bool)isValid;

@end

@implementation ObsidianToolbarWrapper

- (instancetype)initWithParams:(ObsidianToolbarParams)params {
    self = [super init];
    if (self) {
        NSString* identifier = params.identifier 
            ? [NSString stringWithUTF8String:params.identifier] 
            : @"ObsidianMainToolbar";
        
        _toolbar = [[NSToolbar alloc] initWithIdentifier:identifier];
        _itemIdentifiers = [[NSMutableArray alloc] init];
        
        // Set the delegate to self
        [_toolbar setDelegate:self];
        
        // Configure display mode
        switch (params.displayMode) {
            case ObsidianToolbarDisplayModeIconAndLabel:
                [_toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
                break;
            case ObsidianToolbarDisplayModeIconOnly:
                [_toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
                break;
            case ObsidianToolbarDisplayModeLabelOnly:
                [_toolbar setDisplayMode:NSToolbarDisplayModeLabelOnly];
                break;
            case ObsidianToolbarDisplayModeDefault:
            default:
                [_toolbar setDisplayMode:NSToolbarDisplayModeDefault];
                break;
        }
        
        // Configure customization
        [_toolbar setAllowsUserCustomization:params.allowsUserCustomization];
        [_toolbar setAutosavesConfiguration:params.autosavesConfiguration];
        
        NSLog(@"DEBUG: Toolbar created with identifier: %@", identifier);
    }
    return self;
}

- (void)insertSidebarToggleItemAtIndex:(int)index {
    if (_toolbar && _itemIdentifiers) {
        NSToolbarItemIdentifier toggleId = NSToolbarToggleSidebarItemIdentifier;
        
        // Add to our identifier array
        if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
            [_itemIdentifiers insertObject:toggleId atIndex:index];
        } else {
            [_itemIdentifiers addObject:toggleId];
        }
        
        // Force toolbar to reload items
        [_toolbar insertItemWithItemIdentifier:toggleId atIndex:index];
        
        NSLog(@"DEBUG: Inserted sidebar toggle item at index %d", index);
    }
}

- (void)insertSidebarTrackingSeparatorAtIndex:(int)index {
    if (_toolbar && _itemIdentifiers) {
        // NSToolbarSidebarTrackingSeparatorItemIdentifier is available on macOS 11+
        if (@available(macOS 11.0, *)) {
            NSToolbarItemIdentifier separatorId = NSToolbarSidebarTrackingSeparatorItemIdentifier;
            
            // Add to our identifier array
            if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
                [_itemIdentifiers insertObject:separatorId atIndex:index];
            } else {
                [_itemIdentifiers addObject:separatorId];
            }
            
            // Force toolbar to reload items
            [_toolbar insertItemWithItemIdentifier:separatorId atIndex:index];
            
            NSLog(@"DEBUG: Inserted sidebar tracking separator at index %d", index);
        } else {
            NSLog(@"WARNING: NSToolbarSidebarTrackingSeparatorItemIdentifier requires macOS 11.0+");
        }
    }
}

- (void)insertFlexibleSpaceAtIndex:(int)index {
    if (_toolbar && _itemIdentifiers) {
        NSToolbarItemIdentifier flexibleSpaceId = NSToolbarFlexibleSpaceItemIdentifier;
        
        // Add to our identifier array
        if (index >= 0 && index <= (int)[_itemIdentifiers count]) {
            [_itemIdentifiers insertObject:flexibleSpaceId atIndex:index];
        } else {
            [_itemIdentifiers addObject:flexibleSpaceId];
        }
        
        // Force toolbar to reload items
        [_toolbar insertItemWithItemIdentifier:flexibleSpaceId atIndex:index];
        
        NSLog(@"DEBUG: Inserted flexible space at index %d", index);
    }
}

- (bool)isValid {
    return _toolbar != nil;
}

#pragma mark - NSToolbarDelegate

- (NSArray<NSToolbarItemIdentifier>*)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar {
    // Return all standard identifiers plus our custom ones
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
    // Return nil for system-provided items - AppKit handles them automatically
    // NSToolbarToggleSidebarItemIdentifier, NSToolbarFlexibleSpaceItemIdentifier, etc.
    // are all system-provided and don't need custom creation
    return nil;
}

@end

// C interface implementation
extern "C" {

ObsidianToolbarHandle obsidian_macos_create_toolbar(ObsidianToolbarParams params) {
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = [[ObsidianToolbarWrapper alloc] initWithParams:params];
        if (wrapper && [wrapper isValid]) {
            // Retain the wrapper and return as opaque handle
            return (__bridge_retained void*)wrapper;
        }
        return nullptr;
    }
}

void obsidian_macos_toolbar_set_display_mode(ObsidianToolbarHandle handle, ObsidianToolbarDisplayMode displayMode) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        if (wrapper && wrapper.toolbar) {
            switch (displayMode) {
                case ObsidianToolbarDisplayModeIconAndLabel:
                    [wrapper.toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
                    break;
                case ObsidianToolbarDisplayModeIconOnly:
                    [wrapper.toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
                    break;
                case ObsidianToolbarDisplayModeLabelOnly:
                    [wrapper.toolbar setDisplayMode:NSToolbarDisplayModeLabelOnly];
                    break;
                case ObsidianToolbarDisplayModeDefault:
                default:
                    [wrapper.toolbar setDisplayMode:NSToolbarDisplayModeDefault];
                    break;
            }
        }
    }
}

ObsidianToolbarDisplayMode obsidian_macos_toolbar_get_display_mode(ObsidianToolbarHandle handle) {
    if (!handle) return ObsidianToolbarDisplayModeDefault;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        if (wrapper && wrapper.toolbar) {
            switch ([wrapper.toolbar displayMode]) {
                case NSToolbarDisplayModeIconAndLabel:
                    return ObsidianToolbarDisplayModeIconAndLabel;
                case NSToolbarDisplayModeIconOnly:
                    return ObsidianToolbarDisplayModeIconOnly;
                case NSToolbarDisplayModeLabelOnly:
                    return ObsidianToolbarDisplayModeLabelOnly;
                case NSToolbarDisplayModeDefault:
                default:
                    return ObsidianToolbarDisplayModeDefault;
            }
        }
        return ObsidianToolbarDisplayModeDefault;
    }
}

void obsidian_macos_toolbar_insert_sidebar_toggle_item(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        [wrapper insertSidebarToggleItemAtIndex:index];
    }
}

void obsidian_macos_toolbar_insert_sidebar_tracking_separator(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        [wrapper insertSidebarTrackingSeparatorAtIndex:index];
    }
}

void obsidian_macos_toolbar_insert_flexible_space(ObsidianToolbarHandle handle, int index) {
    if (!handle) return;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        [wrapper insertFlexibleSpaceAtIndex:index];
    }
}

int obsidian_macos_toolbar_get_item_count(ObsidianToolbarHandle handle) {
    if (!handle) return 0;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        if (wrapper && wrapper.toolbar) {
            return (int)[[wrapper.toolbar items] count];
        }
        return 0;
    }
}

bool obsidian_macos_toolbar_is_valid(ObsidianToolbarHandle handle) {
    if (!handle) return false;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        return [wrapper isValid];
    }
}

void* obsidian_macos_toolbar_get_nstoolbar(ObsidianToolbarHandle handle) {
    if (!handle) return nullptr;
    
    @autoreleasepool {
        ObsidianToolbarWrapper* wrapper = (__bridge ObsidianToolbarWrapper*)handle;
        if (wrapper && wrapper.toolbar) {
            return (__bridge void*)wrapper.toolbar;
        }
        return nullptr;
    }
}

void obsidian_macos_destroy_toolbar(ObsidianToolbarHandle handle) {
    if (!handle) return;
    
    @autoreleasepool {
        // Release the wrapper (ARC will handle cleanup)
        (void)(__bridge_transfer ObsidianToolbarWrapper*)handle;
    }
}

} // extern "C"
