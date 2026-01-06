/**
 * Obsidian Public API - VStack Implementation
 * 
 * Uses the Layout Engine to calculate positions and sizes.
 * The C++ Layout Engine computes frames, then applies them to native views.
 */

#include "obsidian/vstack.h"
#include "obsidian/window.h"
#include "obsidian/screen_container.h"
#include "obsidian/button.h"
#include "obsidian/link.h"
#include "obsidian/spacer.h"
#include "obsidian/textview.h"
#include "obsidian/hstack.h"
#include <iostream>

// Layout Engine
#include "core/layout/view_node.h"
#include "core/layout/engine.h"
#include "core/layout/manager.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#include "macos_window.h"
#include "macos_vstack.h"
#include "macos_screen_container.h"
#include "macos_textview.h"  // For text measurement like React Native's TextLayoutManager
#endif

#include <vector>
#include <algorithm>
#include <memory>

namespace obsidian {

// Forward declare FFI functions for layout engine
#ifdef __APPLE__
extern "C" void obsidian_macos_view_set_frame(void* view, double x, double y, double width, double height);
extern "C" void obsidian_macos_view_get_bounds(void* view, double* x, double* y, double* w, double* h);
#endif

class VStack::Impl {
public:
#ifdef __APPLE__
    ObsidianVStackHandle vstackHandle;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    double spacing;
    layout::Alignment alignment;
    Padding padding;
    
    // Layout Engine integration
    // IMPORTANT: layoutNode ownership model (inspired by React Native's Shadow Tree)
    // - When a VStack is created, it owns its layoutNode
    // - When a VStack is added as a child to another container, ownership transfers
    // - This ensures layoutNodes persist even when component objects go out of scope
    layout::ViewNode* layoutNode;
    std::vector<layout::LayoutNode*> childLayoutNodes;  // Tracked for reference, not ownership
    bool ownsLayoutNode;  // true if this VStack owns its layoutNode
    
    Impl()
#ifdef __APPLE__
        : vstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Leading)
        , padding{0.0, 0.0, 0.0, 0.0}
        , layoutNode(nullptr)
        , ownsLayoutNode(true)  // Initially owns its layoutNode
    {}
    
    ~Impl() {
        // Only delete layoutNode if we still own it
        // If ownership was transferred (e.g., when added as child to parent container),
        // the parent now owns it and will clean it up
        if (ownsLayoutNode && layoutNode) {
            delete layoutNode;
        }
    }
};

VStack::VStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view
    ObsidianVStackParams params;
    pImpl->vstackHandle = obsidian_macos_create_vstack(params);
    if (pImpl->vstackHandle) {
        pImpl->valid = true;
        
        // Create LayoutNode for this VStack (Column direction)
        void* nativeView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
        pImpl->layoutNode = layout::ViewNode::createContainer(layout::FlexDirection::Column, nativeView);
        
        // Set up the frame setter callback for layout engine
        layout::ViewNode::setSetFrameCallback([](void* view, float x, float y, float w, float h) {
            obsidian_macos_view_set_frame(view, x, y, w, h);
        });
    }
#endif
}

VStack::~VStack() {
    if (pImpl && pImpl->valid) {
#ifdef __APPLE__
        // Release handle but do NOT remove from parent
        // The NSView stays in the superview hierarchy (superview retains it)
        if (pImpl->vstackHandle) {
            obsidian_macos_release_vstack_handle(pImpl->vstackHandle);
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
            obsidian_macos_vstack_set_spacing(pImpl->vstackHandle, spacing);
            
            // Update layout node
            if (pImpl->layoutNode) {
                pImpl->layoutNode->getStyle().gap = static_cast<float>(spacing);
                pImpl->layoutNode->markDirty();
            }
#endif
        }
    }
}

double VStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void VStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
        // Note: Alignment is handled by how we position the container
        // Child alignment within VStack is always fill-width in frame-based layout
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
            obsidian_macos_vstack_set_padding(pImpl->vstackHandle,
                                               padding.top, padding.bottom,
                                               padding.leading, padding.trailing);
            
            // Update layout node padding [left, top, right, bottom]
            if (pImpl->layoutNode) {
                auto& style = pImpl->layoutNode->getStyle();
                style.padding[0] = layout::LayoutValue::points(static_cast<float>(padding.leading));
                style.padding[1] = layout::LayoutValue::points(static_cast<float>(padding.top));
                style.padding[2] = layout::LayoutValue::points(static_cast<float>(padding.trailing));
                style.padding[3] = layout::LayoutValue::points(static_cast<float>(padding.bottom));
                pImpl->layoutNode->markDirty();
            }
