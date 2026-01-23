/**
 * Obsidian Public API - Icon Implementation
 *
 * Uses the Fabric mutation system for view lifecycle.
 * Views are created via Create mutation, properties set via bridge functions.
 */

#include "obsidian/ui/icon.h"
#include "obsidian/window.h"
#include <iostream>

#ifdef __APPLE__
// Fabric bridge - the new clean system
extern "C" {
    void obs_fabric_initialize(void);
    int32_t obs_fabric_generate_tag(void);
    void* obs_fabric_find_view(int32_t tag);

    typedef struct {
        float x, y, width, height;
    } OBSCLayoutMetrics;

    typedef enum {
        OBS_MUTATION_CREATE = 0,
        OBS_MUTATION_DELETE,
        OBS_MUTATION_INSERT,
        OBS_MUTATION_REMOVE,
        OBS_MUTATION_UPDATE
    } OBSCMutationType;

    typedef struct {
        OBSCMutationType type;
        int32_t tag;
        int32_t parentTag;
        int32_t index;
        const char* componentHandle;
        OBSCLayoutMetrics oldLayoutMetrics;
        OBSCLayoutMetrics newLayoutMetrics;
    } OBSCViewMutation;

    void obs_fabric_apply_mutations(const OBSCViewMutation* mutations, int32_t count);

    extern const char* OBS_COMPONENT_ICON;

    void obs_fabric_icon_set_symbol_name(int32_t tag, const char* name);
    void obs_fabric_icon_set_point_size(int32_t tag, double size);
    void obs_fabric_icon_set_weight(int32_t tag, int weight);
    void obs_fabric_icon_set_tint_color(int32_t tag, uint32_t color);
    void obs_fabric_icon_set_rendering_mode(int32_t tag, int mode);
    void obs_fabric_icon_add_effect(int32_t tag, int type, int repeatCount, bool byLayer, double speed);
    void obs_fabric_icon_remove_all_effects(int32_t tag);
    void obs_fabric_add_view_to_window(int32_t tag, void* windowHandle);
    void obs_fabric_remove_view_from_parent(int32_t tag);
}
#endif

namespace obsidian {

class Icon::Impl {
public:
    int32_t tag = 0;
    bool valid = false;

    // Cached properties
    std::string symbolName;
    double pointSize = 17.0;
    SymbolWeight weight = SymbolWeight::Regular;
    uint32_t tintColor = 0;
    SymbolRenderingMode renderingMode = SymbolRenderingMode::Automatic;

    void* getNativeView() const {
#ifdef __APPLE__
        if (tag == 0) return nullptr;
        return obs_fabric_find_view(tag);
#else
        return nullptr;
#endif
    }
};

Icon::Icon() : pImpl(std::make_unique<Impl>()) {}

Icon::~Icon() {
    // Views are managed by ComponentViewRegistry - don't destroy here
    // When we need proper cleanup, we'll send a Delete mutation
    pImpl->valid = false;
}

bool Icon::create(const std::string& symbolName,
                  double pointSize,
                  SymbolWeight weight,
                  uint32_t tintColor) {
    if (pImpl->valid) {
        return false;
    }

#ifdef __APPLE__
    // Generate unique tag
    pImpl->tag = obs_fabric_generate_tag();

    // Create icon via mutation
    OBSCViewMutation createMutation = {};
    createMutation.type = OBS_MUTATION_CREATE;
    createMutation.tag = pImpl->tag;
    createMutation.componentHandle = OBS_COMPONENT_ICON;

    obs_fabric_apply_mutations(&createMutation, 1);

    // Verify view was created
    void* nativeView = pImpl->getNativeView();
    if (!nativeView) {
        std::cerr << "[Icon] Failed to create native view for tag " << pImpl->tag << std::endl;
        pImpl->tag = 0;
        return false;
    }

    // Store properties
    pImpl->symbolName = symbolName;
    pImpl->pointSize = pointSize;
    pImpl->weight = weight;
    pImpl->tintColor = tintColor;

    // Set symbol name
    obs_fabric_icon_set_symbol_name(pImpl->tag, symbolName.c_str());

    // Set point size
    obs_fabric_icon_set_point_size(pImpl->tag, pointSize);

    // Set weight
    obs_fabric_icon_set_weight(pImpl->tag, static_cast<int>(weight));

    // Set tint color if specified
    if (tintColor != 0) {
        obs_fabric_icon_set_tint_color(pImpl->tag, tintColor);
    }
#endif

    pImpl->valid = true;
    return true;
}

void Icon::setSymbolName(const std::string& symbolName) {
    pImpl->symbolName = symbolName;
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_set_symbol_name(pImpl->tag, symbolName.c_str());
#endif
}

std::string Icon::getSymbolName() const {
    return pImpl->symbolName;
}

void Icon::setPointSize(double pointSize) {
    pImpl->pointSize = pointSize;
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_set_point_size(pImpl->tag, pointSize);
#endif
}

double Icon::getPointSize() const {
    return pImpl->pointSize;
}

void Icon::setWeight(SymbolWeight weight) {
    pImpl->weight = weight;
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_set_weight(pImpl->tag, static_cast<int>(weight));
#endif
}

SymbolWeight Icon::getWeight() const {
    return pImpl->weight;
}

void Icon::setTintColor(uint32_t color) {
    pImpl->tintColor = color;
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_set_tint_color(pImpl->tag, color);
#endif
}

uint32_t Icon::getTintColor() const {
    return pImpl->tintColor;
}

void Icon::setRenderingMode(SymbolRenderingMode mode) {
    pImpl->renderingMode = mode;
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_set_rendering_mode(pImpl->tag, static_cast<int>(mode));
#endif
}

SymbolRenderingMode Icon::getRenderingMode() const {
    return pImpl->renderingMode;
}

void Icon::addEffect(const SymbolEffectOptions& options) {
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_add_effect(
        pImpl->tag,
        static_cast<int>(options.effect),
        options.repeatCount,
        options.byLayer,
        options.speed
    );
#endif
}

void Icon::removeAllEffects() {
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_icon_remove_all_effects(pImpl->tag);
#endif
}

void Icon::setFrame(int x, int y, int width, int height) {
    if (!pImpl->valid) return;

#ifdef __APPLE__
    // Set frame via Update mutation
    OBSCViewMutation updateMutation = {};
    updateMutation.type = OBS_MUTATION_UPDATE;
    updateMutation.tag = pImpl->tag;
    updateMutation.newLayoutMetrics = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    obs_fabric_apply_mutations(&updateMutation, 1);
#endif
}

void Icon::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) return;

#ifdef __APPLE__
    void* windowHandle = window.getNativeHandle();
    if (windowHandle) {
        obs_fabric_add_view_to_window(pImpl->tag, windowHandle);
    }
#endif
}

void Icon::removeFromParent() {
    if (!pImpl->valid) return;

#ifdef __APPLE__
    obs_fabric_remove_view_from_parent(pImpl->tag);
#endif
}

bool Icon::isValid() const {
    return pImpl->valid;
}

void* Icon::getNativeViewHandle() const {
    if (!pImpl->valid) return nullptr;
    return pImpl->getNativeView();
}

Icon::Icon(Icon&& other) noexcept = default;
Icon& Icon::operator=(Icon&& other) noexcept = default;

} // namespace obsidian
