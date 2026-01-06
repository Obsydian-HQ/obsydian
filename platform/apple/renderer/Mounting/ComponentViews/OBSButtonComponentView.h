/**
 * OBSButtonComponentView
 *
 * Button component view - wraps NSButton functionality.
 * This IS an NSButton subclass - no wrapper.
 *
 * Stores callback/state directly on the view via associated objects.
 */

#import <AppKit/AppKit.h>
#import "../OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Callback type for button actions.
 */
typedef void (*OBSButtonActionCallback)(void *_Nullable userData);

/**
 * Button component view.
 *
 * Features:
 * - NSButton subclass implementing OBSComponentViewProtocol
 * - C callback support for action handling
 * - No wrapper objects - this IS the button
 */
@interface OBSButtonComponentView : NSButton <OBSComponentViewProtocol>

/**
 * Set the button's action callback (C function pointer).
 */
- (void)setActionCallback:(nullable OBSButtonActionCallback)callback userData:(nullable void *)userData;

/**
 * Configure button with title.
 */
- (void)setButtonTitle:(NSString *)title;

@end

NS_ASSUME_NONNULL_END
