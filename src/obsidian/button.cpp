/**
 * Obsidian Public API - Button Implementation
 * 
 * Uses the Fabric mutation system for view lifecycle.
 * Views are created via Create mutation, properties set via bridge functions.
 */

#include "obsidian/button.h"
#include "obsidian/window.h"
#include <iostream>

#ifdef __APPLE__
// Fabric bridge - the new clean system
extern "C" {
    void obs_fabric_initialize(void);
    int32_t obs_fabric_generate_tag(void);
    void* obs_fabric_find_view(int32_t tag);
    
    typedef struct {
        float x, y, width, height;
    } OBSCLayoutMetrics;
    
    typedef enum {
        OBS_MUTATION_CREATE = 0,
        OBS_MUTATION_DELETE,
        OBS_MUTATION_INSERT,
        OBS_MUTATION_REMOVE,
        OBS_MUTATION_UPDATE
    } OBSCMutationType;
    
    typedef struct {
        OBSCMutationType type;
        int32_t tag;
        int32_t parentTag;
        int32_t index;
        const char* componentHandle;
        OBSCLayoutMetrics oldLayoutMetrics;
        OBSCLayoutMetrics newLayoutMetrics;
    } OBSCViewMutation;
    
    void obs_fabric_apply_mutations(const OBSCViewMutation* mutations, int32_t count);
    
    extern const char* OBS_COMPONENT_BUTTON;
    
    typedef void (*OBSButtonCallback)(void* userData);
    void obs_fabric_button_set_title(int32_t tag, const char* title);
    void obs_fabric_button_set_callback(int32_t tag, OBSButtonCallback callback, void* userData);
    void obs_fabric_add_view_to_window(int32_t tag, void* windowHandle);
    void obs_fabric_remove_view_from_parent(int32_t tag);
}
#endif

namespace obsidian {

class Button::Impl {
public:
    int32_t tag = 0;
    bool valid = false;
    std::function<void()> pendingCallback;
    
    void* getNativeView() const {
#ifdef __APPLE__
        if (tag == 0) return nullptr;
        return obs_fabric_find_view(tag);
#else
        return nullptr;
#endif
    }
};

Button::Button() : pImpl(std::make_unique<Impl>()) {}

Button::~Button() {
    // Views are managed by ComponentViewRegistry - don't destroy here
    // When we need proper cleanup, we'll send a Delete mutation
    pImpl->valid = false;
}

bool Button::create(const std::string& title, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    // Generate unique tag
    pImpl->tag = obs_fabric_generate_tag();
    
    // Create button via mutation
    OBSCViewMutation createMutation = {};
    createMutation.type = OBS_MUTATION_CREATE;
    createMutation.tag = pImpl->tag;
    createMutation.componentHandle = OBS_COMPONENT_BUTTON;
    
    obs_fabric_apply_mutations(&createMutation, 1);
    
    // Verify view was created
    void* nativeView = pImpl->getNativeView();
    if (!nativeView) {
        std::cerr << "[Button] Failed to create native view for tag " << pImpl->tag << std::endl;
        pImpl->tag = 0;
        return false;
    }
    
    // Set title
    obs_fabric_button_set_title(pImpl->tag, title.c_str());
    
    // Set frame via Update mutation
    OBSCViewMutation updateMutation = {};
    updateMutation.type = OBS_MUTATION_UPDATE;
    updateMutation.tag = pImpl->tag;
    updateMutation.newLayoutMetrics = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };
    
    obs_fabric_apply_mutations(&updateMutation, 1);
    
    // Set callback if pending
    if (pImpl->pendingCallback) {
        auto* callbackPtr = new std::function<void()>(pImpl->pendingCallback);
        obs_fabric_button_set_callback(
            pImpl->tag,
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
    obs_fabric_button_set_title(pImpl->tag, title.c_str());
#endif
}

std::string Button::getTitle() const {
    if (!pImpl->valid) return {};
    // TODO: Add obs_fabric_button_get_title if needed
    return {};
}

void Button::setOnClick(std::function<void()> callback) {
    pImpl->pendingCallback = callback;
    
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    auto* callbackPtr = new std::function<void()>(callback);
    obs_fabric_button_set_callback(
        pImpl->tag,
        [](void* userData) {
            auto* cb = static_cast<std::function<void()>*>(userData);
            if (cb && *cb) (*cb)();
        },
        callbackPtr
    );
#endif
}

void Button::setVisible(bool /* visible */) {
    if (!pImpl->valid) return;
    // TODO: Add visibility to Fabric bridge
}

bool Button::isVisible() const {
    if (!pImpl->valid) return false;
    return true; // TODO: Track visibility state
}

void Button::setEnabled(bool /* enabled */) {
    if (!pImpl->valid) return;
    // TODO: Add enabled state to Fabric bridge
}

bool Button::isEnabled() const {
    if (!pImpl->valid) return false;
    return true; // TODO: Track enabled state
}

void Button::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) return;
    
#ifdef __APPLE__
    void* windowHandle = window.getNativeHandle();
    if (windowHandle) {
        obs_fabric_add_view_to_window(pImpl->tag, windowHandle);
    }
#endif
}

void Button::removeFromParent() {
    if (!pImpl->valid) return;
    
#ifdef __APPLE__
    obs_fabric_remove_view_from_parent(pImpl->tag);
#endif
}

bool Button::isValid() const {
    return pImpl->valid;
}

void* Button::getNativeViewHandle() const {
    if (!pImpl->valid) return nullptr;
    return pImpl->getNativeView();
}

Button::Button(Button&& other) noexcept = default;
Button& Button::operator=(Button&& other) noexcept = default;

} // namespace obsidian
