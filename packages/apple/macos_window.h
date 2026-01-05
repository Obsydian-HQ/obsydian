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

/**
 * Update window constraints to prevent shrinking in macOS 15+ (Sequoia)
 * In macOS 15+, layout is computed more lazily, and the window may resize
 * based on contentView's fittingSize. This function ensures constraints are updated.
 */
void obsidian_macos_window_update_constraints(ObsidianWindowHandle handle);

/**
 * Set the content view controller for the window
 * This is REQUIRED for native sidebar behavior with NSSplitViewController:
 * - Collapse button appears automatically
 * - Proper window resizing behavior
 * - Correct view controller lifecycle management
 * 
 * @param handle Window handle
 * @param viewControllerHandle View controller handle (NSViewController*)
 */
void obsidian_macos_window_set_content_view_controller(ObsidianWindowHandle handle, void* viewControllerHandle);

/**
 * Toolbar style enumeration
 */
typedef enum {
    ObsidianWindowToolbarStyleAutomatic = 0,          // Automatic based on window
    ObsidianWindowToolbarStyleExpanded = 1,           // Expanded toolbar
    ObsidianWindowToolbarStylePreference = 2,         // Preference window style
    ObsidianWindowToolbarStyleUnified = 3,            // Unified with title bar
    ObsidianWindowToolbarStyleUnifiedCompact = 4      // Compact unified with title bar
} ObsidianWindowToolbarStyle;

/**
 * Set the toolbar for the window
 * This is REQUIRED for native sidebar collapse button functionality.
 * The toolbar must contain NSToolbarToggleSidebarItemIdentifier.
 * 
 * @param handle Window handle
 * @param toolbarHandle Toolbar handle (from obsidian_macos_create_toolbar)
 */
void obsidian_macos_window_set_toolbar(ObsidianWindowHandle handle, void* toolbarHandle);

/**
 * Get the toolbar from the window
 * @param handle Window handle
 * @return Toolbar handle (NSToolbar*), or NULL if no toolbar is set
 */
void* obsidian_macos_window_get_toolbar(ObsidianWindowHandle handle);

/**
 * Set the window's toolbar style (macOS 11+)
 * For proper sidebar integration, use UnifiedCompact or Unified.
 * 
 * @param handle Window handle
 * @param style Toolbar style
 */
void obsidian_macos_window_set_toolbar_style(ObsidianWindowHandle handle, ObsidianWindowToolbarStyle style);

/**
 * Get the window's toolbar style (macOS 11+)
 * @param handle Window handle
 * @return Current toolbar style
 */
ObsidianWindowToolbarStyle obsidian_macos_window_get_toolbar_style(ObsidianWindowHandle handle);

/**
 * Titlebar separator style enumeration
 */
typedef enum {
    ObsidianWindowTitlebarSeparatorStyleAutomatic = 0,
    ObsidianWindowTitlebarSeparatorStyleNone = 1,
    ObsidianWindowTitlebarSeparatorStyleLine = 2,
    ObsidianWindowTitlebarSeparatorStyleShadow = 3
} ObsidianWindowTitlebarSeparatorStyle;

/**
 * Set the window's titlebar separator style (macOS 11+)
 * For sidebar windows, use Automatic to let the system decide based on split view items.
 * 
 * @param handle Window handle
 * @param style Titlebar separator style
 */
void obsidian_macos_window_set_titlebar_separator_style(ObsidianWindowHandle handle, ObsidianWindowTitlebarSeparatorStyle style);

/**
 * Get the window's titlebar separator style (macOS 11+)
 * @param handle Window handle
 * @return Current titlebar separator style
 */
ObsidianWindowTitlebarSeparatorStyle obsidian_macos_window_get_titlebar_separator_style(ObsidianWindowHandle handle);

/**
 * Configure the window for native sidebar integration.
 * This enables the sidebar to extend into the titlebar area with traffic lights.
 * 
 * MUST be called BEFORE adding a sidebar/split view controller to the window.
 * 
 * This function:
 * - Adds NSWindowStyleMaskFullSizeContentView to allow content under titlebar
 * - Sets titleVisibility = NSWindowTitleHidden
 * - Sets titlebarAppearsTransparent = YES (for seamless sidebar integration)
 * 
 * @param handle Window handle
 */
void obsidian_macos_window_configure_for_sidebar(ObsidianWindowHandle handle);

#ifdef __cplusplus
}
#endif

