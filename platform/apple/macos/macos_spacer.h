/**
 * macOS Spacer FFI - C Interface
 * 
 * Provides C interface for creating and managing flexible spacer views
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS spacer view
 */
typedef void* ObsidianSpacerHandle;

/**
 * Spacer creation parameters
 */
typedef struct {
    double minWidth;
    double minHeight;
    double maxWidth;
    double maxHeight;
} ObsidianSpacerParams;

/**
 * Create a new macOS spacer view
 * Returns a handle to the spacer, or NULL on failure
 */
ObsidianSpacerHandle obsidian_macos_create_spacer(ObsidianSpacerParams params);

/**
 * Set minimum width constraint
 * @param minWidth Minimum width in points (0.0 to remove constraint)
 */
void obsidian_macos_spacer_set_min_width(ObsidianSpacerHandle handle, double minWidth);

/**
 * Get minimum width
 */
double obsidian_macos_spacer_get_min_width(ObsidianSpacerHandle handle);

/**
 * Set minimum height constraint
 * @param minHeight Minimum height in points (0.0 to remove constraint)
 */
void obsidian_macos_spacer_set_min_height(ObsidianSpacerHandle handle, double minHeight);

/**
 * Get minimum height
 */
double obsidian_macos_spacer_get_min_height(ObsidianSpacerHandle handle);

/**
 * Set maximum width constraint
 * @param maxWidth Maximum width in points (0.0 to remove constraint)
 */
void obsidian_macos_spacer_set_max_width(ObsidianSpacerHandle handle, double maxWidth);

/**
 * Get maximum width
 */
double obsidian_macos_spacer_get_max_width(ObsidianSpacerHandle handle);

/**
 * Set maximum height constraint
 * @param maxHeight Maximum height in points (0.0 to remove constraint)
 */
void obsidian_macos_spacer_set_max_height(ObsidianSpacerHandle handle, double maxHeight);

/**
 * Get maximum height
 */
double obsidian_macos_spacer_get_max_height(ObsidianSpacerHandle handle);

/**
 * Get the NSView handle for the spacer
 * Returns NULL if handle is invalid
 */
void* obsidian_macos_spacer_get_view(ObsidianSpacerHandle handle);

/**
 * Close and destroy the spacer
 */
void obsidian_macos_destroy_spacer(ObsidianSpacerHandle handle);

/**
 * Check if spacer handle is valid
 */
bool obsidian_macos_spacer_is_valid(ObsidianSpacerHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
