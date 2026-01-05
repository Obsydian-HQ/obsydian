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
#include "macos_window.h"  // For toolbar functions
#include "macos_toolbar.h" // For toolbar FFI
#endif

#include <iostream>

namespace obsidian {

class Window::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::Window macosWindow;
    obsidian::ffi::macos::Toolbar toolbar;
    bool hasToolbar = false;
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

bool Window::setupToolbarForSidebar() {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    // Get the window handle first
    void* windowHandle = pImpl->macosWindow.getHandle();
    if (!windowHandle) {
        std::cerr << "Window handle is null\n";
        return false;
    }
    
    // CRITICAL: Configure the window for sidebar FIRST, before adding toolbar
    // This adds NSWindowStyleMaskFullSizeContentView, hides the title, and makes
    // the titlebar transparent so the sidebar can extend into the titlebar area
    obsidian_macos_window_configure_for_sidebar(windowHandle);
    
    // Create the toolbar
    if (!pImpl->toolbar.create("ObsidianSidebarToolbar")) {
        std::cerr << "Failed to create toolbar\n";
        return false;
    }
    
    // Insert sidebar toggle button at index 0
    pImpl->toolbar.insertSidebarToggleItem(0);
    
    // Insert sidebar tracking separator at index 1 (macOS 11+)
    pImpl->toolbar.insertSidebarTrackingSeparator(1);
    
    // Get the NSToolbar handle
    void* nsToolbarHandle = pImpl->toolbar.getNSToolbarHandle();
    if (!nsToolbarHandle) {
        std::cerr << "Failed to get NSToolbar handle\n";
        return false;
    }
    
    obsidian_macos_window_set_toolbar(windowHandle, nsToolbarHandle);
    
    // Set unified compact style for sidebar integration (like CodeEdit)
    obsidian_macos_window_set_toolbar_style(windowHandle, ObsidianWindowToolbarStyleUnified);
    
    // CRITICAL: Set window titlebar separator style to automatic
    // This allows the system to determine the separator based on the split view items
    // (which have their own titlebarSeparatorStyle set - none for sidebar, line for content)
    obsidian_macos_window_set_titlebar_separator_style(windowHandle, ObsidianWindowTitlebarSeparatorStyleAutomatic);
    
    pImpl->hasToolbar = true;
    std::cout << "Toolbar setup complete for sidebar\n";
    return true;
#else
    return false;
#endif
}

bool Window::hasToolbar() const {
#ifdef __APPLE__
    return pImpl->hasToolbar;
#else
    return false;
#endif
}

void Window::setToolbarStyle(ToolbarStyle style) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    void* windowHandle = pImpl->macosWindow.getHandle();
    if (!windowHandle) {
        return;
    }
    
    ObsidianWindowToolbarStyle cStyle;
    switch (style) {
        case ToolbarStyle::Expanded:
            cStyle = ObsidianWindowToolbarStyleExpanded;
            break;
        case ToolbarStyle::Preference:
            cStyle = ObsidianWindowToolbarStylePreference;
            break;
        case ToolbarStyle::Unified:
            cStyle = ObsidianWindowToolbarStyleUnified;
            break;
        case ToolbarStyle::UnifiedCompact:
            cStyle = ObsidianWindowToolbarStyleUnifiedCompact;
            break;
        case ToolbarStyle::Automatic:
        default:
            cStyle = ObsidianWindowToolbarStyleAutomatic;
            break;
    }
    obsidian_macos_window_set_toolbar_style(windowHandle, cStyle);
#endif
}

ToolbarStyle Window::getToolbarStyle() const {
    if (!pImpl->valid) {
        return ToolbarStyle::Automatic;
    }
    
#ifdef __APPLE__
    void* windowHandle = pImpl->macosWindow.getHandle();
    if (!windowHandle) {
        return ToolbarStyle::Automatic;
    }
    
    ObsidianWindowToolbarStyle cStyle = obsidian_macos_window_get_toolbar_style(windowHandle);
    switch (cStyle) {
        case ObsidianWindowToolbarStyleExpanded:
            return ToolbarStyle::Expanded;
        case ObsidianWindowToolbarStylePreference:
            return ToolbarStyle::Preference;
        case ObsidianWindowToolbarStyleUnified:
            return ToolbarStyle::Unified;
        case ObsidianWindowToolbarStyleUnifiedCompact:
            return ToolbarStyle::UnifiedCompact;
        case ObsidianWindowToolbarStyleAutomatic:
        default:
            return ToolbarStyle::Automatic;
    }
#else
    return ToolbarStyle::Automatic;
#endif
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

void Window::clearContent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    void* windowHandle = pImpl->macosWindow.getHandle();
    if (windowHandle) {
        obsidian_macos_window_clear_content(windowHandle);
    }
#endif
}

Window::Window(Window&& other) noexcept = default;
Window& Window::operator=(Window&& other) noexcept = default;

} // namespace obsidian

