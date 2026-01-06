/**
 * OBSTextComponentView
 *
 * Text display component - wraps NSTextField (used as label).
 * This IS an NSTextField subclass - no wrapper.
 */

#import <AppKit/AppKit.h>
#import "../OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Text display component view.
 *
 * Features:
 * - NSTextField configured as non-editable label
 * - Implements OBSComponentViewProtocol
 */
@interface OBSTextComponentView : NSTextField <OBSComponentViewProtocol>

/**
 * Set the displayed text.
 */
- (void)setTextContent:(NSString *)text;

/**
 * Set text alignment.
 */
- (void)setTextAlignment:(NSTextAlignment)alignment;

/**
 * Set text color.
 */
- (void)setTextDisplayColor:(nullable NSColor *)color;

/**
 * Set font.
 */
- (void)setTextFont:(NSFont *)font;

@end

NS_ASSUME_NONNULL_END
