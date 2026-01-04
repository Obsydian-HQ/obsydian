/**
 * macOS Layout FFI - Objective-C++ Implementation
 * 
 * Bridges C++ calls to Objective-C NSLayoutConstraint APIs
 */

#import "macos_layout.h"
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

// Internal constraint wrapper class
@interface ObsidianLayoutConstraintWrapper : NSObject {
    NSLayoutConstraint* _constraint;
}

@property (nonatomic, strong) NSLayoutConstraint* constraint;

- (instancetype)initWithParams:(ObsidianLayoutConstraintParams)params;
- (bool)isValid;

@end

@implementation ObsidianLayoutConstraintWrapper

- (instancetype)initWithParams:(ObsidianLayoutConstraintParams)params {
    self = [super init];
    if (self) {
        if (!params.firstView) {
            return nil;
        }
        
        NSView* firstView = (__bridge NSView*)params.firstView;
        if (!firstView) {
            return nil;
        }
        
        NSView* secondView = nil;
        if (params.secondView) {
            secondView = (__bridge NSView*)params.secondView;
            if (!secondView) {
                return nil;
            }
        }
        
        // Convert ObsidianLayoutAttribute to NSLayoutAttribute
        NSLayoutAttribute firstAttr = (NSLayoutAttribute)params.firstAttribute;
        NSLayoutAttribute secondAttr = params.secondView ? (NSLayoutAttribute)params.secondAttribute : NSLayoutAttributeNotAnAttribute;
        
        // Convert ObsidianLayoutRelation to NSLayoutRelation
        NSLayoutRelation relation = (NSLayoutRelation)params.relation;
        
        // Create the constraint with exception handling
        @try {
            _constraint = [NSLayoutConstraint constraintWithItem:firstView
                                                       attribute:firstAttr
                                                       relatedBy:relation
                                                          toItem:secondView
                                                       attribute:secondAttr
                                                      multiplier:params.multiplier
                                                        constant:params.constant];
            
            if (!_constraint) {
                return nil;
            }
            
            // Set priority (convert from 0.0-1000.0 to NSLayoutPriority)
            NSLayoutPriority priority = (NSLayoutPriority)params.priority;
            if (priority < 1.0) {
                priority = NSLayoutPriorityRequired;
            } else if (priority > 1000.0) {
                priority = NSLayoutPriorityRequired;
            }
            [_constraint setPriority:priority];
            
            // Initially inactive (must be explicitly activated)
            [_constraint setActive:NO];
        } @catch (NSException* exception) {
            // Constraint creation failed - return nil to indicate failure
            _constraint = nil;
            return nil;
        }
    }
    return self;
}

- (bool)isValid {
    return _constraint != nil;
}

@end

// C API implementation

ObsidianLayoutConstraintHandle obsidian_macos_create_constraint(
    ObsidianLayoutConstraintParams params
) {
    if (!params.firstView) {
        return NULL;
    }
    
    @autoreleasepool {
        ObsidianLayoutConstraintWrapper* wrapper = [[ObsidianLayoutConstraintWrapper alloc] initWithParams:params];
        if (!wrapper || ![wrapper isValid]) {
            return NULL;
        }
        
        // Retain the wrapper and return as opaque handle
        return (__bridge_retained void*)wrapper;
    }
}

void obsidian_macos_activate_constraints(
    ObsidianLayoutConstraintHandle* constraints,
    size_t count
) {
    if (!constraints || count == 0) {
        return;
    }
    
    NSMutableArray<NSLayoutConstraint*>* constraintArray = [NSMutableArray arrayWithCapacity:count];
    
    for (size_t i = 0; i < count; i++) {
        if (!constraints[i]) {
            continue;
        }
        
        ObsidianLayoutConstraintWrapper* wrapper = (__bridge ObsidianLayoutConstraintWrapper*)constraints[i];
        if (wrapper && wrapper.constraint) {
            [constraintArray addObject:wrapper.constraint];
        }
    }
    
    if (constraintArray.count > 0) {
        [NSLayoutConstraint activateConstraints:constraintArray];
    }
}

void obsidian_macos_deactivate_constraints(
    ObsidianLayoutConstraintHandle* constraints,
    size_t count
) {
    if (!constraints || count == 0) {
        return;
    }
    
    NSMutableArray<NSLayoutConstraint*>* constraintArray = [NSMutableArray arrayWithCapacity:count];
    
    for (size_t i = 0; i < count; i++) {
        if (!constraints[i]) {
            continue;
        }
        
        ObsidianLayoutConstraintWrapper* wrapper = (__bridge ObsidianLayoutConstraintWrapper*)constraints[i];
        if (wrapper && wrapper.constraint) {
            [constraintArray addObject:wrapper.constraint];
        }
    }
    
    if (constraintArray.count > 0) {
        [NSLayoutConstraint deactivateConstraints:constraintArray];
    }
}

void obsidian_macos_destroy_constraint(
    ObsidianLayoutConstraintHandle handle
) {
    if (!handle) {
        return;
    }
    
    ObsidianLayoutConstraintWrapper* wrapper = (__bridge_transfer ObsidianLayoutConstraintWrapper*)handle;
    
    // Deactivate constraint before releasing
    if (wrapper.constraint) {
        [wrapper.constraint setActive:NO];
    }
    
    // wrapper is automatically released due to __bridge_transfer
}

bool obsidian_macos_constraint_is_valid(
    ObsidianLayoutConstraintHandle handle
) {
    if (!handle) {
        return false;
    }
    
    ObsidianLayoutConstraintWrapper* wrapper = (__bridge ObsidianLayoutConstraintWrapper*)handle;
    return wrapper && [wrapper isValid];
}
