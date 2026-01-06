/**
 * Obsidian Public API - Theme System
 * 
 * Provides theme definition and management.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 * 
 * Example usage:
 * 
 *     Theme myTheme = Theme::create()
 *         .colors(ColorScheme::dark()
 *             .primary(Color::rgb(0x3B82F6))
 *             .background(Color::rgb(0x1F2937))
 *             .surface(Color::rgb(0x374151)))
 *         .typography(Typography::system()
 *             .body(Font::size(16).weight(FontWeight::regular))
 *             .heading(Font::size(24).weight(FontWeight::bold)))
 *         .spacing(SpacingScale::default());
 *     
 *     App app;
 *     app.setTheme(myTheme);
 */

#pragma once

#include "color.h"
#include "font.h"
#include <memory>
#include <string>

namespace obsidian::style {

// Forward declarations
class ColorScheme;
class Typography;
class SpacingScale;

/**
 * Theme appearance mode
 */
enum class ThemeMode {
    light,
    dark,
    system  // Follow system preference
};

/**
 * ColorScheme class - Defines color palette for a theme
 */
class ColorScheme {
public:
    /**
     * Create a light color scheme
     */
    static ColorScheme light();
    
    /**
     * Create a dark color scheme
     */
    static ColorScheme dark();
    
    /**
     * Create an adaptive color scheme that switches based on system preference
     */
    static ColorScheme adaptive();
    
    // Color setters (return reference for chaining)
    ColorScheme& primary(const Color& color);
    ColorScheme& secondary(const Color& color);
    ColorScheme& tertiary(const Color& color);
    ColorScheme& error(const Color& color);
    ColorScheme& warning(const Color& color);
    ColorScheme& success(const Color& color);
    ColorScheme& info(const Color& color);
    
    ColorScheme& background(const Color& color);
    ColorScheme& surface(const Color& color);
    ColorScheme& surfaceVariant(const Color& color);
    
    ColorScheme& onPrimary(const Color& color);
    ColorScheme& onSecondary(const Color& color);
    ColorScheme& onBackground(const Color& color);
    ColorScheme& onSurface(const Color& color);
    ColorScheme& onError(const Color& color);
    
    ColorScheme& outline(const Color& color);
    ColorScheme& shadow(const Color& color);
    
    // Adaptive setters (for system-switching themes)
    ColorScheme& lightScheme(const ColorScheme& scheme);
    ColorScheme& darkScheme(const ColorScheme& scheme);
    
    // Accessors
    Color getPrimary() const;
    Color getSecondary() const;
    Color getBackground() const;
    Color getSurface() const;
    Color getOnPrimary() const;
    Color getOnBackground() const;
    Color getOnSurface() const;
    // ... more accessors

private:
    struct Impl;
    std::shared_ptr<Impl> pImpl;
};

/**
 * Typography class - Defines typography scale for a theme
 */
class Typography {
public:
    /**
     * Create system typography (uses platform defaults)
     */
    static Typography system();
    
    /**
     * Create custom typography with specified font family
     */
    static Typography custom(const std::string& fontFamily);
    
    // Typography setters
    Typography& displayLarge(const Font& font);
    Typography& displayMedium(const Font& font);
    Typography& displaySmall(const Font& font);
    
    Typography& headlineLarge(const Font& font);
    Typography& headlineMedium(const Font& font);
    Typography& headlineSmall(const Font& font);
    
    Typography& titleLarge(const Font& font);
    Typography& titleMedium(const Font& font);
    Typography& titleSmall(const Font& font);
    
    Typography& bodyLarge(const Font& font);
    Typography& bodyMedium(const Font& font);
    Typography& bodySmall(const Font& font);
    
    Typography& labelLarge(const Font& font);
    Typography& labelMedium(const Font& font);
    Typography& labelSmall(const Font& font);
    
    // Shorthand setters
    Typography& heading(const Font& font);  // Sets headline styles
    Typography& body(const Font& font);     // Sets body styles
    Typography& caption(const Font& font);  // Sets label styles
    
    // Accessors
    Font getHeadlineLarge() const;
    Font getBodyMedium() const;
    // ... more accessors

private:
    struct Impl;
    std::shared_ptr<Impl> pImpl;
};

/**
 * SpacingScale class - Defines spacing values for a theme
 */
class SpacingScale {
public:
    /**
     * Create default spacing scale
     */
    static SpacingScale defaultScale();
    
    /**
     * Create compact spacing scale
     */
    static SpacingScale compact();
    
    /**
     * Create comfortable spacing scale
     */
    static SpacingScale comfortable();
    
    // Custom spacing setters
    SpacingScale& xs(double value);   // Extra small
    SpacingScale& sm(double value);   // Small
    SpacingScale& md(double value);   // Medium
    SpacingScale& lg(double value);   // Large
    SpacingScale& xl(double value);   // Extra large
    SpacingScale& xxl(double value);  // 2x extra large
    
    // Accessors
    double getXs() const;
    double getSm() const;
    double getMd() const;
    double getLg() const;
    double getXl() const;
    double getXxl() const;

private:
    double m_xs = 4;
    double m_sm = 8;
    double m_md = 16;
    double m_lg = 24;
    double m_xl = 32;
    double m_xxl = 48;
};

/**
 * Theme class - Complete theme definition
 * 
 * A theme combines color scheme, typography, and spacing to provide
 * a consistent visual design across your application.
 */
class Theme {
public:
    /**
     * Create a new theme builder
     */
    static Theme create();
    
    /**
     * Get the default light theme
     */
    static Theme light();
    
    /**
     * Get the default dark theme
     */
    static Theme dark();
    
    // Theme setters (return reference for chaining)
    Theme& colors(const ColorScheme& colorScheme);
    Theme& typography(const Typography& typography);
    Theme& spacing(const SpacingScale& spacing);
    Theme& mode(ThemeMode mode);
    
    // Accessors
    const ColorScheme& getColorScheme() const;
    const Typography& getTypography() const;
    const SpacingScale& getSpacing() const;
    ThemeMode getMode() const;
    
    // Resolve a color for current theme mode
    Color resolveColor(const std::string& semanticName) const;
    
    // Resolve a font for current theme mode
    Font resolveFont(const std::string& semanticName) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pImpl;
};

} // namespace obsidian::style
