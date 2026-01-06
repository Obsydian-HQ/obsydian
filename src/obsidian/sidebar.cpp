/**
 * Obsidian Public API - Sidebar Implementation
 * 
 * Based on real working apps (Watt editor, HelloAppKit):
 * 1. Create split view controller
 * 2. Set its view frame size BEFORE adding items
 * 3. Add items
 * 4. Add to window
 * That's it. No fighting auto layout, no 10000px workarounds.
 */

#include "obsidian/layout/sidebar.h"
#include "obsidian/window.h"
#include "obsidian/layout/vstack.h"
#include "obsidian/layout/hstack.h"
#include "obsidian/ui/list.h"
#include "obsidian/ui/button.h"
#include "obsidian/ui/scrollview.h"
#include "obsidian/navigation/router.h"
#include "obsidian/navigation/screen_container.h"

#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_splitviewcontroller.h"
#include "macos_splitviewitem.h"
#include "macos_viewcontroller.h"
#include "macos_window.h"
#endif

#include <functional>
#include <memory>

namespace obsidian {

class Sidebar::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::SplitViewController splitViewController;
    obsidian::ffi::macos::SplitViewItem sidebarItem;
    obsidian::ffi::macos::SplitViewItem mainContentItem;
    obsidian::ffi::macos::ViewController sidebarViewController;
    obsidian::ffi::macos::ViewController mainContentViewController;
    std::function<void(bool)> onSidebarToggleCallback;
    
    // Store callbacks to trigger layout updates after window is added
    // This is needed because VStack/HStack need to recalculate layout
    // when the view controller's container view gets sized by the split view
    std::function<void()> mainContentLayoutUpdateCallback;
    std::function<void()> sidebarContentLayoutUpdateCallback;
    
    // Store Router pointer if sidebar owns a router (for setMainContent(Router&))
    // We need to give the Router access to the Window when addToWindow is called
    Router* routerContent = nullptr;
#endif
    bool valid = false;
    double minSidebarWidth = 200.0;
    double maxSidebarWidth = 400.0;
    double windowWidth = 0;
    double windowHeight = 0;
};

Sidebar::Sidebar() : pImpl(std::make_unique<Impl>()) {}

Sidebar::~Sidebar() = default;

bool Sidebar::create() {
    if (pImpl->valid) return false;
    
#ifdef __APPLE__
    if (!pImpl->splitViewController.create()) return false;
    pImpl->valid = true;
    return true;
#else
    return false;
#endif
}

// Helper to set sidebar content (reduces code duplication)
void Sidebar::setSidebarContent(VStack& vstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !vstack.isValid()) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) return;
    }
    
    // Get the view controller's container view (this will be the parent for layout)
    void* vcContainerView = obsidian_macos_viewcontroller_get_view(pImpl->sidebarViewController.getHandle());
    if (!vcContainerView) return;
    
    // Add VStack to the view controller's container view (this sets up layout properly)
    // This integrates with the layout engine - VStack will create a layout node and calculate layout
    vstack.addToParentView(vcContainerView);
    
    // Store callback to trigger layout update after window is added
    pImpl->sidebarContentLayoutUpdateCallback = [&vstack]() {
        vstack.updateLayout();
    };
    
    // Configure view controller for sidebar (adds vibrancy effect)
    pImpl->sidebarViewController.configureForSidebar();
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) return;
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setSidebarContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) return;
    }
    
    // Get the view controller's container view (this will be the parent for layout)
    void* vcContainerView = obsidian_macos_viewcontroller_get_view(pImpl->sidebarViewController.getHandle());
    if (!vcContainerView) return;
    
    // Add HStack to the view controller's container view (this sets up layout properly)
    // This integrates with the layout engine - HStack will create a layout node and calculate layout
    hstack.addToParentView(vcContainerView);
    
    // Configure view controller for sidebar (adds vibrancy effect)
    pImpl->sidebarViewController.configureForSidebar();
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) return;
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
        
        // CRITICAL: After adding to split view, trigger layout update
        // The view controller's view now has proper bounds, so we need to recalculate layout
        hstack.updateLayout();
    }
#endif
}

