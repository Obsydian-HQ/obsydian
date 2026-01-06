/**
 * Obsidian Public API - Design Tokens
 * 
 * Provides access to design tokens for styling.
 * 
 * STATUS: Placeholder - Implementation pending
 * See docs/styling/STYLING_SYSTEM_VISION.md for architecture details.
 */

#pragma once

#include <string>
#include <variant>

namespace obsidian::style {

/**
 * Design token value types
 */
using TokenValue = std::variant<
    double,
    std::string,
    bool
>;

/**
 * DesignTokens class - Access to design tokens
 * 
 * Design tokens are the foundation of the styling system.
 * They provide platform-agnostic semantic values that can be
 * transformed for each platform.
 */
class DesignTokens {
public:
    // ========================================================================
    // Token Resolution
    // ========================================================================
    
    /**
     * Get a token value by path
     * @param path Token path (e.g., "color.primary", "spacing.md")
     * @return Token value, or default if not found
     */
    static TokenValue get(const std::string& path);
    
    /**
     * Get a color token value
     * @param path Token path (e.g., "color.primary")
     * @return Hex color value
     */
    static uint32_t getColor(const std::string& path);
    
    /**
     * Get a spacing token value
     * @param path Token path (e.g., "spacing.md")
     * @return Spacing value in pixels
     */
    static double getSpacing(const std::string& path);
    
    /**
     * Get a font size token value
     * @param path Token path (e.g., "typography.body.size")
     * @return Font size value
     */
    static double getFontSize(const std::string& path);
    
    /**
     * Get a border radius token value
     * @param path Token path (e.g., "radius.md")
     * @return Border radius value
     */
    static double getBorderRadius(const std::string& path);
    
    // ========================================================================
    // Token Registration
    // ========================================================================
    
    /**
     * Register a custom token
     * @param path Token path
     * @param value Token value
     */
    static void set(const std::string& path, const TokenValue& value);
    
    /**
     * Register multiple tokens from a JSON file
     * @param jsonPath Path to JSON file containing tokens
     */
    static void loadFromFile(const std::string& jsonPath);
    
    /**
     * Register multiple tokens from a JSON string
     * @param jsonString JSON string containing tokens
     */
    static void loadFromJson(const std::string& jsonString);
    
    // ========================================================================
    // Token Introspection
    // ========================================================================
    
    /**
     * Check if a token exists
     * @param path Token path
     * @return true if token exists
     */
    static bool exists(const std::string& path);
    
    /**
     * Get all token paths
     * @return Vector of all registered token paths
     */
    static std::vector<std::string> getAllPaths();
};

// ============================================================================
// Token Path Constants
// ============================================================================

namespace tokens {

// Color tokens
constexpr const char* COLOR_PRIMARY = "color.primary";
constexpr const char* COLOR_SECONDARY = "color.secondary";
constexpr const char* COLOR_TERTIARY = "color.tertiary";
constexpr const char* COLOR_ERROR = "color.error";
constexpr const char* COLOR_WARNING = "color.warning";
constexpr const char* COLOR_SUCCESS = "color.success";
constexpr const char* COLOR_INFO = "color.info";
constexpr const char* COLOR_BACKGROUND = "color.background";
constexpr const char* COLOR_SURFACE = "color.surface";
constexpr const char* COLOR_ON_PRIMARY = "color.on-primary";
constexpr const char* COLOR_ON_SECONDARY = "color.on-secondary";
constexpr const char* COLOR_ON_BACKGROUND = "color.on-background";
constexpr const char* COLOR_ON_SURFACE = "color.on-surface";

// Spacing tokens
constexpr const char* SPACING_XS = "spacing.xs";
constexpr const char* SPACING_SM = "spacing.sm";
constexpr const char* SPACING_MD = "spacing.md";
constexpr const char* SPACING_LG = "spacing.lg";
constexpr const char* SPACING_XL = "spacing.xl";
constexpr const char* SPACING_XXL = "spacing.xxl";

// Border radius tokens
constexpr const char* RADIUS_NONE = "radius.none";
constexpr const char* RADIUS_SM = "radius.sm";
constexpr const char* RADIUS_MD = "radius.md";
constexpr const char* RADIUS_LG = "radius.lg";
constexpr const char* RADIUS_XL = "radius.xl";
constexpr const char* RADIUS_FULL = "radius.full";

// Typography tokens
constexpr const char* TYPOGRAPHY_BODY = "typography.body";
constexpr const char* TYPOGRAPHY_HEADING = "typography.heading";
constexpr const char* TYPOGRAPHY_CAPTION = "typography.caption";

// Shadow tokens
constexpr const char* SHADOW_SM = "shadow.sm";
constexpr const char* SHADOW_MD = "shadow.md";
constexpr const char* SHADOW_LG = "shadow.lg";
constexpr const char* SHADOW_XL = "shadow.xl";

} // namespace tokens

} // namespace obsidian::style
