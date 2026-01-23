/**
 * Obsidian Public API - Icon (SF Symbols)
 *
 * This is the public API header for SF Symbols icon management.
 * Users should include <obsidian/obsidian.h> for the full API.
 *
 * Example usage:
 *
 *     Icon star;
 *     star.create("star.fill", 24.0);
 *     star.setTintColor(0xFFD700);  // Gold color
 *     star.addToWindow(window);
 *
 *     // With animation (macOS 14+)
 *     Icon heart;
 *     heart.create("heart.fill", 28.0);
 *     heart.addEffect({SymbolEffect::Pulse, -1, false, 1.0});  // Infinite pulse
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace obsidian {

// Forward declaration
class Window;

/**
 * Symbol weight - corresponds to NSFontWeight values.
 */
enum class SymbolWeight {
    UltraLight = 0,
    Thin,
    Light,
    Regular,
    Medium,
    Semibold,
    Bold,
    Heavy,
    Black
};

/**
 * Symbol rendering mode - controls how the symbol is colored.
 */
enum class SymbolRenderingMode {
    Automatic = 0,   // System default
    Monochrome,      // Single color
    Hierarchical,    // Primary color with hierarchy
    Palette,         // Multiple custom colors
    Multicolor       // SF Symbol's intrinsic colors
};

/**
 * Symbol effect types - available on macOS 14+.
 */
enum class SymbolEffect {
    Bounce = 0,      // Bouncing animation
    Pulse,           // Pulsing opacity animation
    Scale,           // Scale up/down animation
    Appear,          // Appearance transition
    Disappear,       // Disappearance transition
    Wiggle,          // Side-to-side wiggle
    Rotate,          // Rotation animation
    Breathe          // Breathing/pulsing animation
};

/**
 * Options for symbol effects.
 */
struct SymbolEffectOptions {
    SymbolEffect effect;     // The effect type
    int repeatCount;         // Number of repeats (-1 for infinite)
    bool byLayer;            // Whether to animate by layer
    double speed;            // Speed multiplier (1.0 = normal)
};

/**
 * Icon class - SF Symbols support for macOS
 *
 * This class provides a clean, high-level interface for rendering
 * SF Symbols with full support for configuration and effects.
 * Requires macOS 11+ for basic symbols, macOS 14+ for effects.
 */
class Icon {
public:
    /**
     * Create a new Icon instance.
     */
    Icon();

    /**
     * Destructor - automatically removes and destroys the icon.
     */
    ~Icon();

    /**
     * Create an icon with the given SF Symbol name.
     * @param symbolName SF Symbol name (e.g., "star.fill", "folder", "heart.fill")
     * @param pointSize Size in points (default: 17.0)
     * @param weight Symbol weight (default: Regular)
     * @param tintColor Tint color as ARGB or RGB uint32 (default: 0 = system color)
     * @return true if icon was created successfully
     */
    bool create(const std::string& symbolName,
                double pointSize = 17.0,
                SymbolWeight weight = SymbolWeight::Regular,
                uint32_t tintColor = 0);

    /**
     * Set the SF Symbol name.
     * @param symbolName System symbol name
     */
    void setSymbolName(const std::string& symbolName);

    /**
     * Get the current symbol name.
     */
    std::string getSymbolName() const;

    /**
     * Set the point size.
     * @param pointSize Size in points
     */
    void setPointSize(double pointSize);

    /**
     * Get the current point size.
     */
    double getPointSize() const;

    /**
     * Set the symbol weight.
     * @param weight Symbol weight
     */
    void setWeight(SymbolWeight weight);

    /**
     * Get the current weight.
     */
    SymbolWeight getWeight() const;

    /**
     * Set the tint color.
     * @param color ARGB or RGB color value (0 = system default)
     */
    void setTintColor(uint32_t color);

    /**
     * Get the current tint color.
     */
    uint32_t getTintColor() const;

    /**
     * Set the rendering mode.
     * @param mode Rendering mode
     */
    void setRenderingMode(SymbolRenderingMode mode);

    /**
     * Get the current rendering mode.
     */
    SymbolRenderingMode getRenderingMode() const;

    /**
     * Add a symbol effect (macOS 14+).
     * @param options Effect options
     */
    void addEffect(const SymbolEffectOptions& options);

    /**
     * Remove all symbol effects.
     */
    void removeAllEffects();

    /**
     * Set the frame position and size.
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Width in pixels
     * @param height Height in pixels
     */
    void setFrame(int x, int y, int width, int height);

    /**
     * Add icon to a window's content view.
     */
    void addToWindow(Window& window);

    /**
     * Remove icon from its parent view.
     */
    void removeFromParent();

    /**
     * Check if icon is valid.
     */
    bool isValid() const;

    /**
     * Get the native view handle (for internal use by layout system).
     * @return Platform-specific native view handle, or nullptr if invalid
     */
    void* getNativeViewHandle() const;

    // Non-copyable
    Icon(const Icon&) = delete;
    Icon& operator=(const Icon&) = delete;

    // Movable
    Icon(Icon&&) noexcept;
    Icon& operator=(Icon&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
