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

/**
 * VStack Implementation Details
 * 
 * Architecture follows Yoga/React Native patterns:
 * 1. Layout tree (shadow tree) is separate from view hierarchy
 * 2. Layout nodes have clear ownership model
 * 3. Layout calculation is explicit, not implicit
 */
class VStack::Impl {
public:
#ifdef __APPLE__
    ObsidianVStackHandle vstackHandle = nullptr;
    std::vector<void*> childViewHandles;
    void* parentView = nullptr;
#endif
    bool valid = false;
    double spacing = 0.0;
    layout::Alignment alignment = layout::Alignment::Leading;
    Padding padding{0.0, 0.0, 0.0, 0.0};
    
    // Layout node ownership (React Native Shadow Tree pattern)
    layout::ViewNode* layoutNode = nullptr;
    std::vector<layout::LayoutNode*> childLayoutNodes;
    bool ownsLayoutNode = true;
    
    ~Impl() {
        if (ownsLayoutNode && layoutNode) {
            delete layoutNode;
        }
    }
    
    // ========================================================================
    // Helper Methods (Following Yoga's single-responsibility pattern)
    // ========================================================================
    
#ifdef __APPLE__
    /**
     * Ensure native handle is created and valid
     */
    bool ensureNativeHandle() {
        if (valid && vstackHandle) return true;
        
        ObsidianVStackParams params;
        vstackHandle = obsidian_macos_create_vstack(params);
        if (!vstackHandle) return false;
        
        valid = true;
        
        // Create layout node if needed
        if (!layoutNode) {
            void* nativeView = obsidian_macos_vstack_get_view(vstackHandle);
            layoutNode = layout::ViewNode::createContainer(layout::FlexDirection::Column, nativeView);
            obsidian_macos_vstack_set_layout_node(vstackHandle, layoutNode);
        }
        
        return true;
    }
    
    /**
     * Add a child's layout node to this container's layout tree
     * Transfers ownership from child to this parent
     */
    void adoptChildLayoutNode(layout::LayoutNode* childNode) {
        if (!layoutNode || !childNode) return;
        layoutNode->addChild(childNode);
        childLayoutNodes.push_back(childNode);
    }
    
    /**
     * Get bounds from a view, with fallback for undefined height
     */
    void getViewBounds(void* view, float& outWidth, float& outHeight) {
        double bx, by, bw, bh;
        obsidian_macos_view_get_bounds(view, &bx, &by, &bw, &bh);
        outWidth = static_cast<float>(bw);
        outHeight = static_cast<float>(bh);
        
        // Use large value when height is undefined (allows content-based sizing)
        if (outHeight <= 0) {
            outHeight = 10000.0f;
        }
    }
    
    /**
     * Configure layout node with current VStack settings
     */
    void configureLayoutNode() {
        if (!layoutNode) return;
        layoutNode->configureAsVStack(
            static_cast<float>(spacing),
            static_cast<float>(padding.top),
            static_cast<float>(padding.bottom),
            static_cast<float>(padding.leading),
            static_cast<float>(padding.trailing)
        );
    }
    
    /**
     * Find root layout node by traversing up the tree
     */
    layout::LayoutNode* findRootLayoutNode(layout::LayoutNode* node) {
        while (node && node->getParent()) {
            node = node->getParent();
        }
        return node;
    }
    
    /**
     * Recalculate layout from root and apply to views
     */
    void recalculateLayoutFromRoot(void* boundsView) {
        if (!layoutNode) return;
        
        layout::LayoutNode* rootNode = findRootLayoutNode(layoutNode);
        if (!rootNode || !rootNode->getNativeView()) return;
        
        float width, height;
        getViewBounds(boundsView, width, height);
        
        rootNode->calculateLayout(width, height);
        
        auto* viewNode = static_cast<layout::ViewNode*>(rootNode);
        if (viewNode) {
            viewNode->layoutAndApply(width, height);
        }
    }
    
