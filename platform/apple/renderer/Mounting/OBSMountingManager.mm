/**
 * OBSMountingManager Implementation
 *
 * Applies mutations in the correct order.
 */

#import "OBSMountingManager.h"
#import <mutex>

// Factory method implementations
OBSViewMutation OBSViewMutation::CreateMutation(OBSTag tag, const OBSComponentHandle &handle) {
    OBSViewMutation m{};
    m.type = OBSMutationTypeCreate;
    m.tag = tag;
    m.componentHandle = handle;
    m.parentTag = 0;
    m.index = -1;
    return m;
}

OBSViewMutation OBSViewMutation::DeleteMutation(OBSTag tag, const OBSComponentHandle &handle) {
    OBSViewMutation m{};
    m.type = OBSMutationTypeDelete;
    m.tag = tag;
    m.componentHandle = handle;
    m.parentTag = 0;
    m.index = -1;
    return m;
}

OBSViewMutation OBSViewMutation::InsertMutation(OBSTag parentTag, OBSTag childTag, int32_t index) {
    OBSViewMutation m{};
    m.type = OBSMutationTypeInsert;
    m.tag = childTag;
    m.parentTag = parentTag;
    m.index = index;
    return m;
}

OBSViewMutation OBSViewMutation::RemoveMutation(OBSTag parentTag, OBSTag childTag, int32_t index) {
    OBSViewMutation m{};
    m.type = OBSMutationTypeRemove;
    m.tag = childTag;
    m.parentTag = parentTag;
    m.index = index;
    return m;
}

OBSViewMutation OBSViewMutation::UpdateMutation(OBSTag tag, OBSLayoutMetrics oldMetrics, OBSLayoutMetrics newMetrics) {
    OBSViewMutation m{};
    m.type = OBSMutationTypeUpdate;
    m.tag = tag;
    m.oldLayoutMetrics = oldMetrics;
    m.newLayoutMetrics = newMetrics;
    m.parentTag = 0;
    m.index = -1;
    return m;
}

@implementation OBSMountingManager {
    OBSComponentViewRegistry *_componentViewRegistry;
    
    // Surface ID -> root view mapping
    std::unordered_map<int64_t, __strong NSView *> _rootViews;
    std::mutex _rootViewMutex;
}

+ (instancetype)sharedManager {
    static OBSMountingManager *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[OBSMountingManager alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        _componentViewRegistry = [OBSComponentViewRegistry sharedRegistry];
    }
    return self;
}

- (OBSComponentViewRegistry *)componentViewRegistry {
    return _componentViewRegistry;
}

- (void)performMutations:(const OBSViewMutationList &)mutations {
    NSAssert(
        [NSThread isMainThread],
        @"OBSMountingManager: performMutations must be called on the main thread."
    );
    
    // Sort mutations: Creates -> Deletes -> Removes -> Inserts -> Updates
    // (React Native does this optimization, we keep it simple for now)
    
    for (const auto &mutation : mutations) {
        switch (mutation.type) {
            case OBSMutationTypeCreate:
                [self _performCreateMutation:mutation];
                break;
            case OBSMutationTypeDelete:
                [self _performDeleteMutation:mutation];
                break;
            case OBSMutationTypeInsert:
                [self _performInsertMutation:mutation];
                break;
            case OBSMutationTypeRemove:
                [self _performRemoveMutation:mutation];
                break;
            case OBSMutationTypeUpdate:
                [self _performUpdateMutation:mutation];
                break;
        }
    }
}

- (void)scheduleMutations:(OBSViewMutationList)mutations
               completion:(void (^ _Nullable)(void))completion {
    // Capture mutations by move for efficiency
    __block OBSViewMutationList capturedMutations = std::move(mutations);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [self performMutations:capturedMutations];
        
        if (completion) {
            completion();
        }
    });
}

#pragma mark - Private Mutation Handlers

- (void)_performCreateMutation:(const OBSViewMutation &)mutation {
    // Dequeue (create or recycle) a view from the registry
    const auto &descriptor = [_componentViewRegistry dequeueComponentViewWithHandle:mutation.componentHandle
                                                                                tag:mutation.tag];
    
    if (!descriptor.view) {
        NSLog(@"[OBS] Error: Failed to create view for handle: %s tag: %d",
              mutation.componentHandle.c_str(), mutation.tag);
    }
}

