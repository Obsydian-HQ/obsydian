/**
 * Obsidian Public API - Style Modifiers
 * 
 * Provides chainable style modifiers for components.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 * 
 * Example usage:
 * 
 *     button.style()
 *         .p(16)           // padding: 16
 *         .px(24)          // padding-x: 24
 *         .bgBlue500()     // background: blue-500
 *         .textWhite()     // text: white
 *         .roundedLg()     // border-radius: large
 *         .shadowMd();     // shadow: medium
 */

#pragma once

#include "color.h"
#include "font.h"
#include "style.h"

namespace obsidian::style {

/**
 * Base style modifier template
 * 
 * Provides a chainable interface for styling components.
 * Each component type can specialize this template.
 */
template <typename T>
class StyleModifier {
public:
    explicit StyleModifier(T& component) : m_component(component) {}
    
    // ========================================================================
    // Color Modifiers
    // ========================================================================
    
    StyleModifier& backgroundColor(const Color& color);
    StyleModifier& foregroundColor(const Color& color);
    StyleModifier& borderColor(const Color& color);
    
    // Tailwind-style background colors
    StyleModifier& bgWhite();
    StyleModifier& bgBlack();
    StyleModifier& bgTransparent();
    
    StyleModifier& bgGray50();
    StyleModifier& bgGray100();
    StyleModifier& bgGray200();
    StyleModifier& bgGray300();
    StyleModifier& bgGray400();
    StyleModifier& bgGray500();
    StyleModifier& bgGray600();
    StyleModifier& bgGray700();
    StyleModifier& bgGray800();
    StyleModifier& bgGray900();
    
    StyleModifier& bgBlue50();
    StyleModifier& bgBlue100();
    StyleModifier& bgBlue200();
    StyleModifier& bgBlue300();
    StyleModifier& bgBlue400();
    StyleModifier& bgBlue500();
    StyleModifier& bgBlue600();
    StyleModifier& bgBlue700();
    StyleModifier& bgBlue800();
    StyleModifier& bgBlue900();
    
    StyleModifier& bgRed500();
    StyleModifier& bgGreen500();
    StyleModifier& bgYellow500();
    StyleModifier& bgPurple500();
    StyleModifier& bgPink500();
    StyleModifier& bgIndigo500();
    StyleModifier& bgTeal500();
    StyleModifier& bgOrange500();
    
    // Tailwind-style text colors
    StyleModifier& textWhite();
    StyleModifier& textBlack();
    StyleModifier& textGray500();
    StyleModifier& textBlue500();
    StyleModifier& textRed500();
    StyleModifier& textGreen500();
    
    // ========================================================================
    // Typography Modifiers
    // ========================================================================
    
    StyleModifier& font(const Font& font);
    StyleModifier& fontSize(double size);
    StyleModifier& fontWeight(FontWeight weight);
    StyleModifier& fontBold();
    StyleModifier& fontSemibold();
    StyleModifier& fontMedium();
    StyleModifier& fontNormal();
    StyleModifier& fontLight();
    
    StyleModifier& textLeft();
    StyleModifier& textCenter();
    StyleModifier& textRight();
    
    // ========================================================================
    // Spacing Modifiers (Tailwind-style)
    // ========================================================================
    
    // Padding
    StyleModifier& p(double value);   // all sides
    StyleModifier& px(double value);  // horizontal
    StyleModifier& py(double value);  // vertical
    StyleModifier& pt(double value);  // top
    StyleModifier& pr(double value);  // right
    StyleModifier& pb(double value);  // bottom
    StyleModifier& pl(double value);  // left
    
    StyleModifier& padding(const Padding& padding);
    
    // Margin
    StyleModifier& m(double value);   // all sides
    StyleModifier& mx(double value);  // horizontal
    StyleModifier& my(double value);  // vertical
    StyleModifier& mt(double value);  // top
    StyleModifier& mr(double value);  // right
    StyleModifier& mb(double value);  // bottom
    StyleModifier& ml(double value);  // left
    
    StyleModifier& margin(const Padding& margin);
    
    // ========================================================================
    // Border Modifiers
    // ========================================================================
    
    StyleModifier& borderRadius(double radius);
    StyleModifier& borderRadius(const BorderRadius& radius);
    StyleModifier& borderWidth(double width);
    
    // Tailwind-style border radius
    StyleModifier& roundedNone();
    StyleModifier& roundedSm();
    StyleModifier& rounded();     // md
    StyleModifier& roundedMd();
    StyleModifier& roundedLg();
    StyleModifier& roundedXl();
    StyleModifier& roundedFull();
    
    // ========================================================================
    // Shadow Modifiers
    // ========================================================================
    
    StyleModifier& shadow(const Shadow& shadow);
    
    StyleModifier& shadowNone();
    StyleModifier& shadowSm();
    StyleModifier& shadowMd();
    StyleModifier& shadowLg();
    StyleModifier& shadowXl();
    
    // ========================================================================
    // Opacity Modifiers
    // ========================================================================
    
    StyleModifier& opacity(double value);
    StyleModifier& opacity0();
    StyleModifier& opacity25();
    StyleModifier& opacity50();
    StyleModifier& opacity75();
    StyleModifier& opacity100();
    
    // ========================================================================
    // Apply and Get
    // ========================================================================
    
    /**
     * Apply the accumulated styles to the component
     */
    T& apply();
    
    /**
     * Get the accumulated style
     */
    const Style& getStyle() const;
    
    /**
     * Conversion operator to allow chaining back to component
     */
    operator T&();

protected:
    T& m_component;
    Style m_style;
};

} // namespace obsidian::style
