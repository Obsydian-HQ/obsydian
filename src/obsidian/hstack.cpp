/**
 * Obsidian Public API - HStack Implementation
 * 
 * Frame-based layout container. The native Objective-C++ implementation
 * handles all layout in layoutSubviews - NO AUTO LAYOUT CONSTRAINTS.
 */

#include "obsidian/hstack.h"
#include "obsidian/window.h"
#include "obsidian/screen_container.h"
#include "obsidian/button.h"
#include "obsidian/link.h"
#include "obsidian/spacer.h"
#include "obsidian/textview.h"
#include "obsidian/vstack.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#include "macos_window.h"
#include "macos_hstack.h"
#include "macos_screen_container.h"
#endif

#include <vector>
#include <algorithm>
#include <memory>

namespace obsidian {

class HStack::Impl {
public:
#ifdef __APPLE__
    ObsidianHStackHandle hstackHandle;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    double spacing;
    layout::Alignment alignment;
    Padding padding;
    
    Impl()
#ifdef __APPLE__
        : hstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Top)
        , padding{0.0, 0.0, 0.0, 0.0}
    {}
    
    ~Impl() = default;
};

HStack::HStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view
    ObsidianHStackParams params;
    pImpl->hstackHandle = obsidian_macos_create_hstack(params);
    if (pImpl->hstackHandle) {
        pImpl->valid = true;
    }
#endif
}

HStack::~HStack() {
    if (pImpl && pImpl->valid) {
#ifdef __APPLE__
        // Release handle but do NOT remove from parent
        if (pImpl->hstackHandle) {
            obsidian_macos_release_hstack_handle(pImpl->hstackHandle);
            pImpl->hstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void HStack::setSpacing(double spacing) {
    if (pImpl) {
        pImpl->spacing = spacing;
        if (pImpl->valid) {
#ifdef __APPLE__
            obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, spacing);
            obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
        }
    }
}

double HStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void HStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
    }
}

layout::Alignment HStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Top;
}

void HStack::setPadding(const Padding& padding) {
    if (pImpl) {
        pImpl->padding = padding;
        if (pImpl->valid) {
#ifdef __APPLE__
            obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                              padding.top, padding.bottom,
                                              padding.leading, padding.trailing);
            obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
        }
    }
}

Padding HStack::getPadding() const {
    return pImpl ? pImpl->padding : Padding{0.0, 0.0, 0.0, 0.0};
}

void HStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    button.removeFromParent();
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
#endif
}

void HStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
#endif
}

void HStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    link.removeFromParent();
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
#endif
}

void HStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    textView.removeFromParent();
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
#endif
}

void HStack::addChild(VStack& vstack) {
    if (!pImpl || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    vstack.removeFromParent();
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
#endif
}

void HStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = hstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    hstack.removeFromParent();
    
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
#endif
}

void HStack::removeChild(Button& button) {
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
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(Link& link) {
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
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(Spacer& spacer) {
    if (!pImpl || !pImpl->valid || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), spacerView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(TextView& textView) {
    if (!pImpl || !pImpl->valid || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), textViewHandle);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, textViewHandle);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = hstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
#endif
}

void HStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add HStack to window - native side handles filling content view
    obsidian_macos_hstack_add_to_window(pImpl->hstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Set padding and spacing
    obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                     pImpl->padding.top, pImpl->padding.bottom,
                                     pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, pImpl->spacing);
    
    // Force layout update
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
}

void HStack::addToScreen(Screen& screen) {
    if (!pImpl) {
        return;
    }
    
    void* screenContentView = screen.getNativeHandle();
    if (screenContentView) {
        addToParentView(screenContentView);
    }
}

void HStack::addToParentView(void* parentView) {
    if (!pImpl || !parentView) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* hstackView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    if (!hstackView) {
        return;
    }
    
    // Add HStack view to parent - native side sets frame to fill parent
    obsidian_macos_screen_add_subview(parentView, hstackView);
    pImpl->parentView = parentView;
    
    // Set padding and spacing
    obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                       pImpl->padding.top, pImpl->padding.bottom,
                                       pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, pImpl->spacing);
    
    // Force layout update
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
}

void HStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    obsidian_macos_hstack_remove_from_parent(pImpl->hstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void HStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Just trigger native layout update - all positioning done in native layoutSubviews
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
}

bool HStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

void* HStack::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    if (pImpl->hstackHandle) {
        return obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    }
#endif
    return nullptr;
}

HStack::HStack(HStack&&) noexcept = default;
HStack& HStack::operator=(HStack&&) noexcept = default;

} // namespace obsidian
