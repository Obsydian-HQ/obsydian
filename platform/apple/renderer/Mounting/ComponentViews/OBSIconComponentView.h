/**
 * OBSIconComponentView
 *
 * Icon component view - wraps NSImageView for SF Symbols.
 * This IS an NSImageView subclass - no wrapper.
 *
 * Supports SF Symbols (macOS 11+), symbol configuration,
 * and symbol effects (macOS 14+).
 */

#import <AppKit/AppKit.h>
#import "../OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Symbol weight values (maps to NSFontWeight).
 */
typedef NS_ENUM(NSInteger, OBSSymbolWeight) {
    OBSSymbolWeightUltraLight = 0,
    OBSSymbolWeightThin,
    OBSSymbolWeightLight,
    OBSSymbolWeightRegular,
    OBSSymbolWeightMedium,
    OBSSymbolWeightSemibold,
    OBSSymbolWeightBold,
    OBSSymbolWeightHeavy,
    OBSSymbolWeightBlack
};

/**
 * Symbol rendering mode.
 */
typedef NS_ENUM(NSInteger, OBSSymbolRenderingMode) {
    OBSSymbolRenderingModeAutomatic = 0,
    OBSSymbolRenderingModeMonochrome,
    OBSSymbolRenderingModeHierarchical,
    OBSSymbolRenderingModePalette,
    OBSSymbolRenderingModeMulticolor
};

/**
 * Symbol effect types (macOS 14+).
 */
typedef NS_ENUM(NSInteger, OBSSymbolEffectType) {
    OBSSymbolEffectTypeBounce = 0,
    OBSSymbolEffectTypePulse,
    OBSSymbolEffectTypeScale,
    OBSSymbolEffectTypeAppear,
    OBSSymbolEffectTypeDisappear,
    OBSSymbolEffectTypeWiggle,
    OBSSymbolEffectTypeRotate,
    OBSSymbolEffectTypeBreathe
};

/**
 * Icon component view.
 *
 * Features:
 * - NSImageView subclass implementing OBSComponentViewProtocol
 * - SF Symbol rendering with configurable weight and size
 * - Tint color support via hierarchical rendering
 * - Symbol effects (macOS 14+)
 */
@interface OBSIconComponentView : NSImageView <OBSComponentViewProtocol>

/**
 * Set the SF Symbol name.
 * @param symbolName System symbol name (e.g., "star.fill", "folder", "heart.fill")
 */
- (void)setSymbolName:(NSString *)symbolName;

/**
 * Set the symbol point size.
 * @param pointSize Size in points (e.g., 16.0, 24.0, 32.0)
 */
- (void)setPointSize:(CGFloat)pointSize;

/**
 * Set the symbol weight.
 * @param weight Symbol weight (ultralight to black)
 */
- (void)setSymbolWeight:(OBSSymbolWeight)weight;

/**
 * Set the tint color.
 * @param color Tint color, or nil for default system color
 */
- (void)setTintColor:(nullable NSColor *)color;

/**
 * Set the rendering mode.
 * @param mode Rendering mode (automatic, monochrome, hierarchical, palette, multicolor)
 */
- (void)setRenderingMode:(OBSSymbolRenderingMode)mode;

/**
 * Add a symbol effect (macOS 14+).
 * @param effectType Type of effect (bounce, pulse, scale, etc.)
 * @param repeatCount Number of times to repeat (-1 for infinite)
 * @param byLayer Whether to animate by layer
 * @param speed Animation speed multiplier (1.0 = normal)
 */
- (void)addSymbolEffectWithType:(OBSSymbolEffectType)effectType
                    repeatCount:(NSInteger)repeatCount
                        byLayer:(BOOL)byLayer
                          speed:(CGFloat)speed;

/**
 * Remove all symbol effects.
 */
- (void)removeAllSymbolEffects;

@end

NS_ASSUME_NONNULL_END
