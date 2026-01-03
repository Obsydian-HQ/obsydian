/**
 * Obsidian Public API - Window Implementation
 * 
 * This file implements the public Window API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/window.h"

// Include internal headers (not exposed to users)
// Access through dependencies: //core:ffi_base exposes ffi/platform.h
#include "platform.h"

#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#endif

namespace obsidian {

class Window::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::Window macosWindow;
#endif
    bool valid = false;
};

Window::Window() : pImpl(std::make_unique<Impl>()) {}

Window::~Window() {
    close();
}

bool Window::create(int width, int height, const std::string& title) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosWindow.create(width, height, title)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

void Window::show() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosWindow.show();
#endif
}

void Window::close() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosWindow.close();
#endif
    
    pImpl->valid = false;
}

bool Window::isValid() const {
    return pImpl->valid;
}

void* Window::getNativeHandle() const {
    if (!pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    return pImpl->macosWindow.getHandle();
#else
    return nullptr;
#endif
}

Window::Window(Window&& other) noexcept = default;
Window& Window::operator=(Window&& other) noexcept = default;

} // namespace obsidian

