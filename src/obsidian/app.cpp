/**
 * Obsidian Public API - Application Implementation
 * 
 * This file implements the public App API by wrapping the internal
 * runtime implementation. Uses Pimpl pattern to avoid exposing internal types.
 */

#include "obsidian/app.h"

// Include internal headers ONLY in implementation (not exposed to users)
#include "platform.h"

#ifdef __APPLE__
#include "macos_ffi.h"
#endif

// Include internal Runtime definition
// Runtime uses AppCallbacks from public API (single source of truth)
#include "obsidian.h"

namespace obsidian {

class App::Impl {
public:
    // Use pointer to Runtime (Pimpl pattern)
    std::unique_ptr<::obsidian::Runtime> runtime;
    bool platformInitialized = false;
    
    Impl() : runtime(std::make_unique<::obsidian::Runtime>()) {}
    ~Impl() = default;
};

App::App() : pImpl(std::make_unique<Impl>()) {}

App::~App() {
    shutdown();
}

bool App::initialize() {
    if (pImpl->platformInitialized) {
        return true;
    }
    
    // Initialize platform
#ifdef __APPLE__
    if (!::obsidian::ffi::macos::initializePlatform()) {
        return false;
    }
#endif
    
    pImpl->platformInitialized = true;
    
    // Initialize runtime
    return pImpl->runtime->initialize();
}

void App::run(const AppCallbacks& callbacks) {
    if (!pImpl->platformInitialized) {
        if (!initialize()) {
            return;
        }
    }
    
    // AppCallbacks is defined in public API and used by internal Runtime
    // No conversion needed - they're the same type
    pImpl->runtime->run(callbacks);
    
#ifdef __APPLE__
    // On macOS, we need to run the event loop
    ::obsidian::ffi::macos::runEventLoop();
#endif
}

void App::shutdown() {
    if (!pImpl->platformInitialized) {
        return;
    }
    
    pImpl->runtime->shutdown();
    
#ifdef __APPLE__
    ::obsidian::ffi::macos::cleanupPlatform();
#endif
    
    pImpl->platformInitialized = false;
}

void App::enableHMR(bool enabled) {
    pImpl->runtime->enableHMR(enabled);
}

bool App::isHMREnabled() const {
    return pImpl->runtime->isHMREnabled();
}

App::App(App&& other) noexcept = default;
App& App::operator=(App&& other) noexcept = default;

} // namespace obsidian
