/**
 * Layout Constraint System
 * 
 * Core constraint representation for the layout engine
 */

#pragma once

#include <cstdint>
#include <cstddef>

namespace obsidian::layout {

/**
 * Constraint attribute
 * Represents what property of a view is being constrained
 */
enum class ConstraintAttribute {
    Leading = 1,
    Trailing = 2,
    Top = 3,
    Bottom = 4,
    Width = 5,
    Height = 6,
    CenterX = 7,
    CenterY = 8
};

/**
 * Constraint relation
 * Defines the relationship between two attributes
 */
enum class ConstraintRelation {
    Equal = 0,
    GreaterThanOrEqual = 1,
    LessThanOrEqual = -1
};

/**
 * Constraint parameters
 * All parameters needed to create a constraint
 */
struct ConstraintParams {
    void* firstView;                    // First view (must not be null)
    ConstraintAttribute firstAttribute; // Attribute of first view
    ConstraintRelation relation;        // Relationship type
    void* secondView;                   // Second view (null for constant-only)
    ConstraintAttribute secondAttribute;// Attribute of second view
    double multiplier;                  // Multiplier (default 1.0)
    double constant;                    // Constant offset (default 0.0)
    double priority;                    // Priority 0.0-1000.0 (default 1000.0 = required)
    
    ConstraintParams()
        : firstView(nullptr)
        , firstAttribute(ConstraintAttribute::Leading)
        , relation(ConstraintRelation::Equal)
        , secondView(nullptr)
        , secondAttribute(ConstraintAttribute::Leading)
        , multiplier(1.0)
        , constant(0.0)
        , priority(1000.0)
    {}
};

/**
 * Constraint handle (opaque)
 * Platform-specific constraint handle
 */
using ConstraintHandle = void*;

} // namespace obsidian::layout
