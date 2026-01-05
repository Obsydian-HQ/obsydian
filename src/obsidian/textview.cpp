/**
 * Obsidian Public API - TextView Implementation
 * 
 * This file implements the public TextView API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/textview.h"
#include "obsidian/window.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_textview.h"  // For C FFI function
#endif

namespace obsidian {

class TextView::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::TextView macosTextView;
#endif
    bool valid = false;
};

TextView::TextView() : pImpl(std::make_unique<Impl>()) {}

TextView::~TextView() {
    // RAII: Automatically clean up resources
    // The FFI TextView destructor will handle proper cleanup (remove from parent + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI TextView
    // The FFI TextView destructor calls obsidian_macos_destroy_textview which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool TextView::create(const std::string& text, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosTextView.create(x, y, width, height)) {
        return false;
    }
    if (!text.empty()) {
        pImpl->macosTextView.setString(text);
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

bool TextView::create(int x, int y, int width, int height) {
    return create("", x, y, width, height);
}

void TextView::setString(const std::string& text) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setString(text);
#endif
}

std::string TextView::getString() const {
    if (!pImpl->valid) {
        return std::string();
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.getString();
#else
    return std::string();
#endif
}

void TextView::setFontSize(double size) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setFontSize(size);
#endif
}

double TextView::getFontSize() const {
    if (!pImpl->valid) {
        return 0.0;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.getFontSize();
#else
    return 0.0;
#endif
}

void TextView::setFontWeight(FontWeight weight) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setFontWeight((int)weight);
#endif
}

FontWeight TextView::getFontWeight() const {
    if (!pImpl->valid) {
        return FontWeight::Regular;
    }
    
#ifdef __APPLE__
    return (FontWeight)pImpl->macosTextView.getFontWeight();
#else
    return FontWeight::Regular;
#endif
}

void TextView::setEditable(bool editable) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setEditable(editable);
#endif
}

bool TextView::isEditable() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.isEditable();
#else
    return false;
#endif
}

void TextView::setSelectable(bool selectable) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setSelectable(selectable);
#endif
}

bool TextView::isSelectable() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.isSelectable();
#else
    return false;
#endif
}

void TextView::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setVisible(visible);
#endif
}

bool TextView::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.isVisible();
#else
    return false;
#endif
}

void TextView::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.setEnabled(enabled);
#endif
}

bool TextView::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.isEnabled();
#else
    return false;
#endif
}

void TextView::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both text view and window
    void* textViewHandle = pImpl->macosTextView.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!textViewHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_textview_add_to_window(textViewHandle, windowHandle);
#endif
}

void TextView::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextView.removeFromParent();
    // Note: TextView remains valid after removal - it's just not attached to a parent
    // The text view will be destroyed when the destructor runs
#endif
}

bool TextView::isValid() const {
    return pImpl->valid;
}

void* TextView::getNativeHandle() const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.getHandle();
#else
    return nullptr;
#endif
}

void* TextView::getNativeViewHandle() const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextView.getViewHandle();
#else
    return nullptr;
#endif
}

TextView::TextView(TextView&& other) noexcept = default;
TextView& TextView::operator=(TextView&& other) noexcept = default;

} // namespace obsidian