- (void)_performDeleteMutation:(const OBSViewMutation &)mutation {
    // Get the descriptor before removing
    const auto &descriptor = [_componentViewRegistry componentViewDescriptorWithTag:mutation.tag];
    
    if (!descriptor.view) {
        NSLog(@"[OBS] Warning: Delete mutation for unknown tag: %d", mutation.tag);
        return;
    }
    
    // Create a copy since we're going to enqueue (removes from registry)
    OBSComponentViewDescriptor descriptorCopy = descriptor;
    
    // Return to recycle pool (or destroy if pool is full)
    [_componentViewRegistry enqueueComponentViewWithHandle:mutation.componentHandle
                                                       tag:mutation.tag
                                   componentViewDescriptor:descriptorCopy];
}

- (void)_performInsertMutation:(const OBSViewMutation &)mutation {
    NSView<OBSComponentViewProtocol> *childView = [_componentViewRegistry findComponentViewWithTag:mutation.tag];
    
    if (!childView) {
        NSLog(@"[OBS] Error: Insert mutation - child view not found for tag: %d", mutation.tag);
        return;
    }
    
    // Parent could be a root view or another component view
    NSView<OBSComponentViewProtocol> *parentView = nil;
    
    // First check if parentTag is a surface root (tag 0 typically)
    if (mutation.parentTag == 0) {
        // Find the root view - this is a simplification, real impl would need surface ID
        // For now, we'll handle this specially
        NSLog(@"[OBS] Insert to root - tag: %d, index: %d", mutation.tag, mutation.index);
        // Root insertion is handled separately via setRootView
        return;
    }
    
    parentView = [_componentViewRegistry findComponentViewWithTag:mutation.parentTag];
    
    if (!parentView) {
        NSLog(@"[OBS] Error: Insert mutation - parent view not found for tag: %d", mutation.parentTag);
        return;
    }
    
    // Use the protocol method for mounting
    [parentView mountChildComponentView:childView index:mutation.index];
}

- (void)_performRemoveMutation:(const OBSViewMutation &)mutation {
    NSView<OBSComponentViewProtocol> *childView = [_componentViewRegistry findComponentViewWithTag:mutation.tag];
    
    if (!childView) {
        NSLog(@"[OBS] Warning: Remove mutation - child view not found for tag: %d", mutation.tag);
        return;
    }
    
    NSView<OBSComponentViewProtocol> *parentView = nil;
    
    if (mutation.parentTag != 0) {
        parentView = [_componentViewRegistry findComponentViewWithTag:mutation.parentTag];
    }
    
    if (!parentView) {
        // Just remove from superview
        [childView removeFromSuperview];
        return;
    }
    
    [parentView unmountChildComponentView:childView index:mutation.index];
}

- (void)_performUpdateMutation:(const OBSViewMutation &)mutation {
    NSView<OBSComponentViewProtocol> *view = [_componentViewRegistry findComponentViewWithTag:mutation.tag];
    
    if (!view) {
        NSLog(@"[OBS] Warning: Update mutation - view not found for tag: %d", mutation.tag);
        return;
    }
    
    [view updateLayoutMetrics:mutation.newLayoutMetrics oldLayoutMetrics:mutation.oldLayoutMetrics];
}

#pragma mark - Root View Management

- (void)setRootView:(NSView *)rootView forSurfaceId:(int64_t)surfaceId {
    std::lock_guard<std::mutex> lock(_rootViewMutex);
    _rootViews[surfaceId] = rootView;
}

- (nullable NSView *)rootViewForSurfaceId:(int64_t)surfaceId {
    std::lock_guard<std::mutex> lock(_rootViewMutex);
    auto it = _rootViews.find(surfaceId);
    if (it == _rootViews.end()) {
        return nil;
    }
    return it->second;
}

- (void)removeRootViewForSurfaceId:(int64_t)surfaceId {
    std::lock_guard<std::mutex> lock(_rootViewMutex);
    _rootViews.erase(surfaceId);
}

@end
