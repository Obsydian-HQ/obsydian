/**
 * Obsidian Public API - Button Implementation
 * 
 * This file implements the public Button API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/button.h"
#include "obsidian/window.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_button.h"  // For C FFI function
#endif

namespace obsidian {

class Button::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::Button macosButton;
#endif
    bool valid = false;
};

Button::Button() : pImpl(std::make_unique<Impl>()) {}

Button::~Button() {
    // RAII: Automatically clean up resources
    // The FFI Button destructor will handle proper cleanup (remove from parent + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI Button
    // The FFI Button destructor calls obsidian_macos_destroy_button which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool Button::create(const std::string& title, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosButton.create(title, x, y, width, height)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void Button::setTitle(const std::string& title) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosButton.setTitle(title);
#endif
}

std::string Button::getTitle() const {
    if (!pImpl->valid) {
        return std::string();
    }
    
#ifdef __APPLE__
    return pImpl->macosButton.getTitle();
#else
    return std::string();
#endif
}

void Button::setOnClick(std::function<void()> callback) {
    // Allow setting callbacks before create() - they will be registered when create() is called
    // But we need the button to exist, so check if it's created
    // Actually, we should allow setting callbacks even before create() - store them and register on create
#ifdef __APPLE__
    pImpl->macosButton.setOnClick(callback);
#endif
}

void Button::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosButton.setVisible(visible);
#endif
}

bool Button::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosButton.isVisible();
#else
    return false;
#endif
}

void Button::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosButton.setEnabled(enabled);
#endif
}

bool Button::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosButton.isEnabled();
#else
    return false;
#endif
}

void Button::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both button and window
    void* buttonHandle = pImpl->macosButton.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!buttonHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_button_add_to_window(buttonHandle, windowHandle);
#endif
}

void Button::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosButton.removeFromParent();
    // Note: Button remains valid after removal - it's just not attached to a parent
    // The button will be destroyed when the destructor runs
#endif
}

bool Button::isValid() const {
    return pImpl->valid;
}

Button::Button(Button&& other) noexcept = default;
Button& Button::operator=(Button&& other) noexcept = default;

} // namespace obsidian

