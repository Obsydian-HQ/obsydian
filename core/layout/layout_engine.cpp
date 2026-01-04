/**
 * Layout Engine Implementation
 * 
 * Cross-platform layout algorithm and constraint generation
 */

#include "layout_engine.h"
#include <algorithm>
#include <cassert>

namespace obsidian::layout {

LayoutEngine::LayoutEngine() = default;
LayoutEngine::~LayoutEngine() = default;

std::vector<LayoutEngine::ConstraintSpec> LayoutEngine::generateVStackConstraints(
    void* parentView,
    const std::vector<void*>& childViews,
    double spacing,
    Alignment alignment,
    double paddingTop,
    double paddingBottom,
    double paddingLeading,
    double paddingTrailing
) {
    std::vector<ConstraintSpec> constraints;
    
    if (!parentView || childViews.empty()) {
        return constraints;
    }
    
    // For each child, create horizontal alignment constraints
    for (void* childView : childViews) {
        if (!childView) continue;
        
        ConstraintSpec alignSpec = createHorizontalAlignmentConstraint(
            childView, parentView, alignment, paddingLeading, paddingTrailing
        );
        if (alignSpec.params.firstView) {
            alignSpec.shouldActivate = true;
            constraints.push_back(alignSpec);
        }
    }
    
    // Create vertical spacing constraints
    if (childViews.size() == 1) {
        // Single child: pin to top and bottom with padding
        void* childView = childViews[0];
        if (childView) {
            // Top constraint
            ConstraintSpec topSpec;
            topSpec.params.firstView = childView;
            topSpec.params.firstAttribute = ConstraintAttribute::Top;
            topSpec.params.relation = ConstraintRelation::Equal;
            topSpec.params.secondView = parentView;
            topSpec.params.secondAttribute = ConstraintAttribute::Top;
            topSpec.params.multiplier = 1.0;
            topSpec.params.constant = paddingTop;
            topSpec.params.priority = 1000.0;
            topSpec.shouldActivate = true;
            constraints.push_back(topSpec);
            
            // Bottom constraint
            ConstraintSpec bottomSpec;
            bottomSpec.params.firstView = childView;
            bottomSpec.params.firstAttribute = ConstraintAttribute::Bottom;
            bottomSpec.params.relation = ConstraintRelation::Equal;
            bottomSpec.params.secondView = parentView;
            bottomSpec.params.secondAttribute = ConstraintAttribute::Bottom;
            bottomSpec.params.multiplier = 1.0;
            bottomSpec.params.constant = -paddingBottom;
            bottomSpec.params.priority = 1000.0;
            bottomSpec.shouldActivate = true;
            constraints.push_back(bottomSpec);
        }
    } else if (childViews.size() > 1) {
        // First child: pin to top with padding
        void* firstChild = childViews[0];
        if (firstChild) {
            ConstraintSpec topSpec;
            topSpec.params.firstView = firstChild;
            topSpec.params.firstAttribute = ConstraintAttribute::Top;
            topSpec.params.relation = ConstraintRelation::Equal;
            topSpec.params.secondView = parentView;
            topSpec.params.secondAttribute = ConstraintAttribute::Top;
            topSpec.params.multiplier = 1.0;
            topSpec.params.constant = paddingTop;
            topSpec.params.priority = 1000.0;
            topSpec.shouldActivate = true;
            constraints.push_back(topSpec);
        }
        
        // Spacing constraints between children
        for (size_t i = 0; i < childViews.size() - 1; i++) {
            void* currentChild = childViews[i];
            void* nextChild = childViews[i + 1];
            
            if (!currentChild || !nextChild) continue;
            
            ConstraintSpec spacingSpec;
            spacingSpec.params.firstView = nextChild;
            spacingSpec.params.firstAttribute = ConstraintAttribute::Top;
            spacingSpec.params.relation = ConstraintRelation::Equal;
            spacingSpec.params.secondView = currentChild;
            spacingSpec.params.secondAttribute = ConstraintAttribute::Bottom;
            spacingSpec.params.multiplier = 1.0;
            spacingSpec.params.constant = spacing;
            spacingSpec.params.priority = 1000.0;
            spacingSpec.shouldActivate = true;
            constraints.push_back(spacingSpec);
        }
        
        // Last child: pin to bottom with padding
        void* lastChild = childViews[childViews.size() - 1];
        if (lastChild) {
            ConstraintSpec bottomSpec;
            bottomSpec.params.firstView = lastChild;
            bottomSpec.params.firstAttribute = ConstraintAttribute::Bottom;
            bottomSpec.params.relation = ConstraintRelation::Equal;
            bottomSpec.params.secondView = parentView;
            bottomSpec.params.secondAttribute = ConstraintAttribute::Bottom;
            bottomSpec.params.multiplier = 1.0;
            bottomSpec.params.constant = -paddingBottom;
            bottomSpec.params.priority = 1000.0;
            bottomSpec.shouldActivate = true;
            constraints.push_back(bottomSpec);
        }
    }
    
    return constraints;
}

std::vector<LayoutEngine::ConstraintSpec> LayoutEngine::generateHStackConstraints(
    void* parentView,
    const std::vector<void*>& childViews,
    double spacing,
    Alignment alignment,
    double paddingTop,
    double paddingBottom,
    double paddingLeading,
    double paddingTrailing
) {
    std::vector<ConstraintSpec> constraints;
    
    if (!parentView || childViews.empty()) {
        return constraints;
    }
    
    // For each child, create vertical alignment constraints
    for (void* childView : childViews) {
        if (!childView) continue;
        
        ConstraintSpec alignSpec = createVerticalAlignmentConstraint(
            childView, parentView, alignment, paddingTop, paddingBottom
        );
        if (alignSpec.params.firstView) {
            alignSpec.shouldActivate = true;
            constraints.push_back(alignSpec);
        }
    }
    
    // Create horizontal spacing constraints
    if (childViews.size() == 1) {
        // Single child: pin to leading and trailing with padding
        void* childView = childViews[0];
        if (childView) {
            // Leading constraint
            ConstraintSpec leadingSpec;
            leadingSpec.params.firstView = childView;
            leadingSpec.params.firstAttribute = ConstraintAttribute::Leading;
            leadingSpec.params.relation = ConstraintRelation::Equal;
            leadingSpec.params.secondView = parentView;
            leadingSpec.params.secondAttribute = ConstraintAttribute::Leading;
            leadingSpec.params.multiplier = 1.0;
            leadingSpec.params.constant = paddingLeading;
            leadingSpec.params.priority = 1000.0;
            leadingSpec.shouldActivate = true;
            constraints.push_back(leadingSpec);
            
            // Trailing constraint
            ConstraintSpec trailingSpec;
            trailingSpec.params.firstView = childView;
            trailingSpec.params.firstAttribute = ConstraintAttribute::Trailing;
            trailingSpec.params.relation = ConstraintRelation::Equal;
            trailingSpec.params.secondView = parentView;
            trailingSpec.params.secondAttribute = ConstraintAttribute::Trailing;
            trailingSpec.params.multiplier = 1.0;
            trailingSpec.params.constant = -paddingTrailing;
            trailingSpec.params.priority = 1000.0;
            trailingSpec.shouldActivate = true;
            constraints.push_back(trailingSpec);
        }
    } else if (childViews.size() > 1) {
        // First child: pin to leading with padding
        void* firstChild = childViews[0];
        if (firstChild) {
            ConstraintSpec leadingSpec;
            leadingSpec.params.firstView = firstChild;
            leadingSpec.params.firstAttribute = ConstraintAttribute::Leading;
            leadingSpec.params.relation = ConstraintRelation::Equal;
            leadingSpec.params.secondView = parentView;
            leadingSpec.params.secondAttribute = ConstraintAttribute::Leading;
            leadingSpec.params.multiplier = 1.0;
            leadingSpec.params.constant = paddingLeading;
            leadingSpec.params.priority = 1000.0;
            leadingSpec.shouldActivate = true;
            constraints.push_back(leadingSpec);
        }
        
        // Spacing constraints between children
        for (size_t i = 0; i < childViews.size() - 1; i++) {
            void* currentChild = childViews[i];
            void* nextChild = childViews[i + 1];
            
            if (!currentChild || !nextChild) continue;
            
            ConstraintSpec spacingSpec;
            spacingSpec.params.firstView = nextChild;
            spacingSpec.params.firstAttribute = ConstraintAttribute::Leading;
            spacingSpec.params.relation = ConstraintRelation::Equal;
            spacingSpec.params.secondView = currentChild;
            spacingSpec.params.secondAttribute = ConstraintAttribute::Trailing;
            spacingSpec.params.multiplier = 1.0;
            spacingSpec.params.constant = spacing;
            spacingSpec.params.priority = 1000.0;
            spacingSpec.shouldActivate = true;
            constraints.push_back(spacingSpec);
        }
        
        // Last child: pin to trailing with padding
        void* lastChild = childViews[childViews.size() - 1];
        if (lastChild) {
            ConstraintSpec trailingSpec;
            trailingSpec.params.firstView = lastChild;
            trailingSpec.params.firstAttribute = ConstraintAttribute::Trailing;
            trailingSpec.params.relation = ConstraintRelation::Equal;
            trailingSpec.params.secondView = parentView;
            trailingSpec.params.secondAttribute = ConstraintAttribute::Trailing;
            trailingSpec.params.multiplier = 1.0;
            trailingSpec.params.constant = -paddingTrailing;
            trailingSpec.params.priority = 1000.0;
            trailingSpec.shouldActivate = true;
            constraints.push_back(trailingSpec);
        }
    }
    
    return constraints;
}

std::vector<LayoutEngine::ConstraintSpec> LayoutEngine::generateZStackConstraints(
    void* parentView,
    const std::vector<void*>& childViews,
    Alignment alignment
) {
    std::vector<ConstraintSpec> constraints;
    
    if (!parentView || childViews.empty()) {
        return constraints;
    }
    
    // Determine horizontal and vertical alignment attributes from combined alignment
    ConstraintAttribute hAttr = alignmentToHorizontalAttribute(alignment);
    ConstraintAttribute vAttr = alignmentToVerticalAttribute(alignment);
    
    // For ZStack, overlay all children at the same position
    // All children get the same constraints to align at the same point
    for (void* childView : childViews) {
        if (!childView) continue;
        
        // Create horizontal alignment constraint
        ConstraintSpec hSpec;
        hSpec.params.firstView = childView;
        hSpec.params.firstAttribute = hAttr;
        hSpec.params.relation = ConstraintRelation::Equal;
        hSpec.params.secondView = parentView;
        hSpec.params.secondAttribute = hAttr;
        hSpec.params.multiplier = 1.0;
        hSpec.params.constant = 0.0;
        hSpec.params.priority = 1000.0;
        hSpec.shouldActivate = true;
        constraints.push_back(hSpec);
        
        // Create vertical alignment constraint
        ConstraintSpec vSpec;
        vSpec.params.firstView = childView;
        vSpec.params.firstAttribute = vAttr;
        vSpec.params.relation = ConstraintRelation::Equal;
        vSpec.params.secondView = parentView;
        vSpec.params.secondAttribute = vAttr;
        vSpec.params.multiplier = 1.0;
        vSpec.params.constant = 0.0;
        vSpec.params.priority = 1000.0;
        vSpec.shouldActivate = true;
        constraints.push_back(vSpec);
    }
    
    return constraints;
}

ConstraintAttribute LayoutEngine::alignmentToHorizontalAttribute(Alignment alignment) {
    switch (alignment) {
        case Alignment::Leading:
        case Alignment::TopLeading:
        case Alignment::MiddleLeading:
        case Alignment::BottomLeading:
            return ConstraintAttribute::Leading;
            
        case Alignment::Center:
        case Alignment::TopCenter:
        case Alignment::MiddleCenter:
        case Alignment::BottomCenter:
            return ConstraintAttribute::CenterX;
            
        case Alignment::Trailing:
        case Alignment::TopTrailing:
        case Alignment::MiddleTrailing:
        case Alignment::BottomTrailing:
            return ConstraintAttribute::Trailing;
            
        default:
            return ConstraintAttribute::Leading;
    }
}

ConstraintAttribute LayoutEngine::alignmentToVerticalAttribute(Alignment alignment) {
    switch (alignment) {
        case Alignment::Top:
        case Alignment::TopLeading:
        case Alignment::TopCenter:
        case Alignment::TopTrailing:
            return ConstraintAttribute::Top;
            
        case Alignment::Middle:
        case Alignment::MiddleLeading:
        case Alignment::MiddleCenter:
        case Alignment::MiddleTrailing:
            return ConstraintAttribute::CenterY;
            
        case Alignment::Bottom:
        case Alignment::BottomLeading:
        case Alignment::BottomCenter:
        case Alignment::BottomTrailing:
            return ConstraintAttribute::Bottom;
            
        default:
            return ConstraintAttribute::Top;
    }
}

LayoutEngine::ConstraintSpec LayoutEngine::createHorizontalAlignmentConstraint(
    void* childView,
    void* parentView,
    Alignment alignment,
    double paddingLeading,
    double paddingTrailing
) {
    ConstraintSpec spec;
    
    if (!childView || !parentView) {
        return spec;
    }
    
    ConstraintAttribute attr = alignmentToHorizontalAttribute(alignment);
    
    if (attr == ConstraintAttribute::CenterX) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::CenterX;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::CenterX;
        spec.params.multiplier = 1.0;
        spec.params.constant = 0.0;
        spec.params.priority = 1000.0;
    } else if (attr == ConstraintAttribute::Leading) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::Leading;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::Leading;
        spec.params.multiplier = 1.0;
        spec.params.constant = paddingLeading;
        spec.params.priority = 1000.0;
    } else if (attr == ConstraintAttribute::Trailing) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::Trailing;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::Trailing;
        spec.params.multiplier = 1.0;
        spec.params.constant = -paddingTrailing;
        spec.params.priority = 1000.0;
    }
    
    return spec;
}

