/**
 * Obsidian Public API - ScrollView Implementation
 * 
 * This file implements the public ScrollView API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/scrollview.h"
#include "obsidian/window.h"
#include "obsidian/textview.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_scrollview.h"  // For C FFI function
#include "macos_textview.h"    // For C FFI function
#endif

namespace obsidian {

class ScrollView::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::ScrollView macosScrollView;
#endif
    bool valid = false;
};

ScrollView::ScrollView() : pImpl(std::make_unique<Impl>()) {}

ScrollView::~ScrollView() {
    // RAII: Automatically clean up resources
    // The FFI ScrollView destructor will handle proper cleanup (remove from parent + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI ScrollView
    // The FFI ScrollView destructor calls obsidian_macos_destroy_scrollview which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool ScrollView::create(int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosScrollView.create(x, y, width, height)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void ScrollView::setDocumentView(TextView& textView) {
    if (!pImpl->valid || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the TextView handle from the public API's internal implementation
    // We need to access the internal macosTextView to get its handle
    // Then get the view handle from that
    void* textViewHandle = textView.getNativeHandle();
    if (!textViewHandle) {
        return;
    }
    
    // Get the view handle from the TextView handle
    void* viewHandle = obsidian_macos_textview_get_view_handle(textViewHandle);
    if (!viewHandle) {
        return;
    }
    
    // Get the ScrollView handle
    void* scrollViewHandle = pImpl->macosScrollView.getHandle();
    if (!scrollViewHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_scrollview_set_document_view(scrollViewHandle, viewHandle);
#endif
}

void* ScrollView::getDocumentView() const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    return pImpl->macosScrollView.getDocumentView();
#else
    return nullptr;
#endif
}

void ScrollView::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosScrollView.setVisible(visible);
#endif
}

bool ScrollView::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosScrollView.isVisible();
#else
    return false;
#endif
}

void ScrollView::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both scroll view and window
    void* scrollViewHandle = pImpl->macosScrollView.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!scrollViewHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_scrollview_add_to_window(scrollViewHandle, windowHandle);
#endif
}

void ScrollView::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosScrollView.removeFromParent();
    // Note: ScrollView remains valid after removal - it's just not attached to a parent
    // The scroll view will be destroyed when the destructor runs
#endif
}

bool ScrollView::isValid() const {
    return pImpl->valid;
}

ScrollView::ScrollView(ScrollView&& other) noexcept = default;
ScrollView& ScrollView::operator=(ScrollView&& other) noexcept = default;

} // namespace obsidian