void Sidebar::setSidebarContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) return;
    }
    
    // For List, we use setView directly (List doesn't use layout engine the same way)
    void* viewHandle = list.getNativeViewHandle();
    if (!viewHandle) return;
    
    pImpl->sidebarViewController.setView(viewHandle);
    pImpl->sidebarViewController.configureForSidebar();
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) return;
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setSidebarContent(Button& button) {
#ifdef __APPLE__
    if (!pImpl->valid || !button.isValid()) return;
    
    void* viewHandle = button.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) return;
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    pImpl->sidebarViewController.configureForSidebar();
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) return;
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setSidebarContent(ScrollView& scrollView) {
#ifdef __APPLE__
    if (!pImpl->valid || !scrollView.isValid()) return;
    
    void* viewHandle = scrollView.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) return;
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    pImpl->sidebarViewController.configureForSidebar();
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) return;
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(VStack& vstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !vstack.isValid()) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    // Get the view controller's container view (this will be the parent for layout)
    void* vcContainerView = obsidian_macos_viewcontroller_get_view(pImpl->mainContentViewController.getHandle());
    if (!vcContainerView) return;
    
    // Add VStack to the view controller's container view (this sets up layout properly)
    // This integrates with the layout engine - VStack will create a layout node and calculate layout
    // NOTE: At this point, the container view may have zero bounds, so layout will be calculated with w=0
    vstack.addToParentView(vcContainerView);
    
    // Store callback to trigger layout update after window is added
    // This is critical: the view controller's container view only gets sized
    // when the split view controller is added to the window and lays out
    pImpl->mainContentLayoutUpdateCallback = [&vstack]() {
        vstack.updateLayout();
    };
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    // Get the view controller's container view (this will be the parent for layout)
    void* vcContainerView = obsidian_macos_viewcontroller_get_view(pImpl->mainContentViewController.getHandle());
    if (!vcContainerView) return;
    
    // Add HStack to the view controller's container view (this sets up layout properly)
    // This integrates with the layout engine - HStack will create a layout node and calculate layout
    hstack.addToParentView(vcContainerView);
    
    // Store callback to trigger layout update after window is added
    pImpl->mainContentLayoutUpdateCallback = [&hstack]() {
        hstack.updateLayout();
    };
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
    // For List, we use setView directly (List doesn't use layout engine the same way)
    void* viewHandle = list.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(Button& button) {
#ifdef __APPLE__
    if (!pImpl->valid || !button.isValid()) return;
    
    // For Button, we use setView directly (Button doesn't use layout engine the same way)
    void* viewHandle = button.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(ScrollView& scrollView) {
#ifdef __APPLE__
    if (!pImpl->valid || !scrollView.isValid()) return;
    
    // For ScrollView, we use setView directly (ScrollView doesn't use layout engine the same way)
    void* viewHandle = scrollView.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(Router& router) {
#ifdef __APPLE__
    if (!pImpl->valid) return;
    
    // Store router pointer - we handle attachment in addToWindow()
    pImpl->routerContent = &router;
    
    // Create the main content view controller if needed
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) return;
    }
    
    // Create the split view item if needed
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) return;
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
    
    // Note: Router attachment happens in addToWindow() when we have the window reference
    // and the view hierarchy is established
#endif
}

void* Sidebar::getMainContentView() const {
#ifdef __APPLE__
    if (!pImpl->valid || !pImpl->mainContentViewController.isValid()) {
        return nullptr;
    }
    return obsidian_macos_viewcontroller_get_view(pImpl->mainContentViewController.getHandle());
#else
    return nullptr;
#endif
}

void Sidebar::setMinimumSidebarWidth(double width) {
    pImpl->minSidebarWidth = width;
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        pImpl->sidebarItem.setMinimumThickness(width);
    }
#endif
}

double Sidebar::getMinimumSidebarWidth() const {
    return pImpl->minSidebarWidth;
}

void Sidebar::setMaximumSidebarWidth(double width) {
    pImpl->maxSidebarWidth = width;
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        pImpl->sidebarItem.setMaximumThickness(width);
    }
#endif
}

double Sidebar::getMaximumSidebarWidth() const {
    return pImpl->maxSidebarWidth;
}

void Sidebar::collapseSidebar() {
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        pImpl->sidebarItem.setCollapsed(true);
        if (pImpl->onSidebarToggleCallback) {
            pImpl->onSidebarToggleCallback(true);
        }
    }
#endif
}

void Sidebar::expandSidebar() {
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        pImpl->sidebarItem.setCollapsed(false);
        if (pImpl->onSidebarToggleCallback) {
            pImpl->onSidebarToggleCallback(false);
        }
    }
#endif
}

void Sidebar::toggleSidebar() {
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        bool currentState = pImpl->sidebarItem.getCollapsed();
        pImpl->sidebarItem.setCollapsed(!currentState);
        if (pImpl->onSidebarToggleCallback) {
            pImpl->onSidebarToggleCallback(!currentState);
        }
    }
#endif
}

bool Sidebar::isSidebarCollapsed() const {
#ifdef __APPLE__
    if (pImpl->sidebarItem.isValid()) {
        return pImpl->sidebarItem.getCollapsed();
    }
#endif
    return false;
}

void Sidebar::addToWindow(Window& window) {
#ifdef __APPLE__
    if (!pImpl->valid || !window.isValid()) return;
    
    void* windowHandle = window.getNativeHandle();
    if (!windowHandle) return;
    
    // KEY INSIGHT from real apps (Watt editor):
    // Set the split view's frame size BEFORE adding to window
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        pImpl->splitViewController.setViewFrameSize(1200, 800);
    }
    
    ObsidianSplitViewControllerHandle splitViewControllerHandle = pImpl->splitViewController.getHandle();
    if (splitViewControllerHandle) {
        obsidian_macos_splitviewcontroller_add_to_window(splitViewControllerHandle, windowHandle);
        
        // CRITICAL: After adding to window, the split view controller sizes its child views
        // Now attach the router to the main content area if one was set
        if (pImpl->routerContent) {
            // Give router access to window for rendering
            pImpl->routerContent->setWindow(window);
            
            // Get the main content view and attach router to it
            void* mainContentView = getMainContentView();
            if (mainContentView) {
                pImpl->routerContent->attachToView(mainContentView);
            }
        }
        
        // Trigger layout updates for VStack/HStack content
        if (pImpl->mainContentLayoutUpdateCallback) {
            pImpl->mainContentLayoutUpdateCallback();
        }
        if (pImpl->sidebarContentLayoutUpdateCallback) {
            pImpl->sidebarContentLayoutUpdateCallback();
        }
    }
#endif
}

void Sidebar::removeFromParent() {
    pImpl->valid = false;
}

bool Sidebar::isValid() const {
    return pImpl->valid;
}

void Sidebar::setOnSidebarToggle(std::function<void(bool collapsed)> callback) {
    pImpl->onSidebarToggleCallback = callback;
}

Sidebar::Sidebar(Sidebar&& other) noexcept = default;
Sidebar& Sidebar::operator=(Sidebar&& other) noexcept = default;

} // namespace obsidian
