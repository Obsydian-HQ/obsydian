/**
 * OBSComponentViewRegistry
 *
 * Registry of native component views.
 * Provides allocation, recycling, and querying by Tag.
 *
 * KEY INSIGHT: This registry OWNS views via strong references.
 * When views are stored here, ARC keeps them alive.
 *
 * Directly inspired by React Native's RCTComponentViewRegistry.
 */

#import <AppKit/AppKit.h>
#import "OBSComponentViewDescriptor.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

NS_ASSUME_NONNULL_BEGIN

/**
 * Tag is an integer identifier for native views.
 */
using OBSTag = int32_t;

/**
 * ComponentHandle identifies the type of component (Button, VStack, etc.).
 */
using OBSComponentHandle = std::string;

/**
 * Function type for creating component views.
 * Returns a newly created NSView implementing OBSComponentViewProtocol.
 */
using OBSViewFactory = std::function<NSView<OBSComponentViewProtocol> *()>;

/**
 * Registry of native component views.
 *
 * Manages view lifecycle:
 * - Creates views via registered factories
 * - Stores views by Tag (with strong references)
 * - Recycles views when unmounted
 * - Queries views by Tag
 */
@interface OBSComponentViewRegistry : NSObject

/**
 * Maximum size of recycle pool per component type.
 */
@property (class, readonly) NSInteger recyclePoolMaxSize;

/**
 * Shared instance (singleton).
 */
+ (instancetype)sharedRegistry;

/**
 * Register a view factory for a component type.
 * Call this during app initialization for each component type.
 */
- (void)registerViewFactory:(OBSViewFactory)factory
         forComponentHandle:(const OBSComponentHandle &)handle;

/**
 * Dequeue a view from the recycle pool (or create a new one).
 * Associates it with the given Tag.
 *
 * @param handle The component type
 * @param tag The unique tag to associate with the view
 * @return Reference to the component view descriptor
 */
- (const OBSComponentViewDescriptor &)dequeueComponentViewWithHandle:(const OBSComponentHandle &)handle
                                                                 tag:(OBSTag)tag;

/**
 * Return a view to the recycle pool.
 * The view is prepared for recycling but NOT destroyed.
 *
 * @param handle The component type
 * @param tag The tag of the view to return
 * @param descriptor The descriptor to enqueue
 */
- (void)enqueueComponentViewWithHandle:(const OBSComponentHandle &)handle
                                   tag:(OBSTag)tag
               componentViewDescriptor:(OBSComponentViewDescriptor)descriptor;

/**
 * Get the descriptor for a view by Tag.
 *
 * @param tag The tag to look up
 * @return Reference to the descriptor (empty if not found)
 */
- (const OBSComponentViewDescriptor &)componentViewDescriptorWithTag:(OBSTag)tag;

/**
 * Find a native view by Tag.
 *
 * @param tag The tag to look up
 * @return The view, or nil if not found
 */
- (nullable NSView<OBSComponentViewProtocol> *)findComponentViewWithTag:(OBSTag)tag;

/**
 * Check if a Tag is registered.
 */
- (BOOL)hasTag:(OBSTag)tag;

/**
 * Generate a new unique Tag.
 */
- (OBSTag)generateTag;

/**
 * Clear all recycled views (e.g., on memory warning).
 */
- (void)clearRecyclePool;

/**
 * Get count of active views.
 */
- (NSUInteger)activeViewCount;

/**
 * Get count of recycled views for a component type.
 */
- (NSUInteger)recycledViewCountForHandle:(const OBSComponentHandle &)handle;

@end

NS_ASSUME_NONNULL_END
