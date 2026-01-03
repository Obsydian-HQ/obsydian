/**
 * Obsidian FFI - Platform Abstraction Layer
 * 
 * Provides unified interface for accessing native platform APIs
 * across iOS, macOS, Android, Windows, and Linux.
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace obsidian::ffi {

/**
 * Platform identifier
 */
enum class Platform {
    Unknown,
    iOS,
    macOS,
    Android,
    Windows,
    Linux,
};

/**
 * Get current platform
 */
Platform getCurrentPlatform();

/**
 * Platform-specific initialization
 */
bool initializePlatform();

/**
 * Platform-specific cleanup
 */
void cleanupPlatform();

/**
 * Base class for platform-specific implementations
 */
class PlatformImpl {
public:
    virtual ~PlatformImpl() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual Platform getPlatform() const = 0;
};

} // namespace obsidian::ffi

