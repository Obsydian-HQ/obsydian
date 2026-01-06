/**
 * Obsidian Public API - ZStack Implementation
 * 
 * Frame-based overlay container. The native Objective-C++ implementation
 * handles all layout in layoutSubviews - NO AUTO LAYOUT CONSTRAINTS.
 */

#include "obsidian/layout/zstack.h"
#include "obsidian/window.h"
#include "obsidian/ui/button.h"
#include "obsidian/ui/link.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#include "macos_window.h"
#include "macos_zstack.h"
#endif

#include <vector>
#include <algorithm>
#include <memory>

namespace obsidian {

class ZStack::Impl {
public:
#ifdef __APPLE__
    ObsidianZStackHandle zstackHandle;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    layout::Alignment alignment;
    
    Impl()
#ifdef __APPLE__
        : zstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , alignment(layout::Alignment::Center)
    {}
    
    ~Impl() = default;
};

ZStack::ZStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view
    ObsidianZStackParams params;
    pImpl->zstackHandle = obsidian_macos_create_zstack(params);
    if (pImpl->zstackHandle) {
        pImpl->valid = true;
    }
#endif
}

ZStack::~ZStack() {
    if (pImpl && pImpl->valid) {
        removeFromParent();
#ifdef __APPLE__
        if (pImpl->zstackHandle) {
            obsidian_macos_destroy_zstack(pImpl->zstackHandle);
            pImpl->zstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void ZStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        // Note: ZStack alignment only affects positioning within parent
        // Children always fill the ZStack bounds in frame-based layout
    }
}

layout::Alignment ZStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Center;
}

void ZStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianZStackParams params;
        pImpl->zstackHandle = obsidian_macos_create_zstack(params);
        if (!pImpl->zstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    button.removeFromParent();
    
    // Add child - native side handles layout in layoutSubviews
    obsidian_macos_zstack_add_child_view(pImpl->zstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
#endif
}

void ZStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianZStackParams params;
        pImpl->zstackHandle = obsidian_macos_create_zstack(params);
        if (!pImpl->zstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    link.removeFromParent();
    
    obsidian_macos_zstack_add_child_view(pImpl->zstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
#endif
}

void ZStack::removeChild(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), buttonView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_zstack_remove_child_view(pImpl->zstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void ZStack::removeChild(Link& link) {
    if (!pImpl || !pImpl->valid || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), linkView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_zstack_remove_child_view(pImpl->zstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void ZStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_zstack_remove_child_view(pImpl->zstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
#endif
}

void ZStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianZStackParams params;
        pImpl->zstackHandle = obsidian_macos_create_zstack(params);
        if (!pImpl->zstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add ZStack to window - native side fills content view
    obsidian_macos_zstack_add_to_window(pImpl->zstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Force layout update
    obsidian_macos_zstack_request_layout_update(pImpl->zstackHandle);
#endif
}

void ZStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    obsidian_macos_zstack_remove_from_parent(pImpl->zstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void ZStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Just trigger native layout update - all positioning done in native layoutSubviews
    obsidian_macos_zstack_request_layout_update(pImpl->zstackHandle);
#endif
}

bool ZStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

ZStack::ZStack(ZStack&&) noexcept = default;
ZStack& ZStack::operator=(ZStack&&) noexcept = default;

} // namespace obsidian
