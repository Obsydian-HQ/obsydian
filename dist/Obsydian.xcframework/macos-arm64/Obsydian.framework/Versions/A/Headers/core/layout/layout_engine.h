/**
 * Layout Engine
 * 
 * Cross-platform layout algorithm and constraint generation
 * Provides the core logic for generating constraints from layout requirements
 */

#pragma once

#include "constraint.h"
#include "alignment.h"
#include <vector>
#include <memory>

namespace obsidian::layout {

/**
 * Layout engine
 * Generates constraints from layout requirements
 */
class LayoutEngine {
public:
    LayoutEngine();
    ~LayoutEngine();
    
    /**
     * Constraint specification
     * Describes a constraint to be created
     */
    struct ConstraintSpec {
        ConstraintParams params;
        bool shouldActivate;
        
        ConstraintSpec()
            : shouldActivate(false)
        {}
    };
    
    /**
     * Generate constraints for a vertical stack layout
     * @param parentView Parent container view
     * @param childViews Array of child views to arrange
     * @param spacing Spacing between children
     * @param alignment Horizontal alignment
     * @param paddingTop Top padding
     * @param paddingBottom Bottom padding
     * @param paddingLeading Leading padding
     * @param paddingTrailing Trailing padding
     * @return Vector of constraint specifications
     */
    std::vector<ConstraintSpec> generateVStackConstraints(
        void* parentView,
        const std::vector<void*>& childViews,
        double spacing,
        Alignment alignment,
        double paddingTop,
        double paddingBottom,
        double paddingLeading,
        double paddingTrailing
    );
    
    /**
     * Generate constraints for a horizontal stack layout
     * @param parentView Parent container view
     * @param childViews Array of child views to arrange
     * @param spacing Spacing between children
     * @param alignment Vertical alignment
     * @param paddingTop Top padding
     * @param paddingBottom Bottom padding
     * @param paddingLeading Leading padding
     * @param paddingTrailing Trailing padding
     * @return Vector of constraint specifications
     */
    std::vector<ConstraintSpec> generateHStackConstraints(
        void* parentView,
        const std::vector<void*>& childViews,
        double spacing,
        Alignment alignment,
        double paddingTop,
        double paddingBottom,
        double paddingLeading,
        double paddingTrailing
    );
    
    /**
     * Generate constraints for an overlay stack layout (ZStack)
     * @param parentView Parent container view
     * @param childViews Array of child views to overlay
     * @param alignment Alignment for positioning
     * @return Vector of constraint specifications
     */
    std::vector<ConstraintSpec> generateZStackConstraints(
        void* parentView,
        const std::vector<void*>& childViews,
        Alignment alignment
    );
    
    /**
     * Convert alignment to constraint attribute for horizontal alignment
     */
    static ConstraintAttribute alignmentToHorizontalAttribute(Alignment alignment);
    
    /**
     * Convert alignment to constraint attribute for vertical alignment
     */
    static ConstraintAttribute alignmentToVerticalAttribute(Alignment alignment);
    
    // Non-copyable
    LayoutEngine(const LayoutEngine&) = delete;
    LayoutEngine& operator=(const LayoutEngine&) = delete;
    
    // Movable
    LayoutEngine(LayoutEngine&&) noexcept = default;
    LayoutEngine& operator=(LayoutEngine&&) noexcept = default;

private:
    /**
     * Helper: Create constraint spec for leading/trailing alignment
     */
    ConstraintSpec createHorizontalAlignmentConstraint(
        void* childView,
        void* parentView,
        Alignment alignment,
        double paddingLeading,
        double paddingTrailing
    );
    
    /**
     * Helper: Create constraint spec for top/bottom alignment
     */
    ConstraintSpec createVerticalAlignmentConstraint(
        void* childView,
        void* parentView,
        Alignment alignment,
        double paddingTop,
        double paddingBottom
    );
};

} // namespace obsidian::layout
