/**
 * OBSMountingManager
 *
 * Applies view mutations to the native view tree.
 * Runs on the main thread.
 *
 * Directly inspired by React Native's RCTMountingManager.
 */

#import <AppKit/AppKit.h>
#import "OBSComponentViewRegistry.h"

#include <vector>

NS_ASSUME_NONNULL_BEGIN

/**
 * Mutation types that can be applied to the view tree.
 */
typedef NS_ENUM(NSInteger, OBSMutationType) {
    OBSMutationTypeCreate,
    OBSMutationTypeDelete,
    OBSMutationTypeInsert,
    OBSMutationTypeRemove,
    OBSMutationTypeUpdate
};

/**
 * A single mutation to apply to the view tree.
 */
struct OBSViewMutation {
    OBSMutationType type;
    
    // For Create/Delete: identifies the component type
    OBSComponentHandle componentHandle;
    
    // For all mutations: identifies the view
    OBSTag tag;
    
    // For Insert/Remove: parent and index
    OBSTag parentTag;
    int32_t index;
    
    // For Update: new layout metrics
    OBSLayoutMetrics oldLayoutMetrics;
    OBSLayoutMetrics newLayoutMetrics;
    
    // Static factory methods
    static OBSViewMutation CreateMutation(OBSTag tag, const OBSComponentHandle &handle);
    static OBSViewMutation DeleteMutation(OBSTag tag, const OBSComponentHandle &handle);
    static OBSViewMutation InsertMutation(OBSTag parentTag, OBSTag childTag, int32_t index);
    static OBSViewMutation RemoveMutation(OBSTag parentTag, OBSTag childTag, int32_t index);
    static OBSViewMutation UpdateMutation(OBSTag tag, OBSLayoutMetrics oldMetrics, OBSLayoutMetrics newMetrics);
};

using OBSViewMutationList = std::vector<OBSViewMutation>;

/**
 * Applies mutations to the native view tree.
 *
 * The MountingManager:
 * - Uses ComponentViewRegistry to create/lookup views
 * - Applies mutations in order: Creates, Deletes, Removes, Inserts, Updates
 * - Must be called on the main thread
 */
@interface OBSMountingManager : NSObject

/**
 * Shared instance (singleton).
 */
+ (instancetype)sharedManager;

/**
 * The component view registry.
 */
@property (nonatomic, readonly) OBSComponentViewRegistry *componentViewRegistry;

/**
 * Synchronously apply mutations to the view tree.
 * MUST be called on the main thread.
 *
 * @param mutations List of mutations to apply
 */
- (void)performMutations:(const OBSViewMutationList &)mutations;

/**
 * Schedule mutations to be applied on the main thread.
 * Safe to call from any thread.
 *
 * @param mutations List of mutations to apply
 * @param completion Optional completion handler (called on main thread)
 */
- (void)scheduleMutations:(OBSViewMutationList)mutations
               completion:(void (^ _Nullable)(void))completion;

/**
 * Set the root view for a surface.
 * Views will be mounted within this view.
 */
- (void)setRootView:(NSView *)rootView forSurfaceId:(int64_t)surfaceId;

/**
 * Get the root view for a surface.
 */
- (nullable NSView *)rootViewForSurfaceId:(int64_t)surfaceId;

/**
 * Remove the root view for a surface.
 */
- (void)removeRootViewForSurfaceId:(int64_t)surfaceId;

@end

NS_ASSUME_NONNULL_END
