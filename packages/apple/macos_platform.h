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

/**
 * Set frame on any NSView
 * Used by the Layout Engine to apply computed layouts
 * 
 * @param viewHandle NSView* pointer
 * @param x X position relative to parent
 * @param y Y position relative to parent
 * @param width Width of the view
 * @param height Height of the view
 */
void obsidian_macos_view_set_frame(void* viewHandle, 
                                    double x, double y, 
                                    double width, double height);

/**
 * Get bounds of any NSView
 * Returns the view's bounds rectangle
 * 
 * @param viewHandle NSView* pointer
 * @param outX Pointer to receive X (origin x, typically 0)
 * @param outY Pointer to receive Y (origin y, typically 0)
 * @param outWidth Pointer to receive width
 * @param outHeight Pointer to receive height
 */
void obsidian_macos_view_get_bounds(void* viewHandle,
                                     double* outX, double* outY,
                                     double* outWidth, double* outHeight);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

