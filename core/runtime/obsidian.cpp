/**
 * Obsidian Core Runtime Implementation
 * 
 * This is the heart of the Obsidian framework - manages the application lifecycle.
 */

#include "obsidian.h"
#include "obsidian/app.h"
#include <iostream>

#ifdef __APPLE__
// Fabric bridge - the new clean mounting system
extern "C" {
    void obs_fabric_initialize(void);
    void obs_fabric_shutdown(void);
}
#endif

namespace obsidian {

class Runtime::Impl {
public:
    bool initialized = false;
    bool hmrEnabled = false;
};

Runtime::Runtime() : pImpl(std::make_unique<Impl>()) {}

Runtime::~Runtime() {
    if (pImpl && pImpl->initialized) {
        shutdown();
    }
}

bool Runtime::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
#ifdef __APPLE__
    // Initialize the Fabric mounting system
    obs_fabric_initialize();
#endif
    
    pImpl->initialized = true;
    std::cout << "[Runtime] Obsidian runtime initialized" << std::endl;
    return true;
}

void Runtime::run(const AppCallbacks& callbacks) {
    if (!pImpl->initialized) {
        initialize();
    }
    
    // Call init callback
    if (callbacks.onInit) {
        callbacks.onInit();
    }
    
    // Call update callback (at least once for test purposes)
    if (callbacks.onUpdate) {
        callbacks.onUpdate();
    }
    
    // Main run loop is handled by platform code (NSApplication)
    // This is a placeholder for future cross-platform implementation
    
    // Call shutdown callback
    if (callbacks.onShutdown) {
        callbacks.onShutdown();
    }
}

void Runtime::shutdown() {
#ifdef __APPLE__
    // Shutdown the Fabric mounting system
    obs_fabric_shutdown();
#endif
    
    pImpl->initialized = false;
    std::cout << "[Runtime] Obsidian runtime shutdown" << std::endl;
}

void Runtime::enableHMR(bool enabled) {
    pImpl->hmrEnabled = enabled;
}

bool Runtime::isHMREnabled() const {
    return pImpl->hmrEnabled;
}

int run(int /* argc */, char* /* argv */[], const AppCallbacks& /* callbacks */) {
    // Placeholder for main entry point
    return 0;
}

} // namespace obsidian
