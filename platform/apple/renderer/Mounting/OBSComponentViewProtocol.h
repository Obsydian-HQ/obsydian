/**
 * OBSComponentViewProtocol
 *
 * All Obsydian component views must implement this protocol.
 * Directly inspired by React Native's RCTComponentViewProtocol.
 *
 * Key insight: The view IS the component. No wrappers.
 */

#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * Layout metrics computed by the layout engine.
 * Applied to views during mounting.
 */
typedef struct {
    CGFloat x;
    CGFloat y;
    CGFloat width;
    CGFloat height;
} OBSLayoutMetrics;

/**
 * Bitmask for update types.
 */
typedef NS_OPTIONS(NSInteger, OBSComponentViewUpdateMask) {
    OBSComponentViewUpdateMaskNone = 0,
    OBSComponentViewUpdateMaskProps = 1 << 0,
    OBSComponentViewUpdateMaskState = 1 << 1,
    OBSComponentViewUpdateMaskLayoutMetrics = 1 << 2,
    OBSComponentViewUpdateMaskAll = OBSComponentViewUpdateMaskProps |
                                    OBSComponentViewUpdateMaskState |
                                    OBSComponentViewUpdateMaskLayoutMetrics
};

/**
 * Protocol that ALL Obsydian component views implement.
 *
 * Views implementing this protocol ARE the components.
 * There are no wrapper objects - the NSView IS the component.
 */
@protocol OBSComponentViewProtocol <NSObject>

@required

/**
 * Mount a child component view at the specified index.
 * Called by the MountingManager during Insert mutations.
 */
- (void)mountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                          index:(NSInteger)index;

/**
 * Unmount a child component view at the specified index.
 * Called by the MountingManager during Remove mutations.
 */
- (void)unmountChildComponentView:(NSView<OBSComponentViewProtocol> *)childComponentView
                            index:(NSInteger)index;

/**
 * Update the view's layout metrics (position and size).
 * Called by the MountingManager during Update mutations.
 */
- (void)updateLayoutMetrics:(OBSLayoutMetrics)layoutMetrics
           oldLayoutMetrics:(OBSLayoutMetrics)oldLayoutMetrics;

/**
 * Prepare the view for recycling.
 * Called when the view is returned to the recycle pool.
 * Should reset any local state and prepare for reuse.
 */
- (void)prepareForRecycle;

@optional

/**
 * Called right after all update methods for a particular update batch.
 * Useful for batched updates.
 */
- (void)finalizeUpdates:(OBSComponentViewUpdateMask)updateMask;

/**
 * Called when the view is about to be deallocated (not recycled).
 * Use for cleanup that shouldn't happen during recycling.
 */
- (void)invalidate;

@end

NS_ASSUME_NONNULL_END
