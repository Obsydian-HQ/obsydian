/**
 * OBSComponentViewRegistry Implementation
 *
 * Uses std::unordered_map with OBSComponentViewDescriptor.
 * The __strong NSView* in the descriptor ensures ARC retains views.
 */

#import "OBSComponentViewRegistry.h"
#import <mutex>

static const NSInteger kRecyclePoolMaxSize = 1024;

@implementation OBSComponentViewRegistry {
    // Tag -> Descriptor mapping (views are strongly retained in descriptor)
    std::unordered_map<OBSTag, OBSComponentViewDescriptor> _registry;
    
    // ComponentHandle -> recycled views pool
    std::unordered_map<OBSComponentHandle, std::vector<OBSComponentViewDescriptor>> _recyclePool;
    
    // Factory functions for each component type
    std::unordered_map<OBSComponentHandle, OBSViewFactory> _factories;
    
    // Next available Tag
    OBSTag _nextTag;
    
    // Thread safety
    std::mutex _mutex;
    
    // Empty descriptor for failed lookups
    OBSComponentViewDescriptor _emptyDescriptor;
}

+ (NSInteger)recyclePoolMaxSize {
    return kRecyclePoolMaxSize;
}

+ (instancetype)sharedRegistry {
    static OBSComponentViewRegistry *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[OBSComponentViewRegistry alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        _nextTag = 1;  // 0 is reserved for "no tag"
    }
    return self;
}

- (void)registerViewFactory:(OBSViewFactory)factory
         forComponentHandle:(const OBSComponentHandle &)handle {
    std::lock_guard<std::mutex> lock(_mutex);
    _factories[handle] = factory;
}

- (const OBSComponentViewDescriptor &)dequeueComponentViewWithHandle:(const OBSComponentHandle &)handle
                                                                 tag:(OBSTag)tag {
    std::lock_guard<std::mutex> lock(_mutex);
    
    NSCAssert(
        _registry.find(tag) == _registry.end(),
        @"OBSComponentViewRegistry: Attempt to dequeue already registered component with tag %d.",
        tag
    );
    
    // Try to get from recycle pool first
    auto &recycledViews = _recyclePool[handle];
    OBSComponentViewDescriptor descriptor;
    
    if (!recycledViews.empty()) {
        descriptor = recycledViews.back();
        recycledViews.pop_back();
    } else {
        // Create new view via factory
        auto factoryIt = _factories.find(handle);
        if (factoryIt != _factories.end()) {
            NSView<OBSComponentViewProtocol> *view = factoryIt->second();
            descriptor = OBSComponentViewDescriptor(view, true);
        } else {
            NSLog(@"[OBS] Error: No factory registered for component handle: %s", handle.c_str());
            return _emptyDescriptor;
        }
    }
    
    // Store in registry - ARC retains via __strong in descriptor
    auto result = _registry.insert({tag, descriptor});
    return result.first->second;
}

- (void)enqueueComponentViewWithHandle:(const OBSComponentHandle &)handle
                                   tag:(OBSTag)tag
               componentViewDescriptor:(OBSComponentViewDescriptor)descriptor {
    std::lock_guard<std::mutex> lock(_mutex);
    
    NSCAssert(
        _registry.find(tag) != _registry.end(),
        @"OBSComponentViewRegistry: Attempt to enqueue unregistered component with tag %d.",
        tag
    );
    
    // Remove from active registry
    _registry.erase(tag);
    
    // Check if we should recycle
    auto &recycledViews = _recyclePool[handle];
    
    if (recycledViews.size() >= kRecyclePoolMaxSize || !descriptor.shouldBeRecycled) {
        // Pool is full or view shouldn't be recycled - invalidate and release
        if (descriptor.view && [descriptor.view respondsToSelector:@selector(invalidate)]) {
            [descriptor.view invalidate];
        }
        // descriptor goes out of scope, ARC releases the view
        return;
    }
    
    // Prepare for recycling and store in pool
    NSCAssert(
        descriptor.view.superview == nil,
        @"OBSComponentViewRegistry: Attempt to recycle a mounted view."
    );
    
    if (descriptor.view) {
        [descriptor.view prepareForRecycle];
    }
    
    recycledViews.push_back(descriptor);
}

- (const OBSComponentViewDescriptor &)componentViewDescriptorWithTag:(OBSTag)tag {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _registry.find(tag);
    if (it == _registry.end()) {
        return _emptyDescriptor;
    }
    return it->second;
}

- (nullable NSView<OBSComponentViewProtocol> *)findComponentViewWithTag:(OBSTag)tag {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _registry.find(tag);
    if (it == _registry.end()) {
        return nil;
    }
    return it->second.view;
}

- (BOOL)hasTag:(OBSTag)tag {
    std::lock_guard<std::mutex> lock(_mutex);
    return _registry.find(tag) != _registry.end();
}

- (OBSTag)generateTag {
    std::lock_guard<std::mutex> lock(_mutex);
    return _nextTag++;
}

- (void)clearRecyclePool {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Invalidate all recycled views
    for (auto &pair : _recyclePool) {
        for (auto &descriptor : pair.second) {
            if (descriptor.view && [descriptor.view respondsToSelector:@selector(invalidate)]) {
                [descriptor.view invalidate];
            }
        }
    }
    
    _recyclePool.clear();
}

- (NSUInteger)activeViewCount {
    std::lock_guard<std::mutex> lock(_mutex);
    return _registry.size();
}

- (NSUInteger)recycledViewCountForHandle:(const OBSComponentHandle &)handle {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _recyclePool.find(handle);
    if (it == _recyclePool.end()) {
        return 0;
    }
    return it->second.size();
}

@end
