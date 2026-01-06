/**
 * NSView+OBSComponentViewProtocol
 *
 * Default implementation of OBSComponentViewProtocol for NSView.
 * Any NSView can adopt the protocol and get sensible defaults.
 */

#import <AppKit/AppKit.h>
#import "OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Default OBSComponentViewProtocol implementation for NSView.
 *
 * Provides basic mount/unmount/layout behavior.
 * Subclasses override methods to customize behavior.
 */
@interface NSView (OBSComponentViewProtocol) <OBSComponentViewProtocol>

@end

NS_ASSUME_NONNULL_END
