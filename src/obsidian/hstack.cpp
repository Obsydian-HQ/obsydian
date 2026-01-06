/**
 * Obsidian Public API - HStack Implementation
 * 
 * Uses the Layout Engine to calculate positions and sizes.
 * The C++ Layout Engine computes frames, then applies them to native views.
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

// Layout Engine
#include "core/layout/view_node.h"
#include "core/layout/engine.h"
#include "core/layout/manager.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_button.h"
#include "macos_window.h"
#include "macos_hstack.h"
#include "macos_screen_container.h"
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

class HStack::Impl {
public:
#ifdef __APPLE__
    ObsidianHStackHandle hstackHandle;
    std::vector<void*> childViewHandles;
    void* parentView;
#endif
    bool valid;
    double spacing;
    layout::Alignment alignment;
    Padding padding;
    
    // Layout Engine integration
    // IMPORTANT: layoutNode ownership model (inspired by React Native's Shadow Tree)
    // - When an HStack is created, it owns its layoutNode
    // - When an HStack is added as a child to another container, ownership transfers
    // - This ensures layoutNodes persist even when component objects go out of scope
    layout::ViewNode* layoutNode;
    std::vector<layout::LayoutNode*> childLayoutNodes;
    bool ownsLayoutNode;  // true if this HStack owns its layoutNode
    
    Impl()
#ifdef __APPLE__
        : hstackHandle(nullptr)
        , parentView(nullptr)
#endif
        , valid(false)
        , spacing(0.0)
        , alignment(layout::Alignment::Top)
        , padding{0.0, 0.0, 0.0, 0.0}
        , layoutNode(nullptr)
        , ownsLayoutNode(true)  // Initially owns its layoutNode
    {}
    
    ~Impl() {
        // Only delete layoutNode if we still own it
        if (ownsLayoutNode && layoutNode) {
            delete layoutNode;
        }
    }
};

HStack::HStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view
    ObsidianHStackParams params;
    pImpl->hstackHandle = obsidian_macos_create_hstack(params);
    if (pImpl->hstackHandle) {
        pImpl->valid = true;
        
        // Create LayoutNode for this HStack (Row direction)
        void* nativeView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
        pImpl->layoutNode = layout::ViewNode::createContainer(layout::FlexDirection::Row, nativeView);
        
        // Set up the frame setter callback for layout engine
        layout::ViewNode::setSetFrameCallback([](void* view, float x, float y, float w, float h) {
            obsidian_macos_view_set_frame(view, x, y, w, h);
        });
    }
#endif
}

HStack::~HStack() {
    if (pImpl && pImpl->valid) {
#ifdef __APPLE__
        // Release handle but do NOT remove from parent
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
            obsidian_macos_hstack_set_spacing(pImpl->hstackHandle, spacing);
            
            // Update layout node
            if (pImpl->layoutNode) {
                pImpl->layoutNode->getStyle().gap = static_cast<float>(spacing);
                pImpl->layoutNode->markDirty();
            }
#endif
        }
    }
}

double HStack::getSpacing() const {
    return pImpl ? pImpl->spacing : 0.0;
}

void HStack::setAlignment(layout::Alignment alignment) {
    if (pImpl) {
        pImpl->alignment = alignment;
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
            obsidian_macos_hstack_set_padding(pImpl->hstackHandle,
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

Padding HStack::getPadding() const {
    return pImpl ? pImpl->padding : Padding{0.0, 0.0, 0.0, 0.0};
}

void HStack::addChild(Button& button) {
    if (!pImpl || !button.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Create LayoutNode for button (leaf node with fixed width)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(buttonView, nullptr);
        childNode->getStyle().width = layout::LayoutValue::points(100.0f);  // Default button width
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void HStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) {
        return;
    }
    
    // Add to native view hierarchy
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
    
    // Create LayoutNode for spacer (uses flexGrow to fill available space)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createSpacer(spacerView);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void HStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
    
    // Create LayoutNode for link (leaf node with auto width)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(linkView, nullptr);
        childNode->getStyle().width = layout::LayoutValue::points(80.0f);  // Default link width
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void HStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
    
    // Create LayoutNode for text view (leaf node with flexible width)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(textViewHandle, nullptr);
        childNode->getStyle().flexGrow = 1.0f;  // Text views expand to fill
        childNode->getStyle().height = layout::LayoutValue::points(24.0f);
        pImpl->layoutNode->addChild(childNode);
        pImpl->childLayoutNodes.push_back(childNode);
    }
#endif
}

void HStack::addChild(VStack& vstack) {
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
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // OWNERSHIP TRANSFER: Use the VStack's existing layoutNode (which has its children!)
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(vstack.getLayoutNode());
        if (childLayoutNode) {
            // Transfer ownership
            vstack.releaseLayoutNodeOwnership();
            
            // Add to layout tree
            pImpl->layoutNode->addChild(childLayoutNode);
            pImpl->childLayoutNodes.push_back(childLayoutNode);
        }
    }
#endif
}

void HStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    void* nestedView = hstack.getNativeViewHandle();
    if (!nestedView) {
        return;
    }
    
    hstack.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_hstack_add_child_view(pImpl->hstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // OWNERSHIP TRANSFER: Use the nested HStack's existing layoutNode (which has its children!)
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(hstack.getLayoutNode());
        if (childLayoutNode) {
            // Transfer ownership
            hstack.releaseLayoutNodeOwnership();
            
            // Add to layout tree
            pImpl->layoutNode->addChild(childLayoutNode);
            pImpl->childLayoutNodes.push_back(childLayoutNode);
        }
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), buttonView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, buttonView);
        pImpl->childViewHandles.erase(it);
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), linkView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, linkView);
        pImpl->childViewHandles.erase(it);
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), spacerView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, spacerView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::removeChild(TextView& textView) {
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
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, textViewHandle);
        pImpl->childViewHandles.erase(it);
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
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
    
    auto it = std::find(pImpl->childViewHandles.begin(), pImpl->childViewHandles.end(), nestedView);
    if (it != pImpl->childViewHandles.end()) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, nestedView);
        pImpl->childViewHandles.erase(it);
    }
#endif
}

void HStack::clearChildren() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    for (void* childView : pImpl->childViewHandles) {
        obsidian_macos_hstack_remove_child_view(pImpl->hstackHandle, childView);
    }
    pImpl->childViewHandles.clear();
#endif
}

void HStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) {
        return;
    }
    
    // Add HStack to window
    obsidian_macos_hstack_add_to_window(pImpl->hstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Configure layoutNode and apply layout
    if (pImpl->layoutNode) {
        pImpl->layoutNode->configureAsHStack(
            static_cast<float>(pImpl->spacing),
            static_cast<float>(pImpl->padding.top),
            static_cast<float>(pImpl->padding.bottom),
            static_cast<float>(pImpl->padding.leading),
            static_cast<float>(pImpl->padding.trailing)
        );
        
        // Get window dimensions from content view bounds
        double bx, by, bw, bh;
        obsidian_macos_view_get_bounds(contentView, &bx, &by, &bw, &bh);
        float width = static_cast<float>(bw);
        float height = static_cast<float>(bh);
        
        // Calculate and apply layout
        pImpl->layoutNode->layoutAndApply(width, height);
    }
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
    if (!pImpl->valid) {
        ObsidianHStackParams params;
        pImpl->hstackHandle = obsidian_macos_create_hstack(params);
        if (!pImpl->hstackHandle) {
            return;
        }
        pImpl->valid = true;
    }
    
    void* hstackView = obsidian_macos_hstack_get_view(pImpl->hstackHandle);
    if (!hstackView) {
        return;
    }
    
    // Add HStack view to parent
    obsidian_macos_screen_add_subview(parentView, hstackView);
    pImpl->parentView = parentView;
    
    // Configure layoutNode and apply layout
    if (pImpl->layoutNode) {
        pImpl->layoutNode->configureAsHStack(
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

void HStack::removeFromParent() {
    if (!pImpl || !pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    obsidian_macos_hstack_remove_from_parent(pImpl->hstackHandle);
    pImpl->parentView = nullptr;
#endif
}

void HStack::updateLayout() {
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

void* HStack::getLayoutNode() const {
    if (!pImpl) {
        return nullptr;
    }
    return pImpl->layoutNode;
}

void HStack::releaseLayoutNodeOwnership() {
    if (pImpl) {
        pImpl->ownsLayoutNode = false;
    }
}

HStack::HStack(HStack&&) noexcept = default;
HStack& HStack::operator=(HStack&&) noexcept = default;

} // namespace obsidian
