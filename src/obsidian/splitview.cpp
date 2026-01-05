/**
 * Obsidian Public API - SplitView Implementation
 * 
 * This file implements the public SplitView API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/splitview.h"
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
#include "macos_splitview.h"  // For C FFI functions
#include "macos_window.h"     // For window content view
#include "macos_vstack.h"     // For getting VStack view handles
#include "macos_hstack.h"     // For getting HStack view handles
#include "macos_list.h"       // For getting List view handles
#include "macos_button.h"     // For getting Button view handles
#include "macos_scrollview.h" // For getting ScrollView view handles
#endif

#include <functional>
#include <memory>

namespace obsidian {

class SplitView::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::SplitView macosSplitView;
    std::function<void(bool)> onSidebarToggleCallback;
#endif
    bool valid;
    SidebarPosition position;
    
    Impl()
#ifdef __APPLE__
        : macosSplitView()
#endif
        , valid(false)
        , position(SidebarPosition::Leading)
    {}
    
    ~Impl() {
        if (valid) {
            removeFromParent();
        }
    }
    
    void removeFromParent() {
#ifdef __APPLE__
        if (valid) {
            macosSplitView.removeFromParent();
        }
#endif
    }
};

SplitView::SplitView() : pImpl(std::make_unique<Impl>()) {}

SplitView::~SplitView() {
    if (pImpl && pImpl->valid) {
        pImpl->removeFromParent();
#ifdef __APPLE__
        // The FFI SplitView destructor will handle cleanup
#endif
        pImpl->valid = false;
    }
}

bool SplitView::create(SidebarPosition position) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    // Convert SidebarPosition to integer (0 = Leading, 1 = Trailing)
    int positionInt = (position == SidebarPosition::Leading) ? 0 : 1;
    
    if (!pImpl->macosSplitView.create(positionInt, 200.0, 150.0, 400.0)) {
        return false;
    }
    
    pImpl->position = position;
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void SplitView::setSidebarWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setSidebarWidth(width);
#endif
}

double SplitView::getSidebarWidth() const {
    if (!pImpl || !pImpl->valid) {
        return 0.0;
    }
    
#ifdef __APPLE__
    return pImpl->macosSplitView.getSidebarWidth();
#else
    return 0.0;
#endif
}

void SplitView::setMinimumSidebarWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setMinimumSidebarWidth(width);
#endif
}

void SplitView::setMaximumSidebarWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setMaximumSidebarWidth(width);
#endif
}

void SplitView::collapseSidebar() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.collapseSidebar();
    if (pImpl->onSidebarToggleCallback) {
        pImpl->onSidebarToggleCallback(true);
    }
#endif
}

void SplitView::expandSidebar() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.expandSidebar();
    if (pImpl->onSidebarToggleCallback) {
        pImpl->onSidebarToggleCallback(false);
    }
#endif
}

void SplitView::toggleSidebar() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    bool wasCollapsed = pImpl->macosSplitView.isSidebarCollapsed();
    pImpl->macosSplitView.toggleSidebar();
    bool isCollapsed = pImpl->macosSplitView.isSidebarCollapsed();
    if (pImpl->onSidebarToggleCallback && wasCollapsed != isCollapsed) {
        pImpl->onSidebarToggleCallback(isCollapsed);
    }
#endif
}

bool SplitView::isSidebarCollapsed() const {
    if (!pImpl || !pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosSplitView.isSidebarCollapsed();
#else
    return false;
#endif
}

void SplitView::setSidebarContent(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* vstackView = vstack.getNativeViewHandle();
    if (vstackView) {
        pImpl->macosSplitView.setSidebarView(vstackView);
    }
#endif
}

void SplitView::setSidebarContent(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (hstackView) {
        pImpl->macosSplitView.setSidebarView(hstackView);
    }
#endif
}

void SplitView::setSidebarContent(List& list) {
    if (!pImpl || !pImpl->valid || !list.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* listView = list.getNativeViewHandle();
    if (listView) {
        pImpl->macosSplitView.setSidebarView(listView);
    }
#endif
}

void SplitView::setSidebarContent(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (buttonView) {
        pImpl->macosSplitView.setSidebarView(buttonView);
    }
#endif
}

void SplitView::setSidebarContent(ScrollView& scrollView) {
    if (!pImpl || !pImpl->valid || !scrollView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* scrollViewHandle = scrollView.getNativeViewHandle();
    if (scrollViewHandle) {
        pImpl->macosSplitView.setSidebarView(scrollViewHandle);
    }
#endif
}

void SplitView::setMainContent(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* vstackView = vstack.getNativeViewHandle();
    if (vstackView) {
        pImpl->macosSplitView.setMainView(vstackView);
    }
#endif
}

void SplitView::setMainContent(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (hstackView) {
        pImpl->macosSplitView.setMainView(hstackView);
    }
#endif
}

void SplitView::setMainContent(List& list) {
    if (!pImpl || !pImpl->valid || !list.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* listView = list.getNativeViewHandle();
    if (listView) {
        pImpl->macosSplitView.setMainView(listView);
    }
#endif
}

void SplitView::setMainContent(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (buttonView) {
        pImpl->macosSplitView.setMainView(buttonView);
    }
#endif
}

void SplitView::setMainContent(ScrollView& scrollView) {
    if (!pImpl || !pImpl->valid || !scrollView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* scrollViewHandle = scrollView.getNativeViewHandle();
    if (scrollViewHandle) {
        pImpl->macosSplitView.setMainView(scrollViewHandle);
    }
#endif
}

void SplitView::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        // Auto-create with default position if not already created
        if (!create(SidebarPosition::Leading)) {
            return;
        }
    }
    
    // Use the C FFI function directly with window handle
    void* windowHandle = window.getNativeHandle();
    if (windowHandle) {
        ObsidianSplitViewHandle splitViewHandle = pImpl->macosSplitView.getHandle();
        obsidian_macos_splitview_add_to_window(splitViewHandle, windowHandle);
    }
#endif
}

void SplitView::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->removeFromParent();
#endif
}

bool SplitView::isValid() const {
    return pImpl ? pImpl->valid : false;
}

void SplitView::setOnSidebarToggle(std::function<void(bool collapsed)> callback) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->onSidebarToggleCallback = callback;
#endif
}

SplitView::SplitView(SplitView&&) noexcept = default;
SplitView& SplitView::operator=(SplitView&&) noexcept = default;

} // namespace obsidian
