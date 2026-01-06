/**
 * Obsidian Public API - Color System
 * 
 * Provides color representation and color utilities.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 */

#pragma once

#include <cstdint>
#include <string>

namespace obsidian::style {

/**
 * Color class - Platform-agnostic color representation
 * 
 * Supports RGB, RGBA, HSL, and semantic/design token colors.
 */
class Color {
public:
    // ========================================================================
    // Constructors
    // ========================================================================
    
    /**
     * Create a transparent color (default)
     */
    Color();
    
    /**
     * Create a color from RGB values (0-255)
     */
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    
    // ========================================================================
    // Static Factory Methods
    // ========================================================================
    
    /**
     * Create a color from RGB values (0-255)
     */
    static Color rgb(uint8_t r, uint8_t g, uint8_t b);
    
    /**
     * Create a color from RGBA values (0-255)
     */
    static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    /**
     * Create a color from a hex value (e.g., 0x3B82F6)
     */
    static Color rgb(uint32_t hex);
    
    /**
     * Create a color from a hex value with alpha (e.g., 0x3B82F680)
     */
    static Color rgba(uint32_t hex);
    
    /**
     * Create a color from a hex string (e.g., "#3B82F6" or "3B82F6")
     */
    static Color hex(const std::string& hexString);
    
    /**
     * Create a color from HSL values
     * @param h Hue (0-360)
     * @param s Saturation (0-100)
     * @param l Lightness (0-100)
     */
    static Color hsl(double h, double s, double l);
    
    /**
     * Create a color from HSLA values
     */
    static Color hsla(double h, double s, double l, double a);
    
    // ========================================================================
    // Semantic Colors (Design Tokens)
    // ========================================================================
    
    /**
     * Get a semantic color by name
     * @param name Semantic color name (e.g., "primary", "secondary", "error")
     */
    static Color semantic(const std::string& name);
    
    /**
     * Get a color from a design token
     * @param token Token path (e.g., "color.primary", "color.surface")
     */
    static Color token(const std::string& token);
    
    // ========================================================================
    // Platform Colors
    // ========================================================================
    
    /**
     * System blue color (adapts to platform)
     */
    static Color systemBlue();
    
    /**
     * System red color (adapts to platform)
     */
    static Color systemRed();
    
    /**
     * System green color (adapts to platform)
     */
    static Color systemGreen();
    
    /**
     * System orange color (adapts to platform)
     */
    static Color systemOrange();
    
    /**
     * System background color (adapts to light/dark mode)
     */
    static Color systemBackground();
    
    /**
     * System foreground color (adapts to light/dark mode)
     */
    static Color systemForeground();
    
    // ========================================================================
    // Predefined Colors
    // ========================================================================
    
    static Color white();
    static Color black();
    static Color transparent();
    static Color red();
    static Color green();
    static Color blue();
    static Color yellow();
    static Color cyan();
    static Color magenta();
    static Color gray();
    
    // Tailwind-style color palette
    static Color gray50();
    static Color gray100();
    static Color gray200();
    static Color gray300();
    static Color gray400();
    static Color gray500();
    static Color gray600();
    static Color gray700();
    static Color gray800();
    static Color gray900();
    
    static Color blue50();
    static Color blue100();
    static Color blue200();
    static Color blue300();
    static Color blue400();
    static Color blue500();
    static Color blue600();
    static Color blue700();
    static Color blue800();
    static Color blue900();
    
    // More color palettes to be added...
    
    // ========================================================================
    // Color Operations
    // ========================================================================
    
    /**
     * Get a lighter shade of this color
     * @param amount Amount to lighten (0-100)
     */
    Color lighten(double amount) const;
    
    /**
     * Get a darker shade of this color
     * @param amount Amount to darken (0-100)
     */
    Color darken(double amount) const;
    
    /**
     * Get this color with a different opacity
     * @param alpha Alpha value (0-255)
     */
    Color withAlpha(uint8_t alpha) const;
    
    /**
     * Get this color with a different opacity
     * @param opacity Opacity value (0.0-1.0)
     */
    Color withOpacity(double opacity) const;
    
    // ========================================================================
    // Accessors
    // ========================================================================
    
    uint8_t r() const;
    uint8_t g() const;
    uint8_t b() const;
    uint8_t a() const;
    
    /**
     * Get color as hex value
     */
    uint32_t toHex() const;
    
    /**
     * Get color as hex string
     */
    std::string toHexString() const;
    
    /**
     * Check if color is transparent
     */
    bool isTransparent() const;

private:
    uint8_t m_r = 0;
    uint8_t m_g = 0;
    uint8_t m_b = 0;
    uint8_t m_a = 255;
    
    // For semantic colors
    bool m_isSemantic = false;
    std::string m_semanticName;
};

} // namespace obsidian::style
