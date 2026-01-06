/**
 * Obsidian Public API - SplitView Implementation
 * 
 * This file implements the public SplitView API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/layout/splitview.h"
#include "obsidian/window.h"
#include "obsidian/layout/vstack.h"
#include "obsidian/layout/hstack.h"
#include "obsidian/ui/list.h"
#include "obsidian/ui/button.h"
#include "obsidian/ui/scrollview.h"

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
#include <iostream>

namespace obsidian {

class SplitView::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::SplitView macosSplitView;
    std::function<void(bool)> onPrimaryPaneToggleCallback;
#endif
    bool valid;
    SplitPosition position;
    
    Impl()
#ifdef __APPLE__
        : macosSplitView()
#endif
        , valid(false)
        , position(SplitPosition::Leading)
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

bool SplitView::create(SplitPosition position) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    // Convert SplitPosition to integer (0 = Leading, 1 = Trailing)
    int positionInt = (position == SplitPosition::Leading) ? 0 : 1;
    
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

void SplitView::setPrimaryPaneWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setPrimaryPaneWidth(width);
#endif
}

double SplitView::getPrimaryPaneWidth() const {
    if (!pImpl || !pImpl->valid) {
        return 0.0;
    }
    
#ifdef __APPLE__
    return pImpl->macosSplitView.getPrimaryPaneWidth();
#else
    return 0.0;
#endif
}

void SplitView::setMinimumPrimaryPaneWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setMinimumPrimaryPaneWidth(width);
#endif
}

void SplitView::setMaximumPrimaryPaneWidth(double width) {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.setMaximumPrimaryPaneWidth(width);
#endif
}

void SplitView::collapsePrimaryPane() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.collapsePrimaryPane();
    if (pImpl->onPrimaryPaneToggleCallback) {
        pImpl->onPrimaryPaneToggleCallback(true);
    }
#endif
}

void SplitView::expandPrimaryPane() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosSplitView.expandPrimaryPane();
    if (pImpl->onPrimaryPaneToggleCallback) {
        pImpl->onPrimaryPaneToggleCallback(false);
    }
#endif
}

void SplitView::togglePrimaryPane() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    bool wasCollapsed = pImpl->macosSplitView.isPrimaryPaneCollapsed();
    pImpl->macosSplitView.togglePrimaryPane();
    bool isCollapsed = pImpl->macosSplitView.isPrimaryPaneCollapsed();
    if (pImpl->onPrimaryPaneToggleCallback && wasCollapsed != isCollapsed) {
        pImpl->onPrimaryPaneToggleCallback(isCollapsed);
    }
#endif
}

bool SplitView::isPrimaryPaneCollapsed() const {
    if (!pImpl || !pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosSplitView.isPrimaryPaneCollapsed();
#else
    return false;
#endif
}

void SplitView::setPrimaryPaneContent(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* vstackView = vstack.getNativeViewHandle();
    if (vstackView) {
        pImpl->macosSplitView.setPrimaryPaneView(vstackView);
    }
#endif
}

void SplitView::setPrimaryPaneContent(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (hstackView) {
        pImpl->macosSplitView.setPrimaryPaneView(hstackView);
    }
#endif
}

void SplitView::setPrimaryPaneContent(List& list) {
    if (!pImpl || !pImpl->valid || !list.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* listView = list.getNativeViewHandle();
    if (listView) {
        pImpl->macosSplitView.setPrimaryPaneView(listView);
    }
#endif
}

void SplitView::setPrimaryPaneContent(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (buttonView) {
        pImpl->macosSplitView.setPrimaryPaneView(buttonView);
    }
#endif
}

void SplitView::setPrimaryPaneContent(ScrollView& scrollView) {
    if (!pImpl || !pImpl->valid || !scrollView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* scrollViewHandle = scrollView.getNativeViewHandle();
    if (scrollViewHandle) {
        pImpl->macosSplitView.setPrimaryPaneView(scrollViewHandle);
    }
#endif
}

void SplitView::setSecondaryPaneContent(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* vstackView = vstack.getNativeViewHandle();
    if (vstackView) {
        pImpl->macosSplitView.setSecondaryPaneView(vstackView);
    }
#endif
}

void SplitView::setSecondaryPaneContent(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (hstackView) {
        pImpl->macosSplitView.setSecondaryPaneView(hstackView);
    }
#endif
}

void SplitView::setSecondaryPaneContent(List& list) {
    if (!pImpl || !pImpl->valid || !list.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* listView = list.getNativeViewHandle();
    if (listView) {
        pImpl->macosSplitView.setSecondaryPaneView(listView);
    }
#endif
}

void SplitView::setSecondaryPaneContent(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (buttonView) {
        pImpl->macosSplitView.setSecondaryPaneView(buttonView);
    }
#endif
}

void SplitView::setSecondaryPaneContent(ScrollView& scrollView) {
    if (!pImpl || !pImpl->valid || !scrollView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* scrollViewHandle = scrollView.getNativeViewHandle();
    if (scrollViewHandle) {
        pImpl->macosSplitView.setSecondaryPaneView(scrollViewHandle);
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
        if (!create(SplitPosition::Leading)) {
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

void SplitView::setOnPrimaryPaneToggle(std::function<void(bool collapsed)> callback) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->onPrimaryPaneToggleCallback = callback;
#endif
}

SplitView::SplitView(SplitView&&) noexcept = default;
SplitView& SplitView::operator=(SplitView&&) noexcept = default;

} // namespace obsidian
