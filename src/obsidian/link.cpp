/**
 * Obsidian Public API - Link Implementation
 * 
 * This file implements the public Link API by wrapping any child component
 * and handling navigation through the Router.
 */

#include "obsidian/link.h"
#include "obsidian/router.h"
#include "obsidian/route_registry_helper.h"
#include "obsidian/window.h"
#include "obsidian/button.h"
#include "obsidian/textview.h"
#include "obsidian/vstack.h"
#include "obsidian/hstack.h"
#include "obsidian/zstack.h"

#ifdef __APPLE__
#include "packages/apple/macos_link.h"
#endif

namespace obsidian {

// Forward declaration for the global navigation callback
static void linkNavigationCallback(void* userData);

/**
 * Link callback data - heap allocated, owned by native handler
 * 
 * Following React Native's pattern: callback data lives as long as the
 * native handler. Ownership is transferred to native side, which deletes
 * it when the handler is deallocated.
 */
struct LinkCallbackData {
    std::string href;
    std::function<void()> onClickCallback;
    
    explicit LinkCallbackData(const std::string& h) : href(h) {}
};

// Release callback for native side to call when handler is deallocated
static void releaseCallbackData(void* userData) {
    if (userData) {
        delete static_cast<LinkCallbackData*>(userData);
    }
}

class Link::Impl {
public:
    std::string href;
    Router* router = nullptr;
    std::function<void()> onClickCallback;
    bool valid = false;
    
    // Child component reference (one of these will be set)
    Button* buttonChild = nullptr;
    TextView* textViewChild = nullptr;
    VStack* vstackChild = nullptr;
    HStack* hstackChild = nullptr;
    
    // For legacy text-based API
    Button internalButton;
    
    // Callback data - ownership transferred to native handler
    // DO NOT delete here - native handler deletes it when deallocated
    LinkCallbackData* callbackData = nullptr;
    
#ifdef __APPLE__
    ObsidianLinkHandle macosLinkHandle = nullptr;
#endif
    
    ~Impl() {
        releaseHandle();
    }
    
    void* getChildNativeViewHandle() const {
        if (buttonChild && buttonChild->isValid()) {
            return buttonChild->getNativeViewHandle();
        }
        if (textViewChild && textViewChild->isValid()) {
            return textViewChild->getNativeHandle();
        }
        if (vstackChild && vstackChild->isValid()) {
            return vstackChild->getNativeViewHandle();
        }
        if (hstackChild && hstackChild->isValid()) {
            return hstackChild->getNativeViewHandle();
        }
        if (internalButton.isValid()) {
            return internalButton.getNativeViewHandle();
        }
        return nullptr;
    }
    
    /**
     * Release handle reference (like VStack pattern)
     * 
     * The native view stays in the view hierarchy - superview retains it.
     * The ObsidianLinkHandler is retained by the child view via associated object.
     * When the child view is deallocated, the handler is deallocated,
     * which then deletes the callbackData via releaseUserData callback.
     */
    void releaseHandle() {
#ifdef __APPLE__
        if (macosLinkHandle) {
            // Just release our reference - native handler lives on
            // (retained by associated object on child view)
            obsidian_macos_release_link_handle(macosLinkHandle);
            macosLinkHandle = nullptr;
        }
#endif
        // DO NOT delete callbackData - native handler owns it now
        callbackData = nullptr;
        valid = false;
    }
};

// Static navigation callback that uses the global router
static void linkNavigationCallback(void* userData) {
    if (!userData) {
        return;
    }
    
    LinkCallbackData* data = static_cast<LinkCallbackData*>(userData);
    
    // Call user callback if set
    if (data->onClickCallback) {
        data->onClickCallback();
    }
    
    // Navigate using global router
    if (g_router && g_router->isValid() && !data->href.empty()) {
        g_router->navigate(data->href);
    }
}

Link::Link() : pImpl(std::make_unique<Impl>()) {
    // Try to get global router instance
    pImpl->router = g_router;
}

Link::Link(Link&&) noexcept = default;
Link& Link::operator=(Link&&) noexcept = default;

Link::~Link() {
    // Impl's destructor handles cleanup
}

// Create with Button child
bool Link::create(const std::string& href, Button& child) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
    if (!child.isValid()) {
        return false; // Child must be valid
    }
    
    pImpl->href = href;
    pImpl->buttonChild = &child;
    
#ifdef __APPLE__
    void* childView = child.getNativeViewHandle();
    if (!childView) {
        return false;
    }
    
    ObsidianLinkParams params;
    params.href = href.c_str();
    params.childView = childView;
    
    pImpl->macosLinkHandle = obsidian_macos_create_link(params);
    if (!pImpl->macosLinkHandle) {
        return false;
    }
    
    // Allocate callback data on heap - ownership transfers to native handler
    pImpl->callbackData = new LinkCallbackData(href);
    obsidian_macos_link_set_on_click(pImpl->macosLinkHandle, linkNavigationCallback, 
                                      pImpl->callbackData, releaseCallbackData);
#endif
    
    pImpl->valid = true;
    return true;
}