#endif
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
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) {
        return;
    }
    
    button.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Create LayoutNode for button (leaf node with fixed height)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(buttonView, nullptr);
        // Buttons have a default height of 30
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void VStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
    
    // Create LayoutNode for spacer (uses flexGrow to fill available space)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createSpacer(spacerView);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void VStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    link.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
    
    // Create LayoutNode for link (leaf node with fixed height)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(linkView, nullptr);
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void VStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    textView.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
    
    // Create LayoutNode for text view
    // Like React Native's TextLayoutManager, we measure the text to get its intrinsic height
    if (pImpl->layoutNode) {
        // Get the FFI handle to measure the text
        void* ffiHandle = textView.getNativeHandle();
        float textHeight = 24.0f;  // Default fallback
        
        if (ffiHandle) {
            // Measure text with unconstrained width to get natural height
            // This is similar to React Native's text measurement approach
            ObsidianTextSize measuredSize = obsidian_macos_textview_measure(ffiHandle, 10000.0);
            if (measuredSize.height > 0) {
                textHeight = static_cast<float>(measuredSize.height);
            }
        }
        
        auto* childNode = layout::ViewNode::createLeaf(textViewHandle, nullptr);
        childNode->getStyle().height = layout::LayoutValue::points(textHeight);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void VStack::addChild(VStack& vstack) {
    if (!pImpl || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    vstack.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // OWNERSHIP TRANSFER: Use the nested VStack's existing layoutNode (which has its children!)
    // This is inspired by React Native's Shadow Tree architecture
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(vstack.getLayoutNode());
        if (childLayoutNode) {
            // Transfer ownership: parent now owns the child's layoutNode
            vstack.releaseLayoutNodeOwnership();
            
            // Add to layout tree
            pImpl->layoutNode->addChild(childLayoutNode);
            pImpl->childLayoutNodes.push_back(childLayoutNode);
        }
    }
#endif
}

void VStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* hstackView = hstack.getNativeViewHandle();
    if (!hstackView) {
        return;
    }
    
    hstack.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, hstackView);
    pImpl->childViewHandles.push_back(hstackView);
    
    // OWNERSHIP TRANSFER: Use the HStack's existing layoutNode (which has its children!)
    // This is inspired by React Native's Shadow Tree architecture:
    // - HStack's layoutNode already contains its Link/Button children
    // - We adopt that layoutNode into VStack's layout tree
    // - HStack releases ownership so its destructor won't delete the node
    // - This ensures layout persists even when HStack object goes out of scope
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(hstack.getLayoutNode());
        if (childLayoutNode) {
            // Transfer ownership: parent now owns the child's layoutNode
            hstack.releaseLayoutNodeOwnership();
            
            // Add to layout tree
            pImpl->layoutNode->addChild(childLayoutNode);
            pImpl->childLayoutNodes.push_back(childLayoutNode);
        }
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), buttonView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(Link& link) {
    if (!pImpl || !pImpl->valid || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* linkView = link.getNativeViewHandle();
    if (!linkView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), linkView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), spacerView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(TextView& textView) {
    if (!pImpl || !pImpl->valid || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), textViewHandle);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, textViewHandle);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(VStack& vstack) {
    if (!pImpl || !pImpl->valid || !vstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::removeChild(HStack& hstack) {
    if (!pImpl || !pImpl->valid || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* hstackView = hstack.getNativeViewHandle();
    if (!hstackView) {
        return;
    }
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), hstackView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, hstackView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void VStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_vstack_remove_child_view(pImpl->vstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
#endif
}

void VStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add VStack to window
    obsidian_macos_vstack_add_to_window(pImpl->vstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Configure layoutNode with current settings
    if (pImpl->layoutNode) {
        pImpl->layoutNode->configureAsVStack(
            static_cast<float>(pImpl->spacing),
            static_cast<float>(pImpl->padding.top),
            static_cast<float>(pImpl->padding.bottom),
            static_cast<float>(pImpl->padding.leading),
            static_cast<float>(pImpl->padding.trailing)
        );
        
        // Get window dimensions from content view bounds
        float width = 800.0f;   // Default fallback
        float height = 600.0f;  // Default fallback
        
        double bx, by, bw, bh;
        obsidian_macos_view_get_bounds(contentView, &bx, &by, &bw, &bh);
        width = static_cast<float>(bw);
        height = static_cast<float>(bh);
        
        // Calculate and apply layout
        pImpl->layoutNode->layoutAndApply(width, height);
    }
#endif
}

void VStack::addToScreen(Screen& screen) {
    if (!pImpl) {
        return;
    }
    
    void* screenContentView = screen.getNativeHandle();
    if (screenContentView) {
        addToParentView(screenContentView);
    }
}

void VStack::addToParentView(void* parentView) {
    if (!pImpl || !parentView) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianVStackParams params;
        pImpl->vstackHandle = obsidian_macos_create_vstack(params);
        if (!pImpl->vstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* vstackView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    if (!vstackView) {
        return;
    }
    
    // Add VStack view to parent
    obsidian_macos_screen_add_subview(parentView, vstackView);
    pImpl->parentView = parentView;
    
    // Configure layoutNode and apply layout
    if (pImpl->layoutNode) {
        pImpl->layoutNode->configureAsVStack(
            static_cast<float>(pImpl->spacing),
            static_cast<float>(pImpl->padding.top),
            static_cast<float>(pImpl->padding.bottom),
            static_cast<float>(pImpl->padding.leading),
            static_cast<float>(pImpl->padding.trailing)
        );
        
        // Get parent view bounds for layout calculation
        double bx, by, bw, bh;
        obsidian_macos_view_get_bounds(parentView, &bx, &by, &bw, &bh);
        float width = static_cast<float>(bw);
        float height = static_cast<float>(bh);
        
        // Calculate and apply layout
        pImpl->layoutNode->layoutAndApply(width, height);
    }
#endif
}

void VStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    obsidian_macos_vstack_remove_from_parent(pImpl->vstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void VStack::updateLayout() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    // Recalculate layout with current parent bounds
    if (pImpl->layoutNode && pImpl->parentView) {
        double bx, by, bw, bh;
        obsidian_macos_view_get_bounds(pImpl->parentView, &bx, &by, &bw, &bh);
        float width = static_cast<float>(bw);
        float height = static_cast<float>(bh);
        
        pImpl->layoutNode->layoutAndApply(width, height);
    }
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

void* VStack::getLayoutNode() const {
    if (!pImpl) {
        return nullptr;
    }
    return pImpl->layoutNode;
}

void VStack::releaseLayoutNodeOwnership() {
    if (pImpl) {
        pImpl->ownsLayoutNode = false;
    }
}

VStack::VStack(VStack&&) noexcept = default;
VStack& VStack::operator=(VStack&&) noexcept = default;

} // namespace obsidian
