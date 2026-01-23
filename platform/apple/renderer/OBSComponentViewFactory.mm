/**
 * OBSComponentViewFactory Implementation
 *
 * Registers all built-in component factories with ComponentViewRegistry.
 */

#import "OBSComponentViewFactory.h"
#import "Mounting/OBSComponentViewRegistry.h"
#import "Mounting/ComponentViews/OBSViewComponentView.h"
#import "Mounting/ComponentViews/OBSButtonComponentView.h"
#import "Mounting/ComponentViews/OBSTextComponentView.h"
#import "Mounting/ComponentViews/OBSIconComponentView.h"

// Component handle constants
NSString * const OBSComponentVStack = @"VStack";
NSString * const OBSComponentHStack = @"HStack";
NSString * const OBSComponentZStack = @"ZStack";
NSString * const OBSComponentButton = @"Button";
NSString * const OBSComponentText = @"Text";
NSString * const OBSComponentSpacer = @"Spacer";
NSString * const OBSComponentScrollView = @"ScrollView";
NSString * const OBSComponentRoot = @"Root";
NSString * const OBSComponentIcon = @"Icon";

@implementation OBSComponentViewFactory

+ (void)registerAllComponents {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        OBSComponentViewRegistry *registry = [OBSComponentViewRegistry sharedRegistry];
        
        // VStack - container view
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentVStack.UTF8String];
        
        // HStack - same container view type
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentHStack.UTF8String];
        
        // ZStack - same container view type  
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentZStack.UTF8String];
        
        // Root container
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentRoot.UTF8String];
        
        // Button
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSButtonComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentButton.UTF8String];
        
        // Text
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSTextComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentText.UTF8String];
        
        // Spacer - invisible container
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            OBSViewComponentView *view = [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
            view.hidden = NO;  // Spacer is visible but transparent
            return view;
        } forComponentHandle:OBSComponentSpacer.UTF8String];
        
        // ScrollView (placeholder - use container for now)
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            // TODO: Create proper OBSScrollViewComponentView
            return [[OBSViewComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentScrollView.UTF8String];

        // Icon - SF Symbols support
        [registry registerViewFactory:^NSView<OBSComponentViewProtocol> *{
            return [[OBSIconComponentView alloc] initWithFrame:NSZeroRect];
        } forComponentHandle:OBSComponentIcon.UTF8String];

        NSLog(@"[OBS] Registered all component factories");
    });
}

@end
