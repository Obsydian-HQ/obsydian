/**
 * macOS Layout FFI - C Interface
 * 
 * Provides C interface for creating and managing NSLayoutConstraint instances
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque handle to a macOS layout constraint
 */
typedef void* ObsidianLayoutConstraintHandle;

/**
 * Layout constraint attributes
 * Maps to NSLayoutAttribute
 * Values must match NSLayoutAttribute enum values exactly
 */
typedef enum {
    ObsidianLayoutAttributeLeft = 1,        // NSLayoutAttributeLeft
    ObsidianLayoutAttributeRight = 2,       // NSLayoutAttributeRight
    ObsidianLayoutAttributeTop = 3,         // NSLayoutAttributeTop
    ObsidianLayoutAttributeBottom = 4,      // NSLayoutAttributeBottom
    ObsidianLayoutAttributeLeading = 5,     // NSLayoutAttributeLeading
    ObsidianLayoutAttributeTrailing = 6,    // NSLayoutAttributeTrailing
    ObsidianLayoutAttributeWidth = 7,       // NSLayoutAttributeWidth
    ObsidianLayoutAttributeHeight = 8,      // NSLayoutAttributeHeight
    ObsidianLayoutAttributeCenterX = 9,     // NSLayoutAttributeCenterX
    ObsidianLayoutAttributeCenterY = 10     // NSLayoutAttributeCenterY
} ObsidianLayoutAttribute;

/**
 * Constraint relationship
 * Maps to NSLayoutRelation
 */
typedef enum {
    ObsidianLayoutRelationEqual = 0,
    ObsidianLayoutRelationGreaterThanOrEqual = 1,
    ObsidianLayoutRelationLessThanOrEqual = -1
} ObsidianLayoutRelation;

/**
 * Constraint parameters
 */
typedef struct {
    void* firstView;
    ObsidianLayoutAttribute firstAttribute;
    ObsidianLayoutRelation relation;
    void* secondView;  // NULL for constant-only constraints
    ObsidianLayoutAttribute secondAttribute;
    double multiplier;
    double constant;
    double priority;  // 0.0 to 1000.0
} ObsidianLayoutConstraintParams;

/**
 * Create a layout constraint
 * Returns a handle to the constraint, or NULL on failure
 */
ObsidianLayoutConstraintHandle obsidian_macos_create_constraint(
    ObsidianLayoutConstraintParams params
);

/**
 * Activate constraints
 * @param constraints Array of constraint handles
 * @param count Number of constraints in the array
 */
void obsidian_macos_activate_constraints(
    ObsidianLayoutConstraintHandle* constraints,
    size_t count
);

/**
 * Deactivate constraints
 * @param constraints Array of constraint handles
 * @param count Number of constraints in the array
 */
void obsidian_macos_deactivate_constraints(
    ObsidianLayoutConstraintHandle* constraints,
    size_t count
);

/**
 * Destroy a constraint
 * Note: Constraints should be deactivated before destruction
 */
void obsidian_macos_destroy_constraint(
    ObsidianLayoutConstraintHandle handle
);

/**
 * Check if a constraint handle is valid
 */
bool obsidian_macos_constraint_is_valid(
    ObsidianLayoutConstraintHandle handle
);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__
