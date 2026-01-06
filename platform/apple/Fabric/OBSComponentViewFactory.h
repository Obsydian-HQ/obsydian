/**
 * OBSComponentViewFactory
 *
 * Factory registration for all component views.
 * Call registerAllComponents during app initialization.
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// Component handle constants
extern NSString * const OBSComponentVStack;
extern NSString * const OBSComponentHStack;
extern NSString * const OBSComponentZStack;
extern NSString * const OBSComponentButton;
extern NSString * const OBSComponentText;
extern NSString * const OBSComponentSpacer;
extern NSString * const OBSComponentScrollView;
extern NSString * const OBSComponentRoot;

/**
 * Factory for creating and registering component views.
 */
@interface OBSComponentViewFactory : NSObject

/**
 * Register all built-in component factories.
 * Call this during app initialization, before any UI is created.
 */
+ (void)registerAllComponents;

@end

NS_ASSUME_NONNULL_END
