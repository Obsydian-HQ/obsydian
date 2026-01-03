/**
 * Obsidian Runtime - Core Implementation
 */

#include "obsidian.h"
// Include public API for AppCallbacks definition (single source of truth)
#include "obsidian/app.h"
#include <iostream>

namespace obsidian {

class Runtime::Impl {
public:
    bool hmrEnabled = false;
    bool initialized = false;
};

Runtime::Runtime() : pImpl(std::make_unique<Impl>()) {}

Runtime::~Runtime() = default;

bool Runtime::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    pImpl->initialized = true;
    std::cout << "[Obsidian] Runtime initialized\n";
    return true;
}

void Runtime::run(const AppCallbacks& callbacks) {
    if (!pImpl->initialized) {
        if (!initialize()) {
            return;
        }
    }

    if (callbacks.onInit) {
        callbacks.onInit();
    }

    // Main loop
    bool running = true;
    while (running) {
        if (callbacks.onUpdate) {
            callbacks.onUpdate();
        }
        
        // Platform-specific event loop integration will go here
        // For now, this is a placeholder
        break;
    }

    if (callbacks.onShutdown) {
        callbacks.onShutdown();
    }
}

void Runtime::shutdown() {
    pImpl->initialized = false;
    std::cout << "[Obsidian] Runtime shutdown\n";
}

void Runtime::enableHMR(bool enabled) {
    pImpl->hmrEnabled = enabled;
    std::cout << "[Obsidian] HMR " << (enabled ? "enabled" : "disabled") << "\n";
}

bool Runtime::isHMREnabled() const {
    return pImpl->hmrEnabled;
}

int run(int /* argc */, char* /* argv */[], const AppCallbacks& callbacks) {
    Runtime runtime;
    
    if (!runtime.initialize()) {
        return 1;
    }

    runtime.run(callbacks);
    runtime.shutdown();
    
    return 0;
}

} // namespace obsidian

