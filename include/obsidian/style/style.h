/**
 * Obsidian Public API - Styling System
 * 
 * This is the main public API header for the styling system.
 * Provides declarative, composable styling modifiers.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 * 
 * Example usage:
 * 
 *     #include <obsidian/style/style.h>
 *     using namespace obsidian::style;
 *     
 *     Button button;
 *     button.create("Click Me", 0, 0, 100, 40)
 *         .style()
 *         .backgroundColor(Color::semantic("primary"))
 *         .foregroundColor(Color::semantic("on-primary"))
 *         .font(Font::body().weight(FontWeight::semibold))
 *         .padding(Padding::all(16))
 *         .borderRadius(BorderRadius::md())
 *         .shadow(Shadow::elevation(2));
 */

#pragma once

#include "color.h"
#include "font.h"
#include "theme.h"
#include "tokens.h"
#include "modifier.h"

namespace obsidian::style {

// ============================================================================
// Forward Declarations
// ============================================================================
class Style;
class Stylable;

// ============================================================================
// Padding
// ============================================================================

/**
 * Padding values for styling
 */
class Padding {
public:
    double top = 0;
    double right = 0;
    double bottom = 0;
    double left = 0;
    
    /**
     * Create padding with same value for all sides
     */
    static Padding all(double value);
    
    /**
     * Create padding with vertical and horizontal values
     */
    static Padding symmetric(double vertical, double horizontal);
    
    /**
     * Create padding with individual values
     */
    static Padding only(double top = 0, double right = 0, double bottom = 0, double left = 0);
};

// ============================================================================
// Border Radius
// ============================================================================

/**
 * Border radius values
 */
class BorderRadius {
public:
    double topLeft = 0;
    double topRight = 0;
    double bottomRight = 0;
    double bottomLeft = 0;
    
    /**
     * No border radius
     */
    static BorderRadius none();
    
    /**
     * Small border radius (4px)
     */
    static BorderRadius sm();
    
    /**
     * Medium border radius (8px)
     */
    static BorderRadius md();
    
    /**
     * Large border radius (12px)
     */
    static BorderRadius lg();
    
    /**
     * Extra large border radius (16px)
     */
    static BorderRadius xl();
    
    /**
     * Full border radius (circular)
     */
    static BorderRadius full();
    
    /**
     * Custom uniform border radius
     */
    static BorderRadius all(double value);
};

// ============================================================================
// Shadow
// ============================================================================

/**
 * Shadow/elevation values
 */
class Shadow {
public:
    double offsetX = 0;
    double offsetY = 0;
    double blurRadius = 0;
    double spreadRadius = 0;
    double opacity = 0.2;
    
    /**
     * No shadow
     */
    static Shadow none();
    
    /**
     * Small shadow
     */
    static Shadow sm();
    
    /**
     * Medium shadow
     */
    static Shadow md();
    
    /**
     * Large shadow
     */
    static Shadow lg();
    
    /**
     * Extra large shadow
     */
    static Shadow xl();
    
    /**
     * Material Design elevation (0-24)
     */
    static Shadow elevation(int level);
};

// ============================================================================
// Stylable Interface
// ============================================================================

/**
 * Interface for stylable components
 * 
 * All UI components that support styling implement this interface.
 */
class Stylable {
public:
    virtual ~Stylable() = default;
    
    /**
     * Get the style for this component
     */
    virtual Style& getStyle() = 0;
    virtual const Style& getStyle() const = 0;
    
    /**
     * Apply a style to this component
     */
    virtual void applyStyle(const Style& style) = 0;
};

// ============================================================================
// Style Class
// ============================================================================

/**
 * Style container class
 * 
 * Holds all styling properties for a component.
 */
class Style {
public:
    // Colors
    Color backgroundColor;
    Color foregroundColor;
    Color borderColor;
    
    // Typography
    Font font;
    
    // Spacing
    Padding padding;
    Padding margin;
    
    // Borders
    BorderRadius borderRadius;
    double borderWidth = 0;
    
    // Effects
    Shadow shadow;
    double opacity = 1.0;
    
    /**
     * Merge another style into this one
     * Properties from other override properties in this
     */
    void merge(const Style& other);
    
    /**
     * Create a copy of this style
     */
    Style copy() const;
};

} // namespace obsidian::style
