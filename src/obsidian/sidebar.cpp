/**
 * Obsidian Public API - Sidebar Implementation
 * 
 * This file implements the public Sidebar API by wrapping the internal
 * platform-specific implementations using NSSplitViewController and
 * NSSplitViewItem.sidebar().
 */

#include "obsidian/sidebar.h"
#include "obsidian/window.h"
#include "obsidian/vstack.h"
#include "obsidian/hstack.h"
#include "obsidian/list.h"
#include "obsidian/button.h"
#include "obsidian/scrollview.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_splitviewcontroller.h"
#include "macos_splitviewitem.h"
#include "macos_viewcontroller.h"
#include "macos_window.h"     // For window content view
#endif

#include <functional>
#include <memory>
#include <iostream>

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
#endif
    bool valid;
    double minSidebarWidth;
    double maxSidebarWidth;
    
    Impl()
#ifdef __APPLE__
        : splitViewController()
        , sidebarItem()
        , mainContentItem()
        , sidebarViewController()
        , mainContentViewController()
#endif
        , valid(false)
        , minSidebarWidth(150.0)
        , maxSidebarWidth(400.0)
    {}
    
    ~Impl() {
        // No need to manually remove split view items - the split view controller
        // will automatically clean them up when it's deallocated
        // Just mark as invalid to prevent further operations
        valid = false;
    }
    
    void removeFromParent() {
#ifdef __APPLE__
        // For now, we don't manually remove split view items
        // The split view controller manages the lifecycle of its items
        // If explicit removal is needed in the future, we can implement it
        // but it requires careful handling to avoid crashes
        valid = false;
#endif
    }
};

Sidebar::Sidebar() : pImpl(std::make_unique<Impl>()) {}

Sidebar::~Sidebar() {
    if (pImpl && pImpl->valid) {
        pImpl->removeFromParent();
        pImpl->valid = false;
    }
}

bool Sidebar::create() {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    // Create the split view controller
    if (!pImpl->splitViewController.create()) {
        return false;
    }
    
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void Sidebar::setSidebarContent(VStack& vstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !vstack.isValid()) return;
    
    // Get the view handle from VStack
    void* viewHandle = vstack.getNativeViewHandle();
    if (!viewHandle) return;
    
    // Create or reuse sidebar view controller
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) {
            return;
        }
    }
    
    // Set the view for the view controller
    pImpl->sidebarViewController.setView(viewHandle);
    
    // Create sidebar item if not already created
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) {
            return;
        }
        
        // Set minimum and maximum widths
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        
        // Add to split view controller
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setSidebarContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    void* viewHandle = hstack.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) {
            return;
        }
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setSidebarContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
    void* viewHandle = list.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->sidebarViewController.isValid()) {
        if (!pImpl->sidebarViewController.create()) {
            return;
        }
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) {
            return;
        }
        
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
        if (!pImpl->sidebarViewController.create()) {
            return;
        }
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) {
            return;
        }
        
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
        if (!pImpl->sidebarViewController.create()) {
            return;
        }
    }
    
    pImpl->sidebarViewController.setView(viewHandle);
    
    if (!pImpl->sidebarItem.isValid()) {
        void* vcHandle = pImpl->sidebarViewController.getHandle();
        if (!pImpl->sidebarItem.createSidebarWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->sidebarItem.setMinimumThickness(pImpl->minSidebarWidth);
        pImpl->sidebarItem.setMaximumThickness(pImpl->maxSidebarWidth);
        
        pImpl->splitViewController.addSplitViewItem(pImpl->sidebarItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(VStack& vstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !vstack.isValid()) return;
    
    void* viewHandle = vstack.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) {
            return;
        }
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    void* viewHandle = hstack.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) {
            return;
        }
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
    void* viewHandle = list.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) {
            return;
        }
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(Button& button) {
#ifdef __APPLE__
    if (!pImpl->valid || !button.isValid()) return;
    
    void* viewHandle = button.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) {
            return;
        }
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
#endif
}

void Sidebar::setMainContent(ScrollView& scrollView) {
#ifdef __APPLE__
    if (!pImpl->valid || !scrollView.isValid()) return;
    
    void* viewHandle = scrollView.getNativeViewHandle();
    if (!viewHandle) return;
    
    if (!pImpl->mainContentViewController.isValid()) {
        if (!pImpl->mainContentViewController.create()) {
            return;
        }
    }
    
    pImpl->mainContentViewController.setView(viewHandle);
    
    if (!pImpl->mainContentItem.isValid()) {
        void* vcHandle = pImpl->mainContentViewController.getHandle();
        if (!pImpl->mainContentItem.createContentListWithViewController(vcHandle)) {
            return;
        }
        
        pImpl->splitViewController.addSplitViewItem(pImpl->mainContentItem.getHandle());
    }
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
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
    // Get the window handle
    void* windowHandle = window.getNativeHandle();
    if (!windowHandle) {
        return;
    }
    
    // Use the C FFI function to add the split view controller's view to the window
    ObsidianSplitViewControllerHandle splitViewControllerHandle = pImpl->splitViewController.getHandle();
    if (splitViewControllerHandle) {
        obsidian_macos_splitviewcontroller_add_to_window(splitViewControllerHandle, windowHandle);
    }
#endif
}

void Sidebar::removeFromParent() {
    pImpl->removeFromParent();
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
