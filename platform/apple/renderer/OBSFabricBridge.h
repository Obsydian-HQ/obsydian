/**
 * OBSFabricBridge
 *
 * C interface for C++ code to interact with the Fabric platform layer.
 * This bridges the C++ renderer core to macOS-specific implementations.
 *
 * All view operations go through the MUTATION system.
 */

#ifndef OBS_FABRIC_BRIDGE_H
#define OBS_FABRIC_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Layout metrics for view positioning.
 */
typedef struct {
    float x;
    float y;
    float width;
    float height;
} OBSCLayoutMetrics;

/**
 * Mutation types.
 */
typedef enum {
    OBS_MUTATION_CREATE = 0,
    OBS_MUTATION_DELETE,
    OBS_MUTATION_INSERT,
    OBS_MUTATION_REMOVE,
    OBS_MUTATION_UPDATE
} OBSCMutationType;

/**
 * A single view mutation.
 */
typedef struct {
    OBSCMutationType type;
    int32_t tag;
    int32_t parentTag;
    int32_t index;
    const char* componentHandle;
    OBSCLayoutMetrics oldLayoutMetrics;
    OBSCLayoutMetrics newLayoutMetrics;
} OBSCViewMutation;

/**
 * Component type constants.
 */
extern const char* OBS_COMPONENT_VSTACK;
extern const char* OBS_COMPONENT_HSTACK;
extern const char* OBS_COMPONENT_ZSTACK;
extern const char* OBS_COMPONENT_BUTTON;
extern const char* OBS_COMPONENT_TEXT;
extern const char* OBS_COMPONENT_SPACER;
extern const char* OBS_COMPONENT_ROOT;
extern const char* OBS_COMPONENT_ICON;

/**
 * Initialize the Fabric platform layer.
 * Registers all component factories.
 * Call once during app startup.
 */
void obs_fabric_initialize(void);

/**
 * Shutdown the Fabric platform layer.
 */
void obs_fabric_shutdown(void);

/**
 * Apply a batch of mutations.
 * MUST be called on the main thread.
 */
void obs_fabric_apply_mutations(const OBSCViewMutation* mutations, int32_t count);

/**
 * Schedule mutations to be applied on the main thread.
 * Safe to call from any thread.
 */
void obs_fabric_schedule_mutations(const OBSCViewMutation* mutations, int32_t count);

/**
 * Generate a new unique tag.
 */
int32_t obs_fabric_generate_tag(void);

/**
 * Find a native view by tag.
 * Returns the NSView* (as void*) or NULL if not found.
 */
void* obs_fabric_find_view(int32_t tag);

/**
 * Set the root view for a surface (window content view).
 */
void obs_fabric_set_root_view(int64_t surfaceId, void* rootView);

/**
 * Button-specific property updates (called AFTER view is created via mutation).
 */
typedef void (*OBSButtonCallback)(void* userData);
void obs_fabric_button_set_title(int32_t tag, const char* title);
void obs_fabric_button_set_callback(int32_t tag, OBSButtonCallback callback, void* userData);

/**
 * Text-specific property updates (called AFTER view is created via mutation).
 */
void obs_fabric_text_set_content(int32_t tag, const char* text);

/**
 * Icon-specific property updates (called AFTER view is created via mutation).
 */
void obs_fabric_icon_set_symbol_name(int32_t tag, const char* name);
void obs_fabric_icon_set_point_size(int32_t tag, double size);
void obs_fabric_icon_set_weight(int32_t tag, int weight);
void obs_fabric_icon_set_tint_color(int32_t tag, uint32_t color);
void obs_fabric_icon_set_rendering_mode(int32_t tag, int mode);
void obs_fabric_icon_add_effect(int32_t tag, int type, int repeatCount, bool byLayer, double speed);
void obs_fabric_icon_remove_all_effects(int32_t tag);

/**
 * Add a view to window's content view.
 */
void obs_fabric_add_view_to_window(int32_t tag, void* windowHandle);

/**
 * Remove a view from its superview.
 */
void obs_fabric_remove_view_from_parent(int32_t tag);

#ifdef __cplusplus
}
#endif

#endif // OBS_FABRIC_BRIDGE_H
