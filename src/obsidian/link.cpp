/**
 * Obsidian Public API - Link Implementation
 * 
 * This file implements the public Link API by wrapping a Button
 * and handling navigation through the Router.
 */

#include "obsidian/link.h"
#include "obsidian/router.h"
#include "obsidian/route_registry_helper.h"
#include "obsidian/window.h"
#include "obsidian/button.h"

namespace obsidian {

class Link::Impl {
public:
    Button button;
    std::string href;
    Router* router = nullptr;
    std::function<void()> onClickCallback;
    bool valid = false;
    
    void handleClick() {
        // Call user callback if set
        if (onClickCallback) {
            onClickCallback();
        }
        
        // Navigate using router
        if (router && router->isValid() && !href.empty()) {
            router->navigate(href);
        }
    }
};

Link::Link() : pImpl(std::make_unique<Impl>()) {
    // Try to get global router instance
    pImpl->router = g_router;
}

Link::Link(Link&&) noexcept = default;
Link& Link::operator=(Link&&) noexcept = default;

Link::~Link() {
    if (pImpl && pImpl->valid) {
        pImpl->valid = false;
    }
}

bool Link::create(const std::string& href, const std::string& text, int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
    pImpl->href = href;
    
    // Create internal button
    if (!pImpl->button.create(text, x, y, width, height)) {
        return false;
    }
    
    // Set button click handler to navigate
    pImpl->button.setOnClick([this]() {
        pImpl->handleClick();
    });
    
    pImpl->valid = true;
    return true;
}

void Link::setHref(const std::string& href) {
    pImpl->href = href;
}

std::string Link::getHref() const {
    return pImpl->href;
}

void Link::setText(const std::string& text) {
    if (!pImpl->valid) {
        return;
    }
    pImpl->button.setTitle(text);
}

std::string Link::getText() const {
    if (!pImpl->valid) {
        return std::string();
    }
    return pImpl->button.getTitle();
}

void Link::setOnClick(std::function<void()> callback) {
    pImpl->onClickCallback = callback;
}

void Link::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    pImpl->button.addToWindow(window);
}

void Link::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    pImpl->button.removeFromParent();
}

void Link::setRouter(Router* router) {
    pImpl->router = router;
}

void Link::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    pImpl->button.setVisible(visible);
}

bool Link::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    return pImpl->button.isVisible();
}

void Link::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    pImpl->button.setEnabled(enabled);
}

bool Link::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    return pImpl->button.isEnabled();
}

bool Link::isValid() const {
    return pImpl->valid;
}

void* Link::getNativeViewHandle() const {
    if (!pImpl->valid) {
        return nullptr;
    }
    return pImpl->button.getNativeViewHandle();
}

} // namespace obsidian
