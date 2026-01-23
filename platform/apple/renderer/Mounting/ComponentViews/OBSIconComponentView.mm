/**
 * OBSIconComponentView Implementation
 *
 * NSImageView subclass for SF Symbols rendering.
 * Supports symbol configuration and effects (macOS 14+).
 */

#import "OBSIconComponentView.h"
#import <objc/runtime.h>

// Associated object keys for storing state
static const void *kSymbolNameKey = &kSymbolNameKey;
static const void *kPointSizeKey = &kPointSizeKey;
static const void *kSymbolWeightKey = &kSymbolWeightKey;
static const void *kTintColorKey = &kTintColorKey;
static const void *kRenderingModeKey = &kRenderingModeKey;

@implementation OBSIconComponentView

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        // Configure image view defaults
        self.imageScaling = NSImageScaleProportionallyUpOrDown;
        self.imageAlignment = NSImageAlignCenter;
        self.editable = NO;

        // Set default values via associated objects
        objc_setAssociatedObject(self, kPointSizeKey, @(17.0), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        objc_setAssociatedObject(self, kSymbolWeightKey, @(OBSSymbolWeightRegular), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        objc_setAssociatedObject(self, kRenderingModeKey, @(OBSSymbolRenderingModeAutomatic), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return self;
}

#pragma mark - Private Helpers

/**
 * Convert OBSSymbolWeight to NSFontWeight.
 */
- (NSFontWeight)_nsFontWeightFromSymbolWeight:(OBSSymbolWeight)weight {
    switch (weight) {
        case OBSSymbolWeightUltraLight: return NSFontWeightUltraLight;
        case OBSSymbolWeightThin: return NSFontWeightThin;
        case OBSSymbolWeightLight: return NSFontWeightLight;
        case OBSSymbolWeightRegular: return NSFontWeightRegular;
        case OBSSymbolWeightMedium: return NSFontWeightMedium;
        case OBSSymbolWeightSemibold: return NSFontWeightSemibold;
        case OBSSymbolWeightBold: return NSFontWeightBold;
        case OBSSymbolWeightHeavy: return NSFontWeightHeavy;
        case OBSSymbolWeightBlack: return NSFontWeightBlack;
        default: return NSFontWeightRegular;
    }
}

/**
 * Update the image with current configuration.
 */
- (void)_updateImage {
    NSString *symbolName = objc_getAssociatedObject(self, kSymbolNameKey);
    if (!symbolName || symbolName.length == 0) {
        self.image = nil;
        return;
    }

    // Get stored values
    NSNumber *pointSizeNum = objc_getAssociatedObject(self, kPointSizeKey);
    NSNumber *weightNum = objc_getAssociatedObject(self, kSymbolWeightKey);
    NSNumber *renderingModeNum = objc_getAssociatedObject(self, kRenderingModeKey);
    NSColor *tintColor = objc_getAssociatedObject(self, kTintColorKey);

    CGFloat pointSize = pointSizeNum ? pointSizeNum.doubleValue : 17.0;
    OBSSymbolWeight weight = weightNum ? (OBSSymbolWeight)weightNum.integerValue : OBSSymbolWeightRegular;
    OBSSymbolRenderingMode renderingMode = renderingModeNum ? (OBSSymbolRenderingMode)renderingModeNum.integerValue : OBSSymbolRenderingModeAutomatic;

    // Create base image from SF Symbol
    NSImage *image = [NSImage imageWithSystemSymbolName:symbolName
                               accessibilityDescription:nil];

    if (!image) {
        NSLog(@"[OBSIconComponentView] Failed to load SF Symbol: %@", symbolName);
        self.image = nil;
        return;
    }

    // Build symbol configuration
    NSFontWeight nsFontWeight = [self _nsFontWeightFromSymbolWeight:weight];
    NSImageSymbolConfiguration *config = [NSImageSymbolConfiguration
        configurationWithPointSize:pointSize weight:nsFontWeight];

    // Apply rendering mode (macOS 12+ for configurationByApplyingConfiguration)
    if (@available(macOS 12.0, *)) {
        switch (renderingMode) {
            case OBSSymbolRenderingModeMonochrome:
                // configurationPreferringMonochrome requires macOS 13+
                if (@available(macOS 13.0, *)) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationPreferringMonochrome]];
                }
                break;
            case OBSSymbolRenderingModeHierarchical:
                if (tintColor) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationWithHierarchicalColor:tintColor]];
                } else if (@available(macOS 13.0, *)) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationPreferringHierarchical]];
                }
                break;
            case OBSSymbolRenderingModeMulticolor:
                if (@available(macOS 13.0, *)) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationPreferringMulticolor]];
                }
                break;
            case OBSSymbolRenderingModePalette:
                // Palette mode requires multiple colors; use tint color if available
                if (tintColor) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationWithPaletteColors:@[tintColor]]];
                }
                break;
            case OBSSymbolRenderingModeAutomatic:
            default:
                // For automatic mode, apply tint color via hierarchical if specified
                if (tintColor) {
                    config = [config configurationByApplyingConfiguration:
                        [NSImageSymbolConfiguration configurationWithHierarchicalColor:tintColor]];
                }
                break;
        }
    }

    // Apply configuration to image
    NSImage *configuredImage = [image imageWithSymbolConfiguration:config];
    self.image = configuredImage ?: image;

    // Set content tint color for additional tinting support
    if (tintColor) {
        self.contentTintColor = tintColor;
    } else {
        self.contentTintColor = nil;
    }
}

