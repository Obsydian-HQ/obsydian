/**
 * ScreenContainer Implementation
 */

#include "obsidian/navigation/screen_container.h"
#include "obsidian/window.h"

#ifdef __APPLE__
#include "../../platform/apple/macos/macos_screen_container.h"
#include "../../platform/apple/macos/macos_window.h"
#endif

#include <unordered_map>

namespace obsidian {

// ============================================================================
// Screen Implementation
// ============================================================================

struct Screen::Impl {
    void* nativeHandle = nullptr;
    std::string routePath;
    bool ownsHandle = false;  // Screen doesn't own handle - container does
    
    Impl(void* handle, const std::string& path)
        : nativeHandle(handle), routePath(path), ownsHandle(false) {}
};

Screen::Screen(void* handle, const std::string& routePath)
    : pImpl(std::make_unique<Impl>(handle, routePath)) {}

Screen::Screen(Screen&& other) noexcept = default;
Screen& Screen::operator=(Screen&& other) noexcept = default;

Screen::~Screen() {
    // Screen doesn't own the native handle - ScreenContainer does
    // So we don't destroy anything here
}

const std::string& Screen::getRoutePath() const {
    return pImpl->routePath;
}

void Screen::clear() {
#ifdef __APPLE__
    if (pImpl->nativeHandle) {
        obsidian_macos_screen_clear_content(pImpl->nativeHandle);
    }
#endif
}

void* Screen::getNativeHandle() const {
    return pImpl->nativeHandle;
}

bool Screen::isActive() const {
    // TODO: Query from native side if needed
    return false;
}

// ============================================================================
// ScreenContainer Implementation
// ============================================================================

struct ScreenContainer::Impl {
    void* nativeHandle = nullptr;
    std::unordered_map<std::string, std::unique_ptr<Screen>> screens;
    Screen* activeScreen = nullptr;
    
    Impl() {
#ifdef __APPLE__
        ObsidianScreenContainerParams params = {0};
        nativeHandle = obsidian_macos_create_screen_container(params);
#endif
    }
    
    ~Impl() {
#ifdef __APPLE__
        if (nativeHandle) {
            obsidian_macos_destroy_screen_container(nativeHandle);
        }
#endif
    }
};

ScreenContainer::ScreenContainer()
    : pImpl(std::make_unique<Impl>()) {}

ScreenContainer::~ScreenContainer() = default;

ScreenContainer::ScreenContainer(ScreenContainer&& other) noexcept = default;
ScreenContainer& ScreenContainer::operator=(ScreenContainer&& other) noexcept = default;

void ScreenContainer::attachToWindow(Window& window) {
#ifdef __APPLE__
    if (pImpl->nativeHandle) {
        void* windowHandle = window.getNativeHandle();
        obsidian_macos_screen_container_attach_to_window(pImpl->nativeHandle, windowHandle);
    }
#endif
}

void ScreenContainer::attachToView(void* parentView) {
#ifdef __APPLE__
    if (pImpl->nativeHandle && parentView) {
        obsidian_macos_screen_container_attach_to_view(pImpl->nativeHandle, parentView);
    }
#endif
}

Screen* ScreenContainer::getOrCreateScreen(const std::string& routePath) {
    // Check if exists
    auto it = pImpl->screens.find(routePath);
    if (it != pImpl->screens.end()) {
        return it->second.get();
    }
    
    // Create new screen
#ifdef __APPLE__
    ObsidianScreenParams params;
    params.routePath = routePath.c_str();
    void* screenHandle = obsidian_macos_screen_container_create_screen(pImpl->nativeHandle, params);
    if (screenHandle) {
        auto screen = std::unique_ptr<Screen>(new Screen(screenHandle, routePath));
        Screen* ptr = screen.get();
        pImpl->screens[routePath] = std::move(screen);
        return ptr;
    }
#endif
    
    return nullptr;
}

Screen* ScreenContainer::getScreen(const std::string& routePath) {
    auto it = pImpl->screens.find(routePath);
    return (it != pImpl->screens.end()) ? it->second.get() : nullptr;
}

void ScreenContainer::setActiveScreen(Screen* screen) {
    pImpl->activeScreen = screen;
    
#ifdef __APPLE__
    if (pImpl->nativeHandle) {
        void* screenHandle = screen ? screen->getNativeHandle() : nullptr;
        obsidian_macos_screen_container_set_active_screen(pImpl->nativeHandle, screenHandle);
    }
#endif
}

Screen* ScreenContainer::getActiveScreen() {
    return pImpl->activeScreen;
}

void ScreenContainer::removeScreen(const std::string& routePath) {
    auto it = pImpl->screens.find(routePath);
    if (it != pImpl->screens.end()) {
        if (pImpl->activeScreen == it->second.get()) {
            pImpl->activeScreen = nullptr;
        }
        
#ifdef __APPLE__
        if (pImpl->nativeHandle && it->second->getNativeHandle()) {
            obsidian_macos_screen_container_remove_screen(
                pImpl->nativeHandle, 
                it->second->getNativeHandle());
        }
#endif
        
        pImpl->screens.erase(it);
    }
}

void ScreenContainer::clearAll() {
#ifdef __APPLE__
    if (pImpl->nativeHandle) {
        obsidian_macos_screen_container_clear_all(pImpl->nativeHandle);
    }
#endif
    
    pImpl->screens.clear();
    pImpl->activeScreen = nullptr;
}

void* ScreenContainer::getNativeHandle() const {
    return pImpl->nativeHandle;
}

} // namespace obsidian
