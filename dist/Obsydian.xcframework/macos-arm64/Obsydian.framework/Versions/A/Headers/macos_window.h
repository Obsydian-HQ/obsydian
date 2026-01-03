/**
 * macOS Window FFI - C Interface
 * 
 * Provides C interface for creating and managing NSWindow instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS window
 */
typedef void* ObsidianWindowHandle;

/**
 * Window creation parameters
 */
typedef struct {
    int width;
    int height;
    const char* title;
} ObsidianWindowParams;

/**
 * Create a new macOS window
 * Returns a handle to the window, or NULL on failure
 */
ObsidianWindowHandle obsidian_macos_create_window(ObsidianWindowParams params);

/**
 * Show the window
 */
void obsidian_macos_show_window(ObsidianWindowHandle handle);

/**
 * Close and destroy the window
 */
void obsidian_macos_destroy_window(ObsidianWindowHandle handle);

/**
 * Check if window is valid
 */
bool obsidian_macos_window_is_valid(ObsidianWindowHandle handle);

/**
 * Get the content view from a window handle
 * Returns an opaque pointer to the NSView content view, or NULL on failure
 * This is for internal use by other FFI bindings (e.g., buttons)
 */
void* obsidian_macos_window_get_content_view(ObsidianWindowHandle handle);

#ifdef __cplusplus
}
#endif