#pragma mark - Public API

- (void)setSymbolName:(NSString *)symbolName {
    objc_setAssociatedObject(self, kSymbolNameKey, symbolName, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [self _updateImage];
}

- (void)setPointSize:(CGFloat)pointSize {
    objc_setAssociatedObject(self, kPointSizeKey, @(pointSize), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self _updateImage];
}

- (void)setSymbolWeight:(OBSSymbolWeight)weight {
    objc_setAssociatedObject(self, kSymbolWeightKey, @(weight), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self _updateImage];
}

- (void)setTintColor:(nullable NSColor *)color {
    objc_setAssociatedObject(self, kTintColorKey, color, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self _updateImage];
}

- (void)setRenderingMode:(OBSSymbolRenderingMode)mode {
    objc_setAssociatedObject(self, kRenderingModeKey, @(mode), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [self _updateImage];
}

- (void)addSymbolEffectWithType:(OBSSymbolEffectType)effectType
                    repeatCount:(NSInteger)repeatCount
                        byLayer:(BOOL)byLayer
                          speed:(CGFloat)speed {
    // Symbol effects require macOS 14.0+
    if (@available(macOS 14.0, *)) {
        // Build options
        NSSymbolEffectOptions *options = [NSSymbolEffectOptions options];

        // Set repeat count (-1 means infinite/repeating)
        if (repeatCount < 0) {
            options = [options optionsWithRepeating];
        } else if (repeatCount > 0) {
            options = [options optionsWithRepeatCount:repeatCount];
        } else {
            options = [options optionsWithNonRepeating];
        }

        // Set speed
        if (speed != 1.0) {
            options = [options optionsWithSpeed:speed];
        }

        // Create and add the appropriate effect
        switch (effectType) {
            case OBSSymbolEffectTypeBounce: {
                NSSymbolBounceEffect *baseEffect = [NSSymbolBounceEffect bounceUpEffect];
                NSSymbolBounceEffect *effect = byLayer ?
                    [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                [self addSymbolEffect:effect options:options animated:YES];
                break;
            }
            case OBSSymbolEffectTypePulse: {
                NSSymbolPulseEffect *baseEffect = [NSSymbolPulseEffect effect];
                NSSymbolPulseEffect *effect = byLayer ?
                    [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                [self addSymbolEffect:effect options:options animated:YES];
                break;
            }
            case OBSSymbolEffectTypeScale: {
                NSSymbolScaleEffect *baseEffect = [NSSymbolScaleEffect scaleUpEffect];
                NSSymbolScaleEffect *effect = byLayer ?
                    [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                [self addSymbolEffect:effect options:options animated:YES];
                break;
            }
            case OBSSymbolEffectTypeAppear: {
                NSSymbolAppearEffect *baseEffect = [NSSymbolAppearEffect appearUpEffect];
                NSSymbolAppearEffect *effect = byLayer ?
                    [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                [self addSymbolEffect:effect options:options animated:YES];
                break;
            }
            case OBSSymbolEffectTypeDisappear: {
                NSSymbolDisappearEffect *baseEffect = [NSSymbolDisappearEffect disappearDownEffect];
                NSSymbolDisappearEffect *effect = byLayer ?
                    [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                [self addSymbolEffect:effect options:options animated:YES];
                break;
            }
            case OBSSymbolEffectTypeWiggle: {
                // Wiggle requires macOS 15+
                if (@available(macOS 15.0, *)) {
                    NSSymbolWiggleEffect *baseEffect = [NSSymbolWiggleEffect effect];
                    NSSymbolWiggleEffect *effect = byLayer ?
                        [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                    [self addSymbolEffect:effect options:options animated:YES];
                } else {
                    NSLog(@"[OBSIconComponentView] Wiggle effect requires macOS 15.0+");
                }
                break;
            }
            case OBSSymbolEffectTypeRotate: {
                // Rotate requires macOS 15+
                if (@available(macOS 15.0, *)) {
                    NSSymbolRotateEffect *baseEffect = [NSSymbolRotateEffect effect];
                    NSSymbolRotateEffect *effect = byLayer ?
                        [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                    [self addSymbolEffect:effect options:options animated:YES];
                } else {
                    NSLog(@"[OBSIconComponentView] Rotate effect requires macOS 15.0+");
                }
                break;
            }
            case OBSSymbolEffectTypeBreathe: {
                // Breathe requires macOS 15+
                if (@available(macOS 15.0, *)) {
                    NSSymbolBreatheEffect *baseEffect = [NSSymbolBreatheEffect breathePulseEffect];
                    NSSymbolBreatheEffect *effect = byLayer ?
                        [baseEffect effectWithByLayer] : [baseEffect effectWithWholeSymbol];
                    [self addSymbolEffect:effect options:options animated:YES];
                } else {
                    NSLog(@"[OBSIconComponentView] Breathe effect requires macOS 15.0+");
                }
                break;
            }
        }
    } else {
        NSLog(@"[OBSIconComponentView] Symbol effects require macOS 14.0+");
    }
}

- (void)removeAllSymbolEffects {
    if (@available(macOS 14.0, *)) {
        [self removeAllSymbolEffectsWithOptions:[NSSymbolEffectOptions options] animated:YES];
    }
}

#pragma mark - OBSComponentViewProtocol

- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index {
    // Icons don't have children
    NSLog(@"[OBS] Warning: Attempt to mount child into icon - not supported");
}

- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index {
    // Icons don't have children
    NSLog(@"[OBS] Warning: Attempt to unmount child from icon - not supported");
}

- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics {
    if (!isfinite(layoutMetrics.x) || !isfinite(layoutMetrics.y) ||
        !isfinite(layoutMetrics.width) || !isfinite(layoutMetrics.height)) {
        NSLog(@"[OBS] Warning: Invalid layout metrics for icon");
        return;
    }

    self.frame = NSMakeRect(
        layoutMetrics.x,
        layoutMetrics.y,
        layoutMetrics.width,
        layoutMetrics.height
    );
}

- (void)prepareForRecycle {
    // Remove all effects
    if (@available(macOS 14.0, *)) {
        [self removeAllSymbolEffectsWithOptions:[NSSymbolEffectOptions options] animated:NO];
    }

    // Clear image
    self.image = nil;

    // Clear stored values
    objc_setAssociatedObject(self, kSymbolNameKey, nil, OBJC_ASSOCIATION_COPY_NONATOMIC);
    objc_setAssociatedObject(self, kTintColorKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);

    // Reset to defaults
    objc_setAssociatedObject(self, kPointSizeKey, @(17.0), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(self, kSymbolWeightKey, @(OBSSymbolWeightRegular), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(self, kRenderingModeKey, @(OBSSymbolRenderingModeAutomatic), OBJC_ASSOCIATION_RETAIN_NONATOMIC);

    // Reset content tint
    self.contentTintColor = nil;

    // Reset frame
    self.frame = NSZeroRect;

    // Remove from superview
    if (self.superview) {
        [self removeFromSuperview];
    }
}

- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask {
    // No-op for icon
}

- (void)invalidate {
    // Remove effects on invalidation
    if (@available(macOS 14.0, *)) {
        [self removeAllSymbolEffectsWithOptions:[NSSymbolEffectOptions options] animated:NO];
    }
}

@end
