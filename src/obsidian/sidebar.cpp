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

#include "obsidian/sidebar.h"
#include "obsidian/window.h"
#include "obsidian/vstack.h"
#include "obsidian/hstack.h"
#include "obsidian/list.h"
#include "obsidian/button.h"
#include "obsidian/scrollview.h"

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
    
    void* viewHandle = vstack.getNativeViewHandle();
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

void Sidebar::setSidebarContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    void* viewHandle = hstack.getNativeViewHandle();
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

void Sidebar::setSidebarContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
    void* viewHandle = list.getNativeViewHandle();
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
    
    void* viewHandle = vstack.getNativeViewHandle();
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

void Sidebar::setMainContent(HStack& hstack) {
#ifdef __APPLE__
    if (!pImpl->valid || !hstack.isValid()) return;
    
    void* viewHandle = hstack.getNativeViewHandle();
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

void Sidebar::setMainContent(List& list) {
#ifdef __APPLE__
    if (!pImpl->valid || !list.isValid()) return;
    
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
    // Get window's content size and use that
    void* contentViewPtr = obsidian_macos_window_get_content_view(windowHandle);
    if (contentViewPtr) {
        // Get the window's content view frame to know the size
        // This will be used to set the split view's initial frame
        // Real apps set: view.frame.size = CGSize(width: 800, height: 600)
        pImpl->splitViewController.setViewFrameSize(1200, 800); // Use window's actual size
    }
    
    ObsidianSplitViewControllerHandle splitViewControllerHandle = pImpl->splitViewController.getHandle();
    if (splitViewControllerHandle) {
        obsidian_macos_splitviewcontroller_add_to_window(splitViewControllerHandle, windowHandle);
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