    /**
     * Integrate this VStack's layout node into a parent VStack's layout tree
     * Returns true if integration happened
     */
    bool integrateIntoParentLayoutTree(void* parentViewHandle, bool& outOwnershipTransferred) {
        outOwnershipTransferred = false;
        
        ObsidianVStackHandle parentHandle = obsidian_macos_vstack_get_handle_from_view(parentViewHandle);
        if (!parentHandle) return false;
        
        void* parentLayoutNodePtr = obsidian_macos_vstack_get_layout_node(parentHandle);
        if (!parentLayoutNodePtr || !layoutNode) return false;
        
        auto* parentNode = static_cast<layout::LayoutNode*>(parentLayoutNodePtr);
        auto* childNode = static_cast<layout::LayoutNode*>(layoutNode);
        
        // Transfer ownership and integrate
        parentNode->addChild(childNode);
        parentNode->markDirty();
        outOwnershipTransferred = true;
        
        return true;
    }
#endif
};

VStack::VStack() : pImpl(std::make_unique<Impl>()) {
#ifdef __APPLE__
    // Eagerly initialize native view and layout node
    if (pImpl->ensureNativeHandle()) {
        // Set up the frame setter callback for layout engine (once per process)
        static bool callbackSet = false;
        if (!callbackSet) {
            layout::ViewNode::setSetFrameCallback([](void* view, float x, float y, float w, float h) {
                obsidian_macos_view_set_frame(view, x, y, w, h);
            });
            callbackSet = true;
        }
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
    if (!pImpl || !button.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* buttonView = button.getNativeViewHandle();
    if (!buttonView) return;
    
    button.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, buttonView);
    pImpl->childViewHandles.push_back(buttonView);
    
    // Create layout node for button (leaf with fixed height)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(buttonView, nullptr);
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->adoptChildLayoutNode(childNode);
    }
#endif
}

void VStack::addChild(Spacer& spacer) {
    if (!pImpl || !spacer.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* spacerView = spacer.getNativeViewHandle();
    if (!spacerView) return;
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, spacerView);
    pImpl->childViewHandles.push_back(spacerView);
    
    // Create layout node for spacer (uses flexGrow)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createSpacer(spacerView);
        pImpl->adoptChildLayoutNode(childNode);
    }
#endif
}

void VStack::addChild(Link& link) {
    if (!pImpl || !link.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* linkView = link.getNativeViewHandle();
    if (!linkView) return;
    
    link.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, linkView);
    pImpl->childViewHandles.push_back(linkView);
    
    // Create layout node for link (leaf with fixed height)
    if (pImpl->layoutNode) {
        auto* childNode = layout::ViewNode::createLeaf(linkView, nullptr);
        childNode->getStyle().height = layout::LayoutValue::points(30.0f);
        pImpl->adoptChildLayoutNode(childNode);
    }
#endif
}

void VStack::addChild(TextView& textView) {
    if (!pImpl || !textView.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* textViewHandle = textView.getNativeViewHandle();
    if (!textViewHandle) return;
    
    textView.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, textViewHandle);
    pImpl->childViewHandles.push_back(textViewHandle);
    
    // Create layout node with measured text height (React Native TextLayoutManager pattern)
    if (pImpl->layoutNode) {
        void* ffiHandle = textView.getNativeHandle();
        float textHeight = 24.0f;  // Default fallback
        
        if (ffiHandle) {
            ObsidianTextSize measuredSize = obsidian_macos_textview_measure(ffiHandle, 10000.0);
            if (measuredSize.height > 0) {
                textHeight = static_cast<float>(measuredSize.height);
            }
        }
        
        auto* childNode = layout::ViewNode::createLeaf(textViewHandle, nullptr);
        childNode->getStyle().height = layout::LayoutValue::points(textHeight);
        pImpl->adoptChildLayoutNode(childNode);
    }
#endif
}

