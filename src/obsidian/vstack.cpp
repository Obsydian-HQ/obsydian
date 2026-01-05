/**
 * Obsidian Public API - VStack Implementation
 * 
 * This file implements the public VStack API by wrapping the internal
 * platform-specific implementations and layout engine.
 */

#include "obsidian/vstack.h"
#include "obsidian/window.h"
#include "obsidian/button.h"
#include "obsidian/spacer.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_button.h"  // For getting view handles
#include "macos_window.h"  // For window content view
#include "macos_vstack.h"  // For VStack FFI
#include "macos_layout_ffi.h"  // For layout constraints
#include "macos_layout.h"  // For ObsidianLayoutAttribute
#endif

// Include layout engine
#include "core/layout/layout_engine.h"
#include "core/layout/constraint.h"

#include <vector>
#include <algorithm>
#include <memory>

namespace obsidian {

#ifdef __APPLE__

// Helper function to convert ConstraintAttribute to ObsidianLayoutAttribute
static ObsidianLayoutAttribute convertAttribute(layout::ConstraintAttribute attr) {
    switch (attr) {
        case layout::ConstraintAttribute::Leading:
            return ObsidianLayoutAttributeLeading;
        case layout::ConstraintAttribute::Trailing:
            return ObsidianLayoutAttributeTrailing;
        case layout::ConstraintAttribute::Top:
            return ObsidianLayoutAttributeTop;
        case layout::ConstraintAttribute::Bottom:
            return ObsidianLayoutAttributeBottom;
        case layout::ConstraintAttribute::Width:
            return ObsidianLayoutAttributeWidth;
        case layout::ConstraintAttribute::Height:
            return ObsidianLayoutAttributeHeight;
        case layout::ConstraintAttribute::CenterX:
            return ObsidianLayoutAttributeCenterX;
        case layout::ConstraintAttribute::CenterY:
            return ObsidianLayoutAttributeCenterY;
        default:
            return ObsidianLayoutAttributeLeading;
    }
}

// Helper function to convert ConstraintRelation to ObsidianLayoutRelation
static ObsidianLayoutRelation convertRelation(layout::ConstraintRelation rel) {
    switch (rel) {
        case layout::ConstraintRelation::Equal:
            return ObsidianLayoutRelationEqual;
        case layout::ConstraintRelation::GreaterThanOrEqual:
            return ObsidianLayoutRelationGreaterThanOrEqual;
        case layout::ConstraintRelation::LessThanOrEqual:
            return ObsidianLayoutRelationLessThanOrEqual;
        default:
            return ObsidianLayoutRelationEqual;
    }
}

#endif

class VStack::Impl {
public:
#ifdef __APPLE__
    ObsidianVStackHandle vstackHandle;
    obsidian::ffi::macos::LayoutConstraint containerTopConstraint;
    obsidian::ffi::macos::LayoutConstraint containerLeadingConstraint;
    obsidian::ffi::macos::LayoutConstraint containerTrailingConstraint;
    obsidian::ffi::macos::LayoutConstraint containerBottomConstraint;
    std::vector<obsidian::ffi::macos::LayoutConstraint> constraints;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    double spacing;
    layout::Alignment alignment;
    Padding padding;
    layout::LayoutEngine layoutEngine;
    
    Impl()
#ifdef __APPLE__
        : vstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Leading)
        , padding{0.0, 0.0, 0.0, 0.0}
    {}
    
    ~Impl() {
        if (valid) {
            clearConstraints();
        }
    }
    
    void clearConstraints() {
#ifdef __APPLE__
        // CRITICAL: Only deactivate CHILD constraints, NOT container constraints!
        // Container constraints (Leading, Trailing, Top, Bottom) must remain active
        // to maintain container size. They are only cleared when removeFromParent() is called.
        for (auto& constraint : constraints) {
            if (constraint.isValid()) {
                constraint.deactivate();
            }
        }
        constraints.clear();
#endif
    }
    
