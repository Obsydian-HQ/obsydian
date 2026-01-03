/**
 * Obsidian FFI - Platform Implementation
 */

#include "platform.h"

#ifdef __APPLE__
#include "packages/apple/macos_ffi.h"
#endif

namespace obsidian::ffi {

Platform getCurrentPlatform() {
#if defined(__APPLE__)
    #if TARGET_OS_IPHONE
        return Platform::iOS;
    #else
        return Platform::macOS;
    #endif
#elif defined(__ANDROID__)
    return Platform::Android;
#elif defined(_WIN32) || defined(_WIN64)
    return Platform::Windows;
#elif defined(__linux__)
    return Platform::Linux;
#else
    return Platform::Unknown;
#endif
}

bool initializePlatform() {
    Platform platform = getCurrentPlatform();
    
    switch (platform) {
        case Platform::iOS:
        case Platform::macOS: {
            // Apple-specific initialization
            #ifdef __APPLE__
                return obsidian::ffi::macos::initializePlatform();
            #endif
            return true;
        }
        case Platform::Android:
            // Android-specific initialization
            break;
        case Platform::Windows:
            // Windows-specific initialization
            break;
        case Platform::Linux:
            // Linux-specific initialization
            break;
        case Platform::Unknown:
            return false;
    }
    
    return true;
}

void cleanupPlatform() {
    Platform platform = getCurrentPlatform();
    
    switch (platform) {
        case Platform::iOS:
        case Platform::macOS: {
            #ifdef __APPLE__
                obsidian::ffi::macos::cleanupPlatform();
            #endif
            break;
        }
        case Platform::Android:
            // Android-specific cleanup
            break;
        case Platform::Windows:
            // Windows-specific cleanup
            break;
        case Platform::Linux:
            // Linux-specific cleanup
            break;
        case Platform::Unknown:
            break;
    }
}

} // namespace obsidian::ffi