void VStack::addChild(VStack& vstack) {
    if (!pImpl || !vstack.isValid()) return;
    
#ifdef __APPLE__
    void* nestedView = vstack.getNativeViewHandle();
    if (!nestedView) return;
    
    vstack.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, nestedView);
    pImpl->childViewHandles.push_back(nestedView);
    
    // Transfer layout node ownership (React Native Shadow Tree pattern)
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(vstack.getLayoutNode());
        if (childLayoutNode) {
            vstack.releaseLayoutNodeOwnership();
            pImpl->adoptChildLayoutNode(childLayoutNode);
        }
    }
    
    // Recalculate layout if already attached
    if (pImpl->parentView) {
        updateLayout();
    }
#endif
}

void VStack::addChild(HStack& hstack) {
    if (!pImpl || !hstack.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* hstackView = hstack.getNativeViewHandle();
    if (!hstackView) return;
    
    hstack.removeFromParent();
    
    // Add to native view hierarchy
    obsidian_macos_vstack_add_child_view(pImpl->vstackHandle, hstackView);
    pImpl->childViewHandles.push_back(hstackView);
    
    // Transfer layout node ownership (React Native Shadow Tree pattern)
    if (pImpl->layoutNode) {
        auto* childLayoutNode = static_cast<layout::LayoutNode*>(hstack.getLayoutNode());
        if (childLayoutNode) {
            hstack.releaseLayoutNodeOwnership();
            pImpl->adoptChildLayoutNode(childLayoutNode);
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
    
    // Also clear layoutNode children to prevent stale references
    if (pImpl->layoutNode) {
        pImpl->layoutNode->removeAllChildren();
        pImpl->childLayoutNodes.clear();
    }
#endif
}

void VStack::addToWindow(Window& window) {
    if (!pImpl || !window.isValid()) return;
    
#ifdef __APPLE__
    if (!pImpl->ensureNativeHandle()) return;
    
    void* windowHandle = window.getNativeHandle();
    void* contentView = obsidian_macos_window_get_content_view(windowHandle);
    if (!contentView) return;
    
    // Add to window
    obsidian_macos_vstack_add_to_window(pImpl->vstackHandle, windowHandle);
    pImpl->parentView = contentView;
    
    // Configure and apply layout
    pImpl->configureLayoutNode();
    
    float width, height;
    pImpl->getViewBounds(contentView, width, height);
    
    if (pImpl->layoutNode) {
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
    if (!pImpl || !parentView) return;
    
#ifdef __APPLE__
    // Ensure native handle exists
    if (!pImpl->ensureNativeHandle()) return;
    
    void* vstackView = obsidian_macos_vstack_get_view(pImpl->vstackHandle);
    if (!vstackView) return;
    
    // Try to integrate into parent's layout tree (if parent is a VStack)
    bool ownershipTransferred = false;
    if (pImpl->integrateIntoParentLayoutTree(parentView, ownershipTransferred)) {
        if (ownershipTransferred) {
            releaseLayoutNodeOwnership();
        }
        pImpl->recalculateLayoutFromRoot(parentView);
    }
    
    // Update wrapper's layoutNode reference
    if (pImpl->layoutNode && pImpl->vstackHandle) {
        obsidian_macos_vstack_set_layout_node(pImpl->vstackHandle, pImpl->layoutNode);
    }
    
    // Add to native view hierarchy
    obsidian_macos_screen_add_subview(parentView, vstackView);
    pImpl->parentView = parentView;
    
    // Configure and apply layout
    pImpl->configureLayoutNode();
    
    float width, height;
    pImpl->getViewBounds(parentView, width, height);
    
    if (pImpl->layoutNode) {
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
    if (!pImpl || !pImpl->valid) return;
    
#ifdef __APPLE__
    if (pImpl->layoutNode && pImpl->parentView) {
        float width, height;
        pImpl->getViewBounds(pImpl->parentView, width, height);
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
