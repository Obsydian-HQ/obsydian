/**
 * macOS Mounting Implementation
 * 
 * Registers view factories and implements platform delegate for macOS
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the macOS mounting system
 * Registers all view factories and sets up the platform delegate
 * Must be called once at app startup
 */
void obsidian_macos_mounting_initialize(void);

/**
 * Shutdown the macOS mounting system
 * Cleans up all views
 */
void obsidian_macos_mounting_shutdown(void);

#ifdef __cplusplus
}
#endif
