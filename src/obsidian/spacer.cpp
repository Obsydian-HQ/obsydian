/**
 * Obsidian Public API - Spacer Implementation
 * 
 * This file implements the public Spacer API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/layout/spacer.h"

#ifdef __APPLE__
#include "macos_spacer.h"
#include "macos_ffi.h"
#endif

#include <memory>

namespace obsidian {

#ifdef __APPLE__

class Spacer::Impl {
public:
    ObsidianSpacerHandle spacerHandle;
    bool valid;
    double minWidth;
    double minHeight;
    double maxWidth;
    double maxHeight;
    
    Impl()
        : spacerHandle(nullptr)
        , valid(false)
        , minWidth(0.0)
        , minHeight(0.0)
        , maxWidth(0.0)
        , maxHeight(0.0)
    {}
    
    ~Impl() {
        if (valid && spacerHandle) {
#ifdef __APPLE__
            obsidian_macos_destroy_spacer(spacerHandle);
            spacerHandle = nullptr;
#endif
            valid = false;
        }
    }
};

#else

class Spacer::Impl {
public:
    bool valid;
    double minWidth;
    double minHeight;
    double maxWidth;
    double maxHeight;
    
    Impl()
        : valid(false)
        , minWidth(0.0)
        , minHeight(0.0)
        , maxWidth(0.0)
        , maxHeight(0.0)
    {}
};

#endif

Spacer::Spacer() : pImpl(std::make_unique<Impl>()) {}

Spacer::~Spacer() {
    // Impl destructor handles cleanup
}

bool Spacer::create(double minWidth, double minHeight, double maxWidth, double maxHeight) {
    if (!pImpl) {
        return false;
    }
    
#ifdef __APPLE__
    if (pImpl->valid) {
        return false; // Already created
    }
    
    ObsidianSpacerParams params;
    params.minWidth = minWidth;
    params.minHeight = minHeight;
    params.maxWidth = maxWidth;
    params.maxHeight = maxHeight;
    
    pImpl->spacerHandle = obsidian_macos_create_spacer(params);
    if (!pImpl->spacerHandle) {
        return false;
    }
    
    pImpl->valid = true;
    pImpl->minWidth = minWidth;
    pImpl->minHeight = minHeight;
    pImpl->maxWidth = maxWidth;
    pImpl->maxHeight = maxHeight;
    
    return true;
#else
    // Not implemented on other platforms yet
    return false;
#endif
}

void Spacer::setMinWidth(double minWidth) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        obsidian_macos_spacer_set_min_width(pImpl->spacerHandle, minWidth);
    }
    pImpl->minWidth = minWidth;
#else
    pImpl->minWidth = minWidth;
#endif
}

double Spacer::getMinWidth() const {
    if (!pImpl) {
        return 0.0;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        return obsidian_macos_spacer_get_min_width(pImpl->spacerHandle);
    }
#endif
    return pImpl->minWidth;
}

void Spacer::setMinHeight(double minHeight) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        obsidian_macos_spacer_set_min_height(pImpl->spacerHandle, minHeight);
    }
    pImpl->minHeight = minHeight;
#else
    pImpl->minHeight = minHeight;
#endif
}

double Spacer::getMinHeight() const {
    if (!pImpl) {
        return 0.0;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        return obsidian_macos_spacer_get_min_height(pImpl->spacerHandle);
    }
#endif
    return pImpl->minHeight;
}

void Spacer::setMaxWidth(double maxWidth) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        obsidian_macos_spacer_set_max_width(pImpl->spacerHandle, maxWidth);
    }
    pImpl->maxWidth = maxWidth;
#else
    pImpl->maxWidth = maxWidth;
#endif
}

double Spacer::getMaxWidth() const {
    if (!pImpl) {
        return 0.0;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        return obsidian_macos_spacer_get_max_width(pImpl->spacerHandle);
    }
#endif
    return pImpl->maxWidth;
}

void Spacer::setMaxHeight(double maxHeight) {
    if (!pImpl) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        obsidian_macos_spacer_set_max_height(pImpl->spacerHandle, maxHeight);
    }
    pImpl->maxHeight = maxHeight;
#else
    pImpl->maxHeight = maxHeight;
#endif
}

double Spacer::getMaxHeight() const {
    if (!pImpl) {
        return 0.0;
    }
    
#ifdef __APPLE__
    if (pImpl->valid && pImpl->spacerHandle) {
        return obsidian_macos_spacer_get_max_height(pImpl->spacerHandle);
    }
#endif
    return pImpl->maxHeight;
}

bool Spacer::isValid() const {
    if (!pImpl) {
        return false;
    }
    
#ifdef __APPLE__
    if (pImpl->spacerHandle) {
        return obsidian_macos_spacer_is_valid(pImpl->spacerHandle);
    }
#endif
    return pImpl->valid;
}

void* Spacer::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    if (pImpl->spacerHandle) {
        return obsidian_macos_spacer_get_view(pImpl->spacerHandle);
    }
#endif
    return nullptr;
}

Spacer::Spacer(Spacer&&) noexcept = default;
Spacer& Spacer::operator=(Spacer&&) noexcept = default;

} // namespace obsidian
