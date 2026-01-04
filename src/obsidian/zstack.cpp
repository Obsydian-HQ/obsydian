/**
 * Obsidian Public API - ZStack Implementation
 * 
 * This file implements the public ZStack API by wrapping the internal
 * platform-specific implementations and layout engine.
 */

#include "obsidian/zstack.h"
#include "obsidian/window.h"
#include "obsidian/button.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_button.h"  // For getting view handles
#include "macos_window.h"  // For window content view
#include "macos_zstack.h"  // For ZStack FFI
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

class ZStack::Impl {
public:
#ifdef __APPLE__
    ObsidianZStackHandle zstackHandle;
    std::vector<obsidian::ffi::macos::LayoutConstraint> constraints;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    layout::Alignment alignment;
    layout::LayoutEngine layoutEngine;
    
    Impl()
#ifdef __APPLE__
        : zstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , alignment(layout::Alignment::Center)
    {}
    
    ~Impl() {
        if (valid) {
            clearConstraints();
        }
    }
    
    void clearConstraints() {
#ifdef __APPLE__
        for (auto& constraint : constraints) {
            if (constraint.isValid()) {
                constraint.deactivate();
            }
        }
        constraints.clear();
#endif
    }
};

ZStack::ZStack() : pImpl(std::make_unique<Impl>()) {}

ZStack::~ZStack() {
    if (pImpl && pImpl->valid) {
        removeFromParent();
#ifdef __APPLE__
        if (pImpl->zstackHandle) {
            obsidian_macos_destroy_zstack(pImpl->zstackHandle);
            pImpl->zstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void ZStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        if (pImpl->valid) {
            updateLayout();
        }
    }
}

layout::Alignment ZStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Center;
}

void ZStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize ZStack if not already created
    if (!pImpl->valid) {
        ObsidianZStackParams params;
        pImpl->zstackHandle = obsidian_macos_create_zstack(params);
        if (!pImpl->zstackHandle) {
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
    obsidian_macos_zstack_add_child_view(pImpl->zstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Update layout ONLY if container is already in window hierarchy
    // Otherwise, layout will be updated when addToWindow is called
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void ZStack::removeChild(Button& button) {
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
        obsidian_macos_zstack_remove_child_view(pImpl->zstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void ZStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Remove all children
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_zstack_remove_child_view(pImpl->zstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
    
    // Clear constraints
    pImpl->clearConstraints();
#endif
}

void ZStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize ZStack if not already created
    if (!pImpl->valid) {
        ObsidianZStackParams params;
        pImpl->zstackHandle = obsidian_macos_create_zstack(params);
        if (!pImpl->zstackHandle) {
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
    
    // Add ZStack to window (container will fill the content view)
    obsidian_macos_zstack_add_to_window(pImpl->zstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Update layout for children
    updateLayout();
    
    // Force immediate layout update
    obsidian_macos_zstack_request_layout_update(pImpl->zstackHandle);
    
    // CRITICAL: Update window constraints to prevent shrinking in macOS 15+ (Sequoia)
    obsidian_macos_window_update_constraints(windowHandle);
#endif
}

void ZStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Clear child constraints first
    pImpl->clearConstraints();
    
    // Remove from parent
    obsidian_macos_zstack_remove_from_parent(pImpl->zstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void ZStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->childViewHandles.empty()) {
        return;
    }
    
    void* containerView = obsidian_macos_zstack_get_view(pImpl->zstackHandle);
    if (!containerView) {
        return;
    }
    
    // Clear existing constraints
    pImpl->clearConstraints();
    pImpl->constraints.clear();
    
    // Generate constraints using layout engine
    std::vector<layout::LayoutEngine::ConstraintSpec> constraintSpecs = 
        pImpl->layoutEngine.generateZStackConstraints(
            containerView,
            pImpl->childViewHandles,
            pImpl->alignment
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
    obsidian_macos_zstack_request_layout_update(pImpl->zstackHandle);
#endif
}

bool ZStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

ZStack::ZStack(ZStack&&) noexcept = default;
ZStack& ZStack::operator=(ZStack&&) noexcept = default;

} // namespace obsidian