LayoutEngine::ConstraintSpec LayoutEngine::createVerticalAlignmentConstraint(
    void* childView,
    void* parentView,
    Alignment alignment,
    double paddingTop,
    double paddingBottom
) {
    ConstraintSpec spec;
    
    if (!childView || !parentView) {
        return spec;
    }
    
    ConstraintAttribute attr = alignmentToVerticalAttribute(alignment);
    
    if (attr == ConstraintAttribute::CenterY) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::CenterY;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::CenterY;
        spec.params.multiplier = 1.0;
        spec.params.constant = 0.0;
        spec.params.priority = 1000.0;
    } else if (attr == ConstraintAttribute::Top) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::Top;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::Top;
        spec.params.multiplier = 1.0;
        spec.params.constant = paddingTop;
        spec.params.priority = 1000.0;
    } else if (attr == ConstraintAttribute::Bottom) {
        spec.params.firstView = childView;
        spec.params.firstAttribute = ConstraintAttribute::Bottom;
        spec.params.relation = ConstraintRelation::Equal;
        spec.params.secondView = parentView;
        spec.params.secondAttribute = ConstraintAttribute::Bottom;
        spec.params.multiplier = 1.0;
        spec.params.constant = -paddingBottom;
        spec.params.priority = 1000.0;
    }
    
    return spec;
}

} // namespace obsidian::layout
