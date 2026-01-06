/**
 * macOS Mounting Implementation
 * 
 * REFACTORED: Following React Native's pattern where handles ARE the views.
 * No wrapper indirection - factories return NSView* directly.
 * 
 * Registers view factories for all component types and implements
 * the platform delegate for mounting operations.
 */

#import "macos_mounting.h"
#import "macos_button.h"
#import "macos_textview.h"
#import "macos_textfield.h"
#import "macos_vstack.h"
#import "macos_hstack.h"
#import "macos_zstack.h"
#import "macos_spacer.h"
#import "macos_scrollview.h"
#import "macos_screen_container.h"
#import <AppKit/AppKit.h>
#import <iostream>

#include "../../core/mounting/component_view_registry.h"
#include "../../core/mounting/mounting_coordinator.h"

using namespace obsidian::mounting;

#pragma mark - View Factory Functions

// Button: handle IS the NSButton (no wrapper)
static void* createButtonView(const ComponentHandle&) {
    @autoreleasepool {
        ObsidianButtonParams params = {};
        params.title = "";
        params.x = 0;
        params.y = 0;
        params.width = 100;
        params.height = 30;
        
        // Returns NSButton* directly
        return obsidian_macos_create_button(params);
    }
}

static void destroyButtonView(void* view, const ComponentHandle&) {
    if (view) {
        obsidian_macos_destroy_button(view);
    }
}

// TextView: for now uses wrapper pattern (to be refactored)
static void* createTextView(const ComponentHandle&) {
    @autoreleasepool {
        ObsidianTextViewParams params = {0, 0, 100, 24};
        ObsidianTextViewHandle handle = obsidian_macos_create_textview(params);
        // TODO: Refactor textview to return NSView directly like button
        // For now, get the view from handle
        return obsidian_macos_textview_get_view_handle(handle);
    }
}

static void destroyTextView(void* view, const ComponentHandle&) {
    // Note: This doesn't work correctly with current wrapper pattern
    // TODO: Refactor textview to not use wrapper
    (void)view;
}

// VStack: uses wrapper pattern (to be refactored)
static void* createVStackView(const ComponentHandle&) {
    @autoreleasepool {
        ObsidianVStackParams params = {0};
        ObsidianVStackHandle handle = obsidian_macos_create_vstack(params);
        return obsidian_macos_vstack_get_view(handle);
    }
}

static void destroyVStackView(void* view, const ComponentHandle&) {
    (void)view;
    // TODO: Proper cleanup
}

// HStack: uses wrapper pattern (to be refactored)
static void* createHStackView(const ComponentHandle&) {
    @autoreleasepool {
        ObsidianHStackParams params = {0};
        ObsidianHStackHandle handle = obsidian_macos_create_hstack(params);
        return obsidian_macos_hstack_get_view(handle);
    }
}

static void destroyHStackView(void* view, const ComponentHandle&) {
    (void)view;
    // TODO: Proper cleanup
}

// Spacer
static void* createSpacerView(const ComponentHandle&) {
    @autoreleasepool {
        ObsidianSpacerParams params = {0, 0, CGFLOAT_MAX, CGFLOAT_MAX};
        ObsidianSpacerHandle handle = obsidian_macos_create_spacer(params);
        return obsidian_macos_spacer_get_view(handle);
    }
}

static void destroySpacerView(void* view, const ComponentHandle&) {
    (void)view;
    // TODO: Proper cleanup
}

#pragma mark - Platform Delegate Implementation

static void mountChildView(void* parentView, void* childView, int32_t index) {
    @autoreleasepool {
        NSView* parent = (__bridge NSView*)parentView;
        NSView* child = (__bridge NSView*)childView;
        
        if (parent && child) {
            if (index >= 0 && index < (int32_t)parent.subviews.count) {
                NSView* referenceView = parent.subviews[index];
                [parent addSubview:child positioned:NSWindowBelow relativeTo:referenceView];
            } else {
                [parent addSubview:child];
            }
        }
    }
}

static void unmountChildView(void* parentView, void* childView, int32_t index) {
    @autoreleasepool {
        (void)parentView;
        (void)index;
        
        NSView* child = (__bridge NSView*)childView;
        if (child && child.superview) {
            [child removeFromSuperview];
        }
    }
}

static void updateLayoutMetrics(void* view, const LayoutMetrics& metrics) {
    @autoreleasepool {
        NSView* nsView = (__bridge NSView*)view;
        if (nsView) {
            nsView.frame = NSMakeRect(metrics.x, metrics.y, metrics.width, metrics.height);
        }
    }
}

static void prepareForRecycle(void* view) {
    @autoreleasepool {
        NSView* nsView = (__bridge NSView*)view;
        if (nsView) {
            if (nsView.superview) {
                [nsView removeFromSuperview];
            }
            nsView.frame = NSZeroRect;
            for (NSView* subview in [nsView.subviews copy]) {
                [subview removeFromSuperview];
            }
        }
    }
}

#pragma mark - Public API

void obsidian_macos_mounting_initialize(void) {
    auto& registry = ComponentViewRegistry::getInstance();
    
    // Register view factories - all return NSView* directly
    registry.registerViewFactory(
        ComponentHandles::Button,
        createButtonView,
        destroyButtonView,
        [](void* view, const ComponentHandle&) { prepareForRecycle(view); }
    );
    
    registry.registerViewFactory(
        ComponentHandles::TextView,
        createTextView,
        destroyTextView,
        [](void* view, const ComponentHandle&) { prepareForRecycle(view); }
    );
    
    registry.registerViewFactory(
        ComponentHandles::VStack,
        createVStackView,
        destroyVStackView,
        [](void* view, const ComponentHandle&) { prepareForRecycle(view); }
    );
    
    registry.registerViewFactory(
        ComponentHandles::HStack,
        createHStackView,
        destroyHStackView,
        [](void* view, const ComponentHandle&) { prepareForRecycle(view); }
    );
    
    registry.registerViewFactory(
        ComponentHandles::Spacer,
        createSpacerView,
        destroySpacerView,
        [](void* view, const ComponentHandle&) { prepareForRecycle(view); }
    );
    
    // Set up platform delegate
    MountingPlatformDelegate delegate;
    delegate.mountChildView = mountChildView;
    delegate.unmountChildView = unmountChildView;
    delegate.updateLayoutMetrics = updateLayoutMetrics;
    delegate.prepareForRecycle = prepareForRecycle;
    
    MountingCoordinator::getInstance().setPlatformDelegate(delegate);
    
    std::cout << "[Mounting] macOS mounting system initialized" << std::endl;
}

void obsidian_macos_mounting_shutdown(void) {
    auto& registry = ComponentViewRegistry::getInstance();
    registry.clearRecyclePool();
    
    std::cout << "[Mounting] macOS mounting system shutdown" << std::endl;
}