// Create with TextView child
bool Link::create(const std::string& href, TextView& child) {
    if (pImpl->valid) {
        return false;
    }
    
    if (!child.isValid()) {
        return false;
    }
    
    pImpl->href = href;
    pImpl->textViewChild = &child;
    
#ifdef __APPLE__
    void* childView = child.getNativeHandle();
    if (!childView) {
        return false;
    }
    
    ObsidianLinkParams params;
    params.href = href.c_str();
    params.childView = childView;
    
    pImpl->macosLinkHandle = obsidian_macos_create_link(params);
    if (!pImpl->macosLinkHandle) {
        return false;
    }
    
    // Allocate callback data on heap - ownership transfers to native handler
    pImpl->callbackData = new LinkCallbackData(href);
    obsidian_macos_link_set_on_click(pImpl->macosLinkHandle, linkNavigationCallback,
                                      pImpl->callbackData, releaseCallbackData);
#endif
    
    pImpl->valid = true;
    return true;
}

// Create with VStack child
bool Link::create(const std::string& href, VStack& child) {
    if (pImpl->valid) {
        return false;
    }
    
    if (!child.isValid()) {
        return false;
    }
    
    pImpl->href = href;
    pImpl->vstackChild = &child;
    
#ifdef __APPLE__
    void* childView = child.getNativeViewHandle();
    if (!childView) {
        return false;
    }
    
    ObsidianLinkParams params;
    params.href = href.c_str();
    params.childView = childView;
    
    pImpl->macosLinkHandle = obsidian_macos_create_link(params);
    if (!pImpl->macosLinkHandle) {
        return false;
    }
    
    // Allocate callback data on heap - ownership transfers to native handler
    pImpl->callbackData = new LinkCallbackData(href);
    obsidian_macos_link_set_on_click(pImpl->macosLinkHandle, linkNavigationCallback,
                                      pImpl->callbackData, releaseCallbackData);
#endif
    
    pImpl->valid = true;
    return true;
}

// Create with HStack child
bool Link::create(const std::string& href, HStack& child) {
    if (pImpl->valid) {
        return false;
    }
    
    if (!child.isValid()) {
        return false;
    }
    
    pImpl->href = href;
    pImpl->hstackChild = &child;
    
#ifdef __APPLE__
    void* childView = child.getNativeViewHandle();
    if (!childView) {
        return false;
    }
    
    ObsidianLinkParams params;
    params.href = href.c_str();
    params.childView = childView;
    
    pImpl->macosLinkHandle = obsidian_macos_create_link(params);
    if (!pImpl->macosLinkHandle) {
        return false;
    }
    
    // Allocate callback data on heap - ownership transfers to native handler
    pImpl->callbackData = new LinkCallbackData(href);
    obsidian_macos_link_set_on_click(pImpl->macosLinkHandle, linkNavigationCallback,
                                      pImpl->callbackData, releaseCallbackData);
#endif
    
    pImpl->valid = true;
    return true;
}

// ZStack support deferred - ZStack needs getNativeViewHandle() method first

// Legacy API: Create with text (creates Button internally)
bool Link::create(const std::string& href, const std::string& text, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false;
    }
    
    pImpl->href = href;
    
    // Create internal button
    if (!pImpl->internalButton.create(text, x, y, width, height)) {
        return false;
    }
    
    // Use the Button create method
    return create(href, pImpl->internalButton);
}

void Link::setHref(const std::string& href) {
    pImpl->href = href;
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        obsidian_macos_link_set_href(pImpl->macosLinkHandle, href.c_str());
    }
#endif
}

std::string Link::getHref() const {
    return pImpl->href;
}

void Link::setText(const std::string& text) {
    if (!pImpl->valid) {
        return;
    }
    // Only works with legacy text-based API (internal button)
    if (pImpl->internalButton.isValid()) {
        pImpl->internalButton.setTitle(text);
    }
}

std::string Link::getText() const {
    if (!pImpl->valid) {
        return std::string();
    }
    // Only works with legacy text-based API (internal button)
    if (pImpl->internalButton.isValid()) {
        return pImpl->internalButton.getTitle();
    }
    return std::string();
}

void Link::setOnClick(std::function<void()> callback) {
    pImpl->onClickCallback = callback;
    // Also update the callback data if it exists
    if (pImpl->callbackData) {
        pImpl->callbackData->onClickCallback = callback;
    }
}

void Link::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        void* windowHandle = window.getNativeHandle();
        obsidian_macos_link_add_to_window(pImpl->macosLinkHandle, windowHandle);
    }
#endif
}

void Link::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        obsidian_macos_link_remove_from_parent(pImpl->macosLinkHandle);
    }
#endif
}

void Link::setRouter(Router* router) {
    pImpl->router = router;
}

void Link::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        obsidian_macos_link_set_visible(pImpl->macosLinkHandle, visible);
    }
#endif
}

bool Link::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        return obsidian_macos_link_is_visible(pImpl->macosLinkHandle);
    }
#endif
    return false;
}

void Link::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        obsidian_macos_link_set_enabled(pImpl->macosLinkHandle, enabled);
    }
#endif
}

bool Link::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        return obsidian_macos_link_is_enabled(pImpl->macosLinkHandle);
    }
#endif
    return false;
}

bool Link::isValid() const {
    return pImpl->valid;
}

void* Link::getNativeViewHandle() const {
    if (!pImpl->valid) {
        return nullptr;
    }
#ifdef __APPLE__
    if (pImpl->macosLinkHandle) {
        return obsidian_macos_link_get_view(pImpl->macosLinkHandle);
    }
#endif
    return nullptr;
}

} // namespace obsidian
