/**
 * Obsidian Public API - VStack Implementation
 * 
 * Frame-based layout container. The native Objective-C++ implementation
 * handles all layout in layoutSubviews - NO AUTO LAYOUT CONSTRAINTS.
 */

#include "obsidian/vstack.h"
#include "obsidian/window.h"
#include "obsidian/screen_container.h"
#include "obsidian/button.h"
#include "obsidian/link.h"
#include "obsidian/spacer.h"
#include "obsidian/textview.h"
#include "obsidian/hstack.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#include "macos_window.h"
#include "macos_vstack.h"
#include "macos_screen_container.h"
#endif

#include <vector>
#include <algorithm>
#include <memory>

namespace obsidian {

class VStack::Impl {
public:
#ifdef __APPLE__
    ObsidianVStackHandle vstackHandle;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    double spacing;
    layout::Alignment alignment;
    Padding padding;
    
    Impl()
#ifdef __APPLE__
        : vstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Leading)
        , padding{0.0, 0.0, 0.0, 0.0}
    {}
    
    ~Impl() = default;
};

VStack::VStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view
    ObsidianVStackParams params;
    pImpl->vstackHandle = obsidian_macos_create_vstack(params);
    if (pImpl->vstackHandle) {
        pImpl->valid = true;
    }
#endif
}

VStack::~VStack() {
    if (pImpl && pImpl->valid) {
#ifdef __APPLE__
        // Release handle but do NOT remove from parent
        // The NSView stays in the superview hierarchy (superview retains it)
        if (pImpl->vstackHandle) {
            obsidian_macos_release_vstack_handle(pImpl->vstackHandle);
            pImpl->vstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void VStack::setSpacing(double spacing) {
    if (pImpl) {
        pImpl->spacing = spacing;
        if (pImpl->valid) {
#ifdef __APPLE__
            obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, spacing);
            // Trigger layout update - native side handles positioning
            obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
        }
    }
}

double VStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void VStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        // Note: Alignment is handled by how we position the container
        // Child alignment within VStack is always fill-width in frame-based layout
    }
}

layout::Alignment VStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Leading;
}

void VStack::setPadding(const Padding& padding) {
    if (pImpl) {
        pImpl->padding = padding;
        if (pImpl->valid) {
#ifdef __APPLE__
            obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                               padding.top, padding.bottom,
                                               padding.leading, padding.trailing);
            // Trigger layout update - native side handles positioning
            obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
        }
    }
}

Padding VStack::getPadding() const {
    return pImpl ? pImpl->padding : Padding{0.0, 0.0, 0.0, 0.0};
}

void VStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
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
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
#endif
}

void VStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
#endif
}

void VStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    link.removeFromParent();
    
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
#endif
}

void VStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    textView.removeFromParent();
    
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
#endif
}

void VStack::addChild(VStack& vstack) {
    if (!pImpl || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    vstack.removeFromParent();
    
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
#endif
}

void VStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* hstackView = hstack.getNativeViewHandle();
    if (!hstackView) {
        return;
    }
    
    hstack.removeFromParent();
    
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, hstackView);
    pImpl->childViewHandles.push_back(hstackView);
#endif
}

void VStack::removeChild(Button& button) {
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
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(Link& link) {
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
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(Spacer& spacer) {
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
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(TextView& textView) {
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
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, textViewHandle);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(VStack& vstack) {
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
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (!hstackView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), hstackView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, hstackView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
#endif
}

void VStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add VStack to window - native side handles filling content view
    obsidian_macos_vstack_add_to_window(pImpl->vstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Set padding and spacing
    obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                       pImpl->padding.top, pImpl->padding.bottom,
                                       pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, pImpl->spacing);
    
    // Force layout update
    obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
}

void VStack::addToScreen(Screen& screen) {
    if (!pImpl) {
        return;
    }
    
    void* screenContentView = screen.getNativeHandle();
    if (screenContentView) {
        addToParentView(screenContentView);
    }
}

void VStack::addToParentView(void* parentView) {
    if (!pImpl || !parentView) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* vstackView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    if (!vstackView) {
        return;
    }
    
    // Add VStack view to parent - native side sets frame to fill parent
    obsidian_macos_screen_add_subview(parentView, vstackView);
    pImpl->parentView = parentView;
    
    // Set padding and spacing
    obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                       pImpl->padding.top, pImpl->padding.bottom,
                                       pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, pImpl->spacing);
    
    // Force layout update
    obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
}

void VStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    obsidian_macos_vstack_remove_from_parent(pImpl->vstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void VStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Just trigger native layout update - all positioning done in native layoutSubviews
    obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
}

bool VStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

void* VStack::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    if (pImpl->vstackHandle) {
        return obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    }
#endif
    return nullptr;
}

VStack::VStack(VStack&&) noexcept = default;
VStack& VStack::operator=(VStack&&) noexcept = default;

} // namespace obsidian
