/**
 * Obsidian Public API - Button Implementation
 * 
 * REFACTORED: Uses Tag-based identification with ComponentViewRegistry.
 * The handle IS the NSButton (no wrapper indirection).
 * Native view lifecycle managed by registry, not RAII destructors.
 */

#include "obsidian/button.h"
#include "obsidian/window.h"
#include <iostream>

#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#endif

#include "../../core/mounting/component_view_registry.h"
#include "../../core/mounting/mounting_coordinator.h"
#include "../../core/mounting/view_mutation.h"

namespace obsidian {

using namespace obsidian::mounting;

class Button::Impl {
public:
    Tag tag = 0;
    bool valid = false;
    std::function<void()> pendingCallback;
    
    void* getNativeView() const {
        if (tag == 0) return nullptr;
        return ComponentViewRegistry::getInstance().findNativeView(tag);
    }
};

Button::Button() : pImpl(std::make_unique<Impl>()) {}

Button::~Button() {
    // Do NOT destroy native view - managed by ComponentViewRegistry
    pImpl->valid = false;
}

bool Button::create(const std::string& title, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false;
    }
    
    auto& registry = ComponentViewRegistry::getInstance();
    auto& coordinator = MountingCoordinator::getInstance();
    
    pImpl->tag = registry.generateTag();
    
    // Create the native button via registry
    auto createMutation = ViewMutation::CreateMutation(pImpl->tag, ComponentHandles::Button);
    coordinator.performMutation(createMutation);
    
    void* nativeView = pImpl->getNativeView();
    if (!nativeView) {
        pImpl->tag = 0;
        return false;
    }

#ifdef __APPLE__
    // Configure button - nativeView IS the NSButton handle
    obsidian_macos_button_set_title(nativeView, title.c_str());
    
    // Set frame via mutation
    LayoutMetrics metrics{
        static_cast<float>(x), 
        static_cast<float>(y), 
        static_cast<float>(width), 
        static_cast<float>(height)
    };
    auto updateMutation = ViewMutation::UpdateMutation(pImpl->tag, LayoutMetrics{}, metrics);
    coordinator.performMutation(updateMutation);
    
    // Set callback if pending
    if (pImpl->pendingCallback) {
        auto* callbackPtr = new std::function<void()>(pImpl->pendingCallback);
        obsidian_macos_button_set_on_click(
            nativeView,
            [](void* userData) {
                auto* cb = static_cast<std::function<void()>*>(userData);
                if (cb && *cb) (*cb)();
            },
            callbackPtr
        );
    }
#endif
    
    pImpl->valid = true;
    return true;
}

void Button::setTitle(const std::string& title) {
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        obsidian_macos_button_set_title(view, title.c_str());
    }
#endif
}

std::string Button::getTitle() const {
    if (!pImpl->valid) return {};
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        const char* title = obsidian_macos_button_get_title(view);
        return title ? std::string(title) : std::string();
    }
#endif
    return {};
}

void Button::setOnClick(std::function<void()> callback) {
    pImpl->pendingCallback = callback;
    
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        auto* callbackPtr = new std::function<void()>(callback);
        obsidian_macos_button_set_on_click(
            view,
            [](void* userData) {
                auto* cb = static_cast<std::function<void()>*>(userData);
                if (cb && *cb) (*cb)();
            },
            callbackPtr
        );
    }
#endif
}

void Button::setVisible(bool visible) {
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        obsidian_macos_button_set_visible(view, visible);
    }
#endif
}

bool Button::isVisible() const {
    if (!pImpl->valid) return false;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        return obsidian_macos_button_is_visible(view);
    }
#endif
    return false;
}

void Button::setEnabled(bool enabled) {
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        obsidian_macos_button_set_enabled(view, enabled);
    }
#endif
}

bool Button::isEnabled() const {
    if (!pImpl->valid) return false;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        return obsidian_macos_button_is_enabled(view);
    }
#endif
    return false;
}

void Button::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    void* windowHandle = window.getNativeHandle();
    
    if (view && windowHandle) {
        obsidian_macos_button_add_to_window(view, windowHandle);
    }
#endif
}

void Button::removeFromParent() {
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    void* view = pImpl->getNativeView();
    if (view) {
        obsidian_macos_button_remove_from_parent(view);
    }
#endif
}

bool Button::isValid() const {
    return pImpl->valid;
}

void* Button::getNativeViewHandle() const {
    if (!pImpl->valid) return nullptr;
    
    // Handle IS the NSButton - no indirection
    return pImpl->getNativeView();
}

Button::Button(Button&& other) noexcept = default;
Button& Button::operator=(Button&& other) noexcept = default;

} // namespace obsidian