    void clearContainerConstraints() {
#ifdef __APPLE__
        // Clear container constraints (called only when removing from parent)
        if (containerTopConstraint.isValid()) {
            containerTopConstraint.deactivate();
        }
        if (containerLeadingConstraint.isValid()) {
            containerLeadingConstraint.deactivate();
        }
        if (containerTrailingConstraint.isValid()) {
            containerTrailingConstraint.deactivate();
        }
        if (containerBottomConstraint.isValid()) {
            containerBottomConstraint.deactivate();
        }
#endif
    }
};

VStack::VStack() : pImpl(std::make_unique<Impl>()) {}

VStack::~VStack() {
    if (pImpl && pImpl->valid) {
        removeFromParent();
#ifdef __APPLE__
        if (pImpl->vstackHandle) {
            obsidian_macos_destroy_vstack(pImpl->vstackHandle);
            pImpl->vstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void VStack::setSpacing(double spacing) {
    if (pImpl) {
        pImpl->spacing = spacing;
        if (pImpl->valid) {
#ifdef __APPLE__
            // Update spacing on the custom container view
            obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, spacing);
#endif
            updateLayout();
        }
    }
}

double VStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void VStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        if (pImpl->valid) {
            updateLayout();
        }
    }
}

layout::Alignment VStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Leading;
}

void VStack::setPadding(const Padding& padding) {
    if (pImpl) {
        pImpl->padding = padding;
        if (pImpl->valid) {
#ifdef __APPLE__
            // Update padding on the custom container view
            obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                               padding.top, padding.bottom,
                                               padding.leading, padding.trailing);
#endif
            updateLayout();
        }
    }
}

Padding VStack::getPadding() const {
    return pImpl ? pImpl->padding : Padding{0.0, 0.0, 0.0, 0.0};
}

void VStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize VStack if not already created
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get button's native view handle
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    // Remove button from current parent if it has one (button can only have one parent)
    button.removeFromParent();
    
    // Add child view to container
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Update layout ONLY if container is already in window hierarchy
    // Otherwise, layout will be updated when addToWindow is called
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void VStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize VStack if not already created
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get spacer's native view handle
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    // Add child view to container
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
    
    // Update layout ONLY if container is already in window hierarchy
    // Otherwise, layout will be updated when addToWindow is called
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void VStack::removeChild(Button& button) {
    if (!pImpl || !pImpl->valid || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), buttonView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void VStack::removeChild(Spacer& spacer) {
    if (!pImpl || !pImpl->valid || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), spacerView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void VStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Remove all children
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
    
    // Clear constraints
    pImpl->clearConstraints();
#endif
}

void VStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize VStack if not already created
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get window's content view
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add VStack to window
    obsidian_macos_vstack_add_to_window(pImpl->vstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Set padding and spacing on the custom container view
    // This updates the container's intrinsic content size calculation
    obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                       pImpl->padding.top, pImpl->padding.bottom,
                                       pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, pImpl->spacing);
    
    // CRITICAL: Update layout for children FIRST
    // This ensures children are constrained to the container, which causes the container's
    // intrinsic content size to be calculated (based on children + padding + spacing)
    // The container MUST have a valid intrinsic content size before we can create
    // centerX/centerY constraints, otherwise Auto Layout can't resolve them
    updateLayout();
    
    // Force immediate layout pass so container gets its intrinsic content size
    obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
    
    // Create container constraints to position it in contentView based on alignment
    // The container now has intrinsic content size (from custom NSView subclass + children)
    // so we can position it without pinning to all edges
    void* containerView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    if (containerView) {
        // Determine positioning based on alignment
        layout::Alignment align = pImpl->alignment;
        
        // Horizontal alignment
        if (align == layout::Alignment::Center || 
            align == layout::Alignment::TopCenter || 
            align == layout::Alignment::MiddleCenter || 
            align == layout::Alignment::BottomCenter) {
            // Center horizontally
            ObsidianLayoutConstraintParams centerXParams;
            centerXParams.firstView = containerView;
            centerXParams.firstAttribute = ObsidianLayoutAttributeCenterX;
            centerXParams.relation = ObsidianLayoutRelationEqual;
            centerXParams.secondView = contentView;
            centerXParams.secondAttribute = ObsidianLayoutAttributeCenterX;
            centerXParams.multiplier = 1.0;
            centerXParams.constant = 0.0;
            centerXParams.priority = 1000.0;
            obsidian::ffi::macos::LayoutConstraint centerXConstraint;
            if (centerXConstraint.create(centerXParams)) {
                centerXConstraint.activate();
                pImpl->constraints.push_back(std::move(centerXConstraint));
            }
        } else if (align == layout::Alignment::Leading || 
                   align == layout::Alignment::TopLeading || 
                   align == layout::Alignment::MiddleLeading || 
                   align == layout::Alignment::BottomLeading) {
            // Pin to leading edge
            ObsidianLayoutConstraintParams leadingParams;
            leadingParams.firstView = containerView;
            leadingParams.firstAttribute = ObsidianLayoutAttributeLeading;
            leadingParams.relation = ObsidianLayoutRelationEqual;
            leadingParams.secondView = contentView;
            leadingParams.secondAttribute = ObsidianLayoutAttributeLeading;
            leadingParams.multiplier = 1.0;
            leadingParams.constant = 0.0;
            leadingParams.priority = 1000.0;
            pImpl->containerLeadingConstraint.create(leadingParams);
            pImpl->containerLeadingConstraint.activate();
        } else {
            // Pin to trailing edge (Trailing, TopTrailing, MiddleTrailing, BottomTrailing)
            ObsidianLayoutConstraintParams trailingParams;
            trailingParams.firstView = containerView;
            trailingParams.firstAttribute = ObsidianLayoutAttributeTrailing;
            trailingParams.relation = ObsidianLayoutRelationEqual;
            trailingParams.secondView = contentView;
            trailingParams.secondAttribute = ObsidianLayoutAttributeTrailing;
            trailingParams.multiplier = 1.0;
            trailingParams.constant = 0.0;
            trailingParams.priority = 1000.0;
            pImpl->containerTrailingConstraint.create(trailingParams);
            pImpl->containerTrailingConstraint.activate();
        }
        
        // Vertical alignment
        // For VStack, Center alignment should center both horizontally and vertically
        // The container's height is determined by its intrinsic content size (children + padding)
        if (align == layout::Alignment::Center || 
            align == layout::Alignment::MiddleCenter || 
            align == layout::Alignment::MiddleLeading || 
            align == layout::Alignment::MiddleTrailing ||
            align == layout::Alignment::Middle) {
            // Center vertically
            ObsidianLayoutConstraintParams centerYParams;
            centerYParams.firstView = containerView;
            centerYParams.firstAttribute = ObsidianLayoutAttributeCenterY;
            centerYParams.relation = ObsidianLayoutRelationEqual;
            centerYParams.secondView = contentView;
            centerYParams.secondAttribute = ObsidianLayoutAttributeCenterY;
            centerYParams.multiplier = 1.0;
            centerYParams.constant = 0.0;
            centerYParams.priority = 1000.0;
            obsidian::ffi::macos::LayoutConstraint centerYConstraint;
            if (centerYConstraint.create(centerYParams)) {
                centerYConstraint.activate();
                pImpl->constraints.push_back(std::move(centerYConstraint));
            }
        } else if (align == layout::Alignment::Top || 
                   align == layout::Alignment::TopLeading || 
                   align == layout::Alignment::TopCenter || 
                   align == layout::Alignment::TopTrailing) {
            // Pin to top edge
            ObsidianLayoutConstraintParams topParams;
            topParams.firstView = containerView;
            topParams.firstAttribute = ObsidianLayoutAttributeTop;
            topParams.relation = ObsidianLayoutRelationEqual;
            topParams.secondView = contentView;
            topParams.secondAttribute = ObsidianLayoutAttributeTop;
            topParams.multiplier = 1.0;
            topParams.constant = 0.0;
            topParams.priority = 1000.0;
            pImpl->containerTopConstraint.create(topParams);
            pImpl->containerTopConstraint.activate();
        } else {
            // Pin to bottom edge (Bottom, BottomLeading, BottomCenter, BottomTrailing)
            ObsidianLayoutConstraintParams bottomParams;
            bottomParams.firstView = containerView;
            bottomParams.firstAttribute = ObsidianLayoutAttributeBottom;
            bottomParams.relation = ObsidianLayoutRelationEqual;
            bottomParams.secondView = contentView;
            bottomParams.secondAttribute = ObsidianLayoutAttributeBottom;
            bottomParams.multiplier = 1.0;
            bottomParams.constant = 0.0;
            bottomParams.priority = 1000.0;
            pImpl->containerBottomConstraint.create(bottomParams);
            pImpl->containerBottomConstraint.activate();
        }
        
        // For layout showcase: Pin bottom edge to fill height so spacer can expand/contract
        // When using TopLeading alignment, also pin bottom to window bottom
        if (align == layout::Alignment::TopLeading) {
            ObsidianLayoutConstraintParams bottomParams;
            bottomParams.firstView = containerView;
            bottomParams.firstAttribute = ObsidianLayoutAttributeBottom;
            bottomParams.relation = ObsidianLayoutRelationEqual;
            bottomParams.secondView = contentView;
            bottomParams.secondAttribute = ObsidianLayoutAttributeBottom;
            bottomParams.multiplier = 1.0;
            bottomParams.constant = 0.0;
            bottomParams.priority = 1000.0;
            pImpl->containerBottomConstraint.create(bottomParams);
            pImpl->containerBottomConstraint.activate();
        }
        
        // Container's width comes from intrinsic content size
        // Height is now constrained by top+bottom when using TopLeading
    }
    
    // CRITICAL: Update window constraints to prevent shrinking in macOS 15+ (Sequoia)
    // In macOS 15+, layout is computed more lazily, and the window may resize
    // based on contentView's fittingSize. We must explicitly update constraints.
    obsidian_macos_window_update_constraints(windowHandle);
#endif
}

void VStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Clear child constraints first
    pImpl->clearConstraints();
    
    // Clear container constraints (these are only cleared when removing from parent)
    pImpl->clearContainerConstraints();
    
    // Remove from parent
    obsidian_macos_vstack_remove_from_parent(pImpl->vstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void VStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->childViewHandles.empty()) {
        return;
    }
    
    void* containerView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    if (!containerView) {
        return;
    }
    
    // Clear existing constraints
    pImpl->clearConstraints();
    pImpl->constraints.clear();
    
    // Generate constraints using layout engine
    std::vector<layout::LayoutEngine::ConstraintSpec> constraintSpecs = 
        pImpl->layoutEngine.generateVStackConstraints(
            containerView,
            pImpl->childViewHandles,
            pImpl->spacing,
            pImpl->alignment,
            pImpl->padding.top,
            pImpl->padding.bottom,
            pImpl->padding.leading,
            pImpl->padding.trailing
        );
    
    // Create and activate constraints
    for (const auto& spec : constraintSpecs) {
        if (!spec.shouldActivate) {
            continue;
        }
        
        ObsidianLayoutConstraintParams params;
        params.firstView = spec.params.firstView;
        params.firstAttribute = convertAttribute(spec.params.firstAttribute);
        params.relation = convertRelation(spec.params.relation);
        params.secondView = spec.params.secondView;
        params.secondAttribute = spec.params.secondView ? 
            convertAttribute(spec.params.secondAttribute) : ObsidianLayoutAttributeLeading;
        params.multiplier = spec.params.multiplier;
        params.constant = spec.params.constant;
        params.priority = spec.params.priority;
        
        obsidian::ffi::macos::LayoutConstraint constraint;
        if (constraint.create(params)) {
            constraint.activate();
            pImpl->constraints.push_back(std::move(constraint));
        }
    }
    
    // Force immediate layout update after activating child constraints
    // This ensures views are positioned correctly using constraints, not default frame positions
    // CRITICAL: Container constraints must remain active for this to work correctly
    obsidian_macos_vstack_request_layout_update(pImpl->vstackHandle);
#endif
}

bool VStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

void* VStack::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    if (pImpl->vstackHandle) {
        return obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    }
#endif
    return nullptr;
}

VStack::VStack(VStack&&) noexcept = default;
VStack& VStack::operator=(VStack&&) noexcept = default;

} // namespace obsidian
