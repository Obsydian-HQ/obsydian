/**
 * Obsidian Public API - HStack Implementation
 * 
 * This file implements the public HStack API by wrapping the internal
 * platform-specific implementations and layout engine.
 */

#include "obsidian/hstack.h"
#include "obsidian/window.h"
#include "obsidian/screen_container.h"
#include "obsidian/button.h"
#include "obsidian/link.h"
#include "obsidian/spacer.h"
#include "obsidian/textview.h"
#include "obsidian/vstack.h"
#include <iostream>

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_button.h"  // For getting view handles
#include "macos_window.h"  // For window content view
#include "macos_hstack.h"  // For HStack FFI
#include "macos_layout_ffi.h"  // For layout constraints
#include "macos_layout.h"  // For ObsidianLayoutAttribute
#include "macos_screen_container.h"  // For Screen FFI
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

class HStack::Impl {
public:
#ifdef __APPLE__
    ObsidianHStackHandle hstackHandle;
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
        : hstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Top)
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

HStack::HStack() : pImpl(std::make_unique<Impl>()) {}

HStack::~HStack() {
    if (pImpl && pImpl->valid) {
        // NOTE: We do NOT call removeFromParent() here.
        // The native view should remain in the view hierarchy until explicitly cleared.
        // This allows HStacks created on the stack (local variables in route functions)
        // to remain visible after the function returns.
#ifdef __APPLE__
        // Clear constraints to avoid dangling references
        pImpl->clearConstraints();
        pImpl->clearContainerConstraints();
        // Release our handle but do NOT remove from parent
        if (pImpl->hstackHandle) {
            obsidian_macos_release_hstack_handle(pImpl->hstackHandle);
            pImpl->hstackHandle = nullptr;
        }
#endif
        pImpl->valid = false;
    }
}

void HStack::setSpacing(double spacing) {
    if (pImpl) {
        pImpl->spacing = spacing;
        if (pImpl->valid) {
#ifdef __APPLE__
            // Update spacing on the custom container view
            obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, spacing);
#endif
            updateLayout();
        }
    }
}

double HStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void HStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        if (pImpl->valid) {
            updateLayout();
        }
    }
}

layout::Alignment HStack::getAlignment() const {
    return pImpl ? pImpl->alignment : layout::Alignment::Top;
}

void HStack::setPadding(const Padding& padding) {
    if (pImpl) {
        pImpl->padding = padding;
        if (pImpl->valid) {
#ifdef __APPLE__
            // Update padding on the custom container view
            obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                              padding.top, padding.bottom,
                                              padding.leading, padding.trailing);
#endif
            updateLayout();
        }
    }
}

Padding HStack::getPadding() const {
    return pImpl ? pImpl->padding : Padding{0.0, 0.0, 0.0, 0.0};
}

void HStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Update layout ONLY if container is already in window hierarchy
    // Otherwise, layout will be updated when addToWindow is called
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
    
    // Update layout ONLY if container is already in window hierarchy
    // Otherwise, layout will be updated when addToWindow is called
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get link's native view handle (which is the button's view)
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    // Remove link from current parent if it has one
    link.removeFromParent();
    
    // Add child view to container
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
    
    // Update layout ONLY if container is already in window hierarchy
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get text view's ACTUAL native view handle (NSTextView*, not the wrapper)
    // This is critical because the HStack needs the real NSView to apply Auto Layout constraints
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    // Remove text view from current parent if it has one
    textView.removeFromParent();
    
    // Add child view to container
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
    
    // Update layout ONLY if container is already in window hierarchy
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::addChild(VStack& vstack) {
    if (!pImpl || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get nested VStack's native view handle
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    // Remove nested VStack from current parent if it has one
    vstack.removeFromParent();
    
    // Add child view to container
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // Update layout ONLY if container is already in window hierarchy
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get nested HStack's native view handle
    void* nestedView = hstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    // Remove nested HStack from current parent if it has one
    hstack.removeFromParent();
    
    // Add child view to container
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // Update layout ONLY if container is already in window hierarchy
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void HStack::removeChild(Button& button) {
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
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::removeChild(Link& link) {
    if (!pImpl || !pImpl->valid || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), linkView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::removeChild(Spacer& spacer) {
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
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::removeChild(TextView& textView) {
    if (!pImpl || !pImpl->valid || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Use the ACTUAL native view handle to match what was added
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), textViewHandle);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, textViewHandle);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::removeChild(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::removeChild(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = hstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    // Find and remove from child handles
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
        
        // Update layout
        updateLayout();
    }
#endif
}

void HStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Remove all children
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
    
    // Clear constraints
    pImpl->clearConstraints();
#endif
}

void HStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    
    // Add HStack to window
    obsidian_macos_hstack_add_to_window(pImpl->hstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Set padding and spacing on the custom container view
    // This updates the container's intrinsic content size calculation
    obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                     pImpl->padding.top, pImpl->padding.bottom,
                                     pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, pImpl->spacing);
    
    // CRITICAL: Update layout for children FIRST
    // This ensures children are constrained to the container, which causes the container's
    // intrinsic content size to be calculated (based on children + padding + spacing)
    // The container MUST have a valid intrinsic content size before we can create
    // centerX/centerY constraints, otherwise Auto Layout can't resolve them
    updateLayout();
    
    // Force immediate layout pass so container gets its intrinsic content size
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
    
    // CRITICAL: Clear any existing container constraints before creating new ones
    // This prevents constraint conflicts if addToWindow is called multiple times
    pImpl->clearContainerConstraints();
    
    // Create container constraints to position it in contentView based on alignment
    // The container now has intrinsic content size (from custom NSView subclass + children)
    // so we can position it without pinning to all edges
    void* containerView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    if (containerView) {
        // Determine positioning based on alignment
        layout::Alignment align = pImpl->alignment;
        
        // Vertical alignment
        if (align == layout::Alignment::Middle || 
            align == layout::Alignment::MiddleLeading || 
            align == layout::Alignment::MiddleCenter || 
            align == layout::Alignment::MiddleTrailing) {
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
        
        // Horizontal alignment
        // For HStack, Center alignment should center both horizontally and vertically
        // The container's width is determined by its intrinsic content size (children + padding)
        if (align == layout::Alignment::Center || 
            align == layout::Alignment::MiddleCenter || 
            align == layout::Alignment::TopCenter || 
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
        
        // For layout showcase: Pin trailing edge to fill width so spacer can expand/contract
        // When using BottomLeading alignment, also pin trailing to window trailing
        if (align == layout::Alignment::BottomLeading) {
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
        
        // Container's height comes from intrinsic content size
        // Width is now constrained by leading+trailing when using BottomLeading
    }
    
    // CRITICAL: Update window constraints to prevent shrinking in macOS 15+ (Sequoia)
    // In macOS 15+, layout is computed more lazily, and the window may resize
    // based on contentView's fittingSize. We must explicitly update constraints.
    obsidian_macos_window_update_constraints(windowHandle);
#endif
}

void HStack::addToScreen(Screen& screen) {
    if (!pImpl) {
        return;
    }
    
    void* screenContentView = screen.getNativeHandle();
    if (screenContentView) {
        addToParentView(screenContentView);
    }
}

void HStack::addToParentView(void* parentView) {
    if (!pImpl || !parentView) {
        return;
    }
    
#ifdef __APPLE__
    // Initialize HStack if not already created
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    // Get the HStack's view
    void* hstackView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    if (!hstackView) {
        return;
    }
    
    // Add HStack view to parent
    obsidian_macos_screen_add_subview(parentView, hstackView);
    pImpl->parentView = parentView;
    
    // Set padding and spacing on the custom container view
    obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
                                       pImpl->padding.top, pImpl->padding.bottom,
                                       pImpl->padding.leading, pImpl->padding.trailing);
    obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, pImpl->spacing);
    
    // Update layout for children
    updateLayout();
    
    // Force immediate layout pass
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
    
    // Create container constraints to pin to parent
    void* containerView = hstackView;
    
    // For screens, pin to all edges to fill the screen
    ObsidianLayoutConstraintParams leadingParams;
    leadingParams.firstView = containerView;
    leadingParams.firstAttribute = ObsidianLayoutAttributeLeading;
    leadingParams.relation = ObsidianLayoutRelationEqual;
    leadingParams.secondView = parentView;
    leadingParams.secondAttribute = ObsidianLayoutAttributeLeading;
    leadingParams.multiplier = 1.0;
    leadingParams.constant = 0.0;
    leadingParams.priority = 1000.0;
    pImpl->containerLeadingConstraint.create(leadingParams);
    pImpl->containerLeadingConstraint.activate();
    
    ObsidianLayoutConstraintParams trailingParams;
    trailingParams.firstView = containerView;
    trailingParams.firstAttribute = ObsidianLayoutAttributeTrailing;
    trailingParams.relation = ObsidianLayoutRelationEqual;
    trailingParams.secondView = parentView;
    trailingParams.secondAttribute = ObsidianLayoutAttributeTrailing;
    trailingParams.multiplier = 1.0;
    trailingParams.constant = 0.0;
    trailingParams.priority = 1000.0;
    pImpl->containerTrailingConstraint.create(trailingParams);
    pImpl->containerTrailingConstraint.activate();
    
    ObsidianLayoutConstraintParams topParams;
    topParams.firstView = containerView;
    topParams.firstAttribute = ObsidianLayoutAttributeTop;
    topParams.relation = ObsidianLayoutRelationEqual;
    topParams.secondView = parentView;
    topParams.secondAttribute = ObsidianLayoutAttributeTop;
    topParams.multiplier = 1.0;
    topParams.constant = 0.0;
    topParams.priority = 1000.0;
    pImpl->containerTopConstraint.create(topParams);
    pImpl->containerTopConstraint.activate();
    
    ObsidianLayoutConstraintParams bottomParams;
    bottomParams.firstView = containerView;
    bottomParams.firstAttribute = ObsidianLayoutAttributeBottom;
    bottomParams.relation = ObsidianLayoutRelationEqual;
    bottomParams.secondView = parentView;
    bottomParams.secondAttribute = ObsidianLayoutAttributeBottom;
    bottomParams.multiplier = 1.0;
    bottomParams.constant = 0.0;
    bottomParams.priority = 1000.0;
    pImpl->containerBottomConstraint.create(bottomParams);
    pImpl->containerBottomConstraint.activate();
#endif
}

void HStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Clear child constraints first
    pImpl->clearConstraints();
    
    // Clear container constraints (these are only cleared when removing from parent)
    pImpl->clearContainerConstraints();
    
    // Remove from parent
    obsidian_macos_hstack_remove_from_parent(pImpl->hstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void HStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    if (pImpl->childViewHandles.empty()) {
        return;
    }
    
    void* containerView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    if (!containerView) {
        return;
    }
    
    // Clear existing constraints
    pImpl->clearConstraints();
    pImpl->constraints.clear();
    
    // Generate constraints using layout engine
    std::vector<layout::LayoutEngine::ConstraintSpec> constraintSpecs = 
        pImpl->layoutEngine.generateHStackConstraints(
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
    obsidian_macos_hstack_request_layout_update(pImpl->hstackHandle);
#endif
}

bool HStack::isValid() const {
    return pImpl ? pImpl->valid : false;
}

void* HStack::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    if (pImpl->hstackHandle) {
        return obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    }
#endif
    return nullptr;
}

HStack::HStack(HStack&&) noexcept = default;
HStack& HStack::operator=(HStack&&) noexcept = default;

} // namespace obsidian
