/**
 * OBSFabricBridge Implementation
 *
 * C interface implementation that bridges to Objective-C++ platform layer.
 * All view operations go through the MUTATION system.
 */

#import "OBSFabricBridge.h"
#import "OBSComponentViewFactory.h"
#import "Mounting/OBSComponentViewRegistry.h"
#import "Mounting/OBSMountingManager.h"
#import "Mounting/ComponentViews/OBSButtonComponentView.h"
#import "Mounting/ComponentViews/OBSTextComponentView.h"

// Component type constants
const char* OBS_COMPONENT_VSTACK = "VStack";
const char* OBS_COMPONENT_HSTACK = "HStack";
const char* OBS_COMPONENT_ZSTACK = "ZStack";
const char* OBS_COMPONENT_BUTTON = "Button";
const char* OBS_COMPONENT_TEXT = "Text";
const char* OBS_COMPONENT_SPACER = "Spacer";
const char* OBS_COMPONENT_ROOT = "Root";

void obs_fabric_initialize(void) {
    // Register all component factories
    [OBSComponentViewFactory registerAllComponents];
    NSLog(@"[Fabric] Initialized");
}

void obs_fabric_shutdown(void) {
    // Clear recycle pools
    [[OBSComponentViewRegistry sharedRegistry] clearRecyclePool];
    NSLog(@"[Fabric] Shutdown");
}

void obs_fabric_apply_mutations(const OBSCViewMutation* mutations, int32_t count) {
    NSCAssert([NSThread isMainThread], @"obs_fabric_apply_mutations must be called on main thread");
    
    if (!mutations || count <= 0) {
        return;
    }
    
    // Convert C mutations to ObjC++ mutations
    OBSViewMutationList mutationList;
    mutationList.reserve(count);
    
    for (int32_t i = 0; i < count; i++) {
        const OBSCViewMutation& cMut = mutations[i];
        
        OBSViewMutation mutation{};
        mutation.type = static_cast<OBSMutationType>(cMut.type);
        mutation.tag = cMut.tag;
        mutation.parentTag = cMut.parentTag;
        mutation.index = cMut.index;
        
        if (cMut.componentHandle) {
            mutation.componentHandle = std::string(cMut.componentHandle);
        }
        
        mutation.oldLayoutMetrics = {
            static_cast<CGFloat>(cMut.oldLayoutMetrics.x),
            static_cast<CGFloat>(cMut.oldLayoutMetrics.y),
            static_cast<CGFloat>(cMut.oldLayoutMetrics.width),
            static_cast<CGFloat>(cMut.oldLayoutMetrics.height)
        };
        
        mutation.newLayoutMetrics = {
            static_cast<CGFloat>(cMut.newLayoutMetrics.x),
            static_cast<CGFloat>(cMut.newLayoutMetrics.y),
            static_cast<CGFloat>(cMut.newLayoutMetrics.width),
            static_cast<CGFloat>(cMut.newLayoutMetrics.height)
        };
        
        mutationList.push_back(mutation);
    }
    
    // Apply mutations through MountingManager
    [[OBSMountingManager sharedManager] performMutations:mutationList];
}

void obs_fabric_schedule_mutations(const OBSCViewMutation* mutations, int32_t count) {
    if (!mutations || count <= 0) {
        return;
    }
    
    // Copy mutations for async dispatch
    __block std::vector<OBSCViewMutation> mutationsCopy(mutations, mutations + count);
    
    // Also copy the component handle strings since they're pointers
    __block std::vector<std::string> handles;
    handles.reserve(count);
    for (int32_t i = 0; i < count; i++) {
        if (mutations[i].componentHandle) {
            handles.push_back(std::string(mutations[i].componentHandle));
        } else {
            handles.push_back("");
        }
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        // Update pointers to our captured strings
        for (size_t i = 0; i < mutationsCopy.size(); i++) {
            mutationsCopy[i].componentHandle = handles[i].c_str();
        }
        obs_fabric_apply_mutations(mutationsCopy.data(), (int32_t)mutationsCopy.size());
    });
}

void obs_fabric_set_root_view(int64_t surfaceId, void* rootView) {
    NSView *view = (__bridge NSView *)rootView;
    [[OBSMountingManager sharedManager] setRootView:view forSurfaceId:surfaceId];
}

int32_t obs_fabric_generate_tag(void) {
    return [[OBSComponentViewRegistry sharedRegistry] generateTag];
}

void* obs_fabric_find_view(int32_t tag) {
    NSView *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    return (__bridge void *)view;
}

void obs_fabric_button_set_title(int32_t tag, const char* title) {
    NSView<OBSComponentViewProtocol> *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    
    if ([view isKindOfClass:[OBSButtonComponentView class]]) {
        OBSButtonComponentView *button = (OBSButtonComponentView *)view;
        NSString *nsTitle = title ? [NSString stringWithUTF8String:title] : @"";
        [button setButtonTitle:nsTitle];
    }
}

void obs_fabric_button_set_callback(int32_t tag, OBSButtonCallback callback, void* userData) {
    NSView<OBSComponentViewProtocol> *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    
    if ([view isKindOfClass:[OBSButtonComponentView class]]) {
        OBSButtonComponentView *button = (OBSButtonComponentView *)view;
        [button setActionCallback:callback userData:userData];
    }
}

void obs_fabric_text_set_content(int32_t tag, const char* text) {
    NSView<OBSComponentViewProtocol> *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    
    if ([view isKindOfClass:[OBSTextComponentView class]]) {
        OBSTextComponentView *textView = (OBSTextComponentView *)view;
        NSString *nsText = text ? [NSString stringWithUTF8String:text] : @"";
        [textView setTextContent:nsText];
    }
}

void obs_fabric_add_view_to_window(int32_t tag, void* windowHandle) {
    if (!windowHandle) return;
    
    NSView<OBSComponentViewProtocol> *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    if (!view) return;
    
    NSWindow *window = (__bridge NSWindow *)windowHandle;
    NSView *contentView = window.contentView;
    
    if (contentView) {
        [contentView addSubview:view];
    }
}

void obs_fabric_remove_view_from_parent(int32_t tag) {
    NSView<OBSComponentViewProtocol> *view = [[OBSComponentViewRegistry sharedRegistry] findComponentViewWithTag:tag];
    if (view && view.superview) {
        [view removeFromSuperview];
    }
}
