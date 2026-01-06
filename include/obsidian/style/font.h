/**
 * Obsidian Public API - Font/Typography System
 * 
 * Provides font and typography utilities.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 */

#pragma once

#include <string>

namespace obsidian::style {

/**
 * Font weight options
 */
enum class FontWeight {
    thin,       // 100
    extraLight, // 200
    light,      // 300
    regular,    // 400
    medium,     // 500
    semibold,   // 600
    bold,       // 700
    extraBold,  // 800
    black       // 900
};

/**
 * Font style options
 */
enum class FontStyle {
    normal,
    italic,
    oblique
};

/**
 * Text alignment options
 */
enum class TextAlignment {
    left,
    center,
    right,
    justified
};

/**
 * Text decoration options
 */
enum class TextDecoration {
    none,
    underline,
    strikethrough,
    overline
};

/**
 * Font class - Platform-agnostic font representation
 * 
 * Supports system fonts, custom fonts, and typography tokens.
 */
class Font {
public:
    // ========================================================================
    // Constructors
    // ========================================================================
    
    /**
     * Create a default font
     */
    Font();
    
    // ========================================================================
    // Static Factory Methods - System Fonts
    // ========================================================================
    
    /**
     * System default font
     */
    static Font system();
    
    /**
     * System monospace font
     */
    static Font mono();
    
    /**
     * System serif font
     */
    static Font serif();
    
    /**
     * System rounded font (platform-specific)
     */
    static Font rounded();
    
    // ========================================================================
    // Static Factory Methods - Semantic Fonts
    // ========================================================================
    
    /**
     * Large title font (typically 34pt)
     */
    static Font largeTitle();
    
    /**
     * Title 1 font (typically 28pt)
     */
    static Font title1();
    
    /**
     * Title 2 font (typically 22pt)
     */
    static Font title2();
    
    /**
     * Title 3 font (typically 20pt)
     */
    static Font title3();
    
    /**
     * Headline font (typically 17pt semibold)
     */
    static Font headline();
    
    /**
     * Subheadline font (typically 15pt)
     */
    static Font subheadline();
    
    /**
     * Body font (typically 17pt)
     */
    static Font body();
    
    /**
     * Callout font (typically 16pt)
     */
    static Font callout();
    
    /**
     * Footnote font (typically 13pt)
     */
    static Font footnote();
    
    /**
     * Caption 1 font (typically 12pt)
     */
    static Font caption1();
    
    /**
     * Caption 2 font (typically 11pt)
     */
    static Font caption2();
    
    // ========================================================================
    // Static Factory Methods - Custom
    // ========================================================================
    
    /**
     * Create a font with a custom family
     */
    static Font family(const std::string& name);
    
    /**
     * Create a font with a specific size
     */
    static Font size(double size);
    
    /**
     * Get a font from a design token
     */
    static Font token(const std::string& token);
    
    // ========================================================================
    // Modifiers (Return new Font with modification)
    // ========================================================================
    
    /**
     * Set font weight
     */
    Font weight(FontWeight weight) const;
    
    /**
     * Set font style
     */
    Font style(FontStyle style) const;
    
    /**
     * Set font size
     */
    Font size(double size) const;
    
    /**
     * Set line height
     */
    Font lineHeight(double lineHeight) const;
    
    /**
     * Set letter spacing
     */
    Font letterSpacing(double spacing) const;
    
    /**
     * Make font bold
     */
    Font bold() const;
    
    /**
     * Make font italic
     */
    Font italic() const;
    
    // ========================================================================
    // Accessors
    // ========================================================================
    
    std::string getFamilyName() const;
    double getSize() const;
    FontWeight getWeight() const;
    FontStyle getStyle() const;
    double getLineHeight() const;
    double getLetterSpacing() const;

private:
    std::string m_familyName;
    double m_size = 17.0;
    FontWeight m_weight = FontWeight::regular;
    FontStyle m_style = FontStyle::normal;
    double m_lineHeight = 1.2;
    double m_letterSpacing = 0.0;
};

} // namespace obsidian::style
