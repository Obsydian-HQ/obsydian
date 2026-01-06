/**
 * OBSViewComponentView
 *
 * Base component view for container types (VStack, HStack, ZStack).
 * This IS the NSView - no wrapper.
 *
 * Uses top-left coordinate system (isFlipped = YES).
 */

#import <AppKit/AppKit.h>
#import "../OBSComponentViewProtocol.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * Base container view for Obsydian layout containers.
 *
 * Features:
 * - Uses flipped (top-left origin) coordinate system
 * - No automatic layout - frames are set by layout engine
 * - Implements OBSComponentViewProtocol
 */
@interface OBSViewComponentView : NSView <OBSComponentViewProtocol>

@end

NS_ASSUME_NONNULL_END
