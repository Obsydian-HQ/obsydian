/**
 * macOS Platform FFI - C Interface
 * 
 * Provides C interface for macOS platform initialization and window management
 * This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#ifdef __cplusplus
#include <cstdint>
#include <cstdbool>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

/**
 * Initialize macOS platform (NSApplication, etc.)
 * Must be called before creating windows
 */
bool obsidian_macos_platform_init();

/**
 * Run the macOS event loop
 * This blocks until the application terminates
 */
void obsidian_macos_platform_run();

/**
 * Stop the macOS event loop
 */
void obsidian_macos_platform_stop();

/**
 * Cleanup macOS platform resources
 */
void obsidian_macos_platform_cleanup();

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

