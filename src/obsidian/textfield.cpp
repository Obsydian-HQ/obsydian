/**
 * Obsidian Public API - TextField Implementation
 * 
 * This file implements the public TextField API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/textfield.h"
#include "obsidian/window.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_textfield.h"  // For C FFI function
#endif

namespace obsidian {

class TextField::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::TextField macosTextField;
#endif
    bool valid = false;
};

TextField::TextField() : pImpl(std::make_unique<Impl>()) {}

TextField::~TextField() {
    // RAII: Automatically clean up resources
    // The FFI TextField destructor will handle proper cleanup (remove from parent + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI TextField
    // The FFI TextField destructor calls obsidian_macos_destroy_textfield which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool TextField::create(const std::string& placeholder, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosTextField.create(placeholder, x, y, width, height)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void TextField::setText(const std::string& text) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.setText(text);
#endif
}

std::string TextField::getText() const {
    if (!pImpl->valid) {
        return std::string();
    }
    
#ifdef __APPLE__
    return pImpl->macosTextField.getText();
#else
    return std::string();
#endif
}

void TextField::setPlaceholder(const std::string& placeholder) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.setPlaceholder(placeholder);
#endif
}

void TextField::setOnEnter(std::function<void()> callback) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.setOnEnter(callback);
#endif
}

void TextField::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.setVisible(visible);
#endif
}

bool TextField::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextField.isVisible();
#else
    return false;
#endif
}

void TextField::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.setEnabled(enabled);
#endif
}

bool TextField::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTextField.isEnabled();
#else
    return false;
#endif
}

void TextField::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both text field and window
    void* textFieldHandle = pImpl->macosTextField.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!textFieldHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_textfield_add_to_window(textFieldHandle, windowHandle);
#endif
}

void TextField::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTextField.removeFromParent();
    // Note: TextField remains valid after removal - it's just not attached to a parent
    // The text field will be destroyed when the destructor runs
#endif
}

bool TextField::isValid() const {
    return pImpl->valid;
}

TextField::TextField(TextField&& other) noexcept = default;
TextField& TextField::operator=(TextField&& other) noexcept = default;

} // namespace obsidian

