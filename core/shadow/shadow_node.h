/**
 * Obsidian Shadow Tree - Shadow Node
 * 
 * Architecture inspired by React Native's Fabric renderer.
 * 
 * Key principles:
 * - ShadowNodes are SEPARATE from native views
 * - ShadowNodes are OWNED by the ShadowTree (not by UI components)
 * - Layout is computed on ShadowNodes, then applied to native views
 * - ShadowNodes persist for the lifetime of the visible UI
 * 
 * This solves the problem where VStack/HStack local variables go out of
 * scope but the UI should remain visible and properly laid out.
 */

#pragma once

#include "../layout/style.h"
#include "../layout/node.h"
#include <memory>
#include <string>
#include <cstdint>

namespace obsidian::shadow {

// Unique identifier for shadow nodes
using ShadowTag = uint64_t;

// Forward declarations
class ShadowTree;

/**
 * Layout metrics computed by the layout engine.
 * This is passed to native views via updateLayoutMetrics.
 */
struct LayoutMetrics {
    float x = 0.0f;         // X position relative to parent
    float y = 0.0f;         // Y position relative to parent
    float width = 0.0f;     // Computed width
    float height = 0.0f;    // Computed height
    
    // Padding (resolved to pixels)
    float paddingLeft = 0.0f;
    float paddingTop = 0.0f;
    float paddingRight = 0.0f;
    float paddingBottom = 0.0f;
    
    // Content area
    float contentWidth() const { return width - paddingLeft - paddingRight; }
    float contentHeight() const { return height - paddingTop - paddingBottom; }
    
    bool operator==(const LayoutMetrics& other) const {
        return x == other.x && y == other.y &&
               width == other.width && height == other.height &&
               paddingLeft == other.paddingLeft && paddingTop == other.paddingTop &&
               paddingRight == other.paddingRight && paddingBottom == other.paddingBottom;
    }
    
    bool operator!=(const LayoutMetrics& other) const {
        return !(*this == other);
    }
};

/**
 * Component types for shadow nodes
 */
enum class ComponentType {
    Root,       // Root container (window/screen content)
    VStack,     // Vertical stack
    HStack,     // Horizontal stack
    ZStack,     // Overlay stack
    Spacer,     // Flexible spacer
    TextView,   // Text display
    Button,     // Clickable button
    Link,       // Navigation link
    TextField,  // Text input
    ScrollView, // Scrollable container
    List,       // List container
    Table,      // Table container
    Custom      // User-defined component
};

/**
 * Shadow Node
 * 
 * Represents a UI element in the shadow tree.
 * Stores:
 * - Component type and tag
 * - Style properties (layout input)
 * - Layout metrics (layout output)
 * - Children references
 * - Native view handle (for applying layout)
 */
class ShadowNode {
public:
    ShadowNode(ShadowTag tag, ComponentType type);
    ~ShadowNode();
    
    // Identity
    ShadowTag getTag() const { return tag_; }
    ComponentType getComponentType() const { return componentType_; }
    
    // Style (layout input)
    layout::Style& getStyle() { return layoutNode_->getStyle(); }
    const layout::Style& getStyle() const { return layoutNode_->getStyle(); }
    
    // Layout metrics (computed output)
    const LayoutMetrics& getLayoutMetrics() const { return layoutMetrics_; }
    void setLayoutMetrics(const LayoutMetrics& metrics);
    
    // Children
    void addChild(ShadowNode* child);
    void insertChild(ShadowNode* child, size_t index);
    void removeChild(ShadowNode* child);
    void removeAllChildren();
    
    size_t getChildCount() const;
    ShadowNode* getChild(size_t index) const;
    const std::vector<ShadowNode*>& getChildren() const { return children_; }
    
    // Parent
    ShadowNode* getParent() const { return parent_; }
    
    // Native view association
    void setNativeView(void* view) { nativeView_ = view; layoutNode_->setNativeView(view); }
    void* getNativeView() const { return nativeView_; }
    
    // Layout node (internal - for layout engine)
    layout::LayoutNode* getLayoutNode() { return layoutNode_.get(); }
    const layout::LayoutNode* getLayoutNode() const { return layoutNode_.get(); }
    
    // Dirty tracking
    void markDirty();
    bool isDirty() const { return isDirty_; }
    void clearDirty() { isDirty_ = false; }
    
    // Update layout metrics from computed layout
    void updateFromLayoutResult();

private:
    friend class ShadowTree;
    
    ShadowTag tag_;
    ComponentType componentType_;
    
    // Layout node for computation
    std::unique_ptr<layout::LayoutNode> layoutNode_;
    
    // Computed layout metrics (result)
    LayoutMetrics layoutMetrics_;
    LayoutMetrics previousLayoutMetrics_;  // For diff detection
    
    // Tree structure
    ShadowNode* parent_ = nullptr;
    std::vector<ShadowNode*> children_;
    
    // Native view (for applying layout)
    void* nativeView_ = nullptr;
    
    // State
    bool isDirty_ = true;
    bool layoutMetricsChanged_ = false;
    
    // Non-copyable
    ShadowNode(const ShadowNode&) = delete;
    ShadowNode& operator=(const ShadowNode&) = delete;
};

} // namespace obsidian::shadow
