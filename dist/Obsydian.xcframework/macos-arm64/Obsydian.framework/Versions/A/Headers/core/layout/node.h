/**
 * Obsidian Layout Engine - Layout Node
 * 
 * A node in the layout tree. Stores:
 * - Style properties (input)
 * - Computed layout results (output)
 * - Children and parent references
 * 
 * Inspired by Yoga's Node but simplified.
 */

#pragma once

#include "style.h"
#include <vector>
#include <memory>
#include <functional>

namespace obsidian::layout {

/**
 * Computed layout results
 * These are the output of the layout algorithm
 */
struct LayoutResult {
    float left = 0.0f;      // X position relative to parent
    float top = 0.0f;       // Y position relative to parent
    float width = 0.0f;     // Computed width
    float height = 0.0f;    // Computed height
    
    // Padding (resolved to pixels)
    float paddingLeft = 0.0f;
    float paddingTop = 0.0f;
    float paddingRight = 0.0f;
    float paddingBottom = 0.0f;
    
    // Content area (excludes padding)
    float contentWidth() const { return width - paddingLeft - paddingRight; }
    float contentHeight() const { return height - paddingTop - paddingBottom; }
    
    // Convenience
    float right() const { return left + width; }
    float bottom() const { return top + height; }
};

/**
 * Measure function type
 * Used for leaf nodes that have intrinsic size (e.g., text)
 * 
 * @param widthMode How to interpret width constraint
 * @param width Width constraint (if applicable)
 * @param heightMode How to interpret height constraint
 * @param height Height constraint (if applicable)
 * @return The measured size
 */
enum class MeasureMode {
    Undefined,      // No constraint
    Exactly,        // Must be exactly this size
    AtMost          // Can be up to this size
};

struct Size {
    float width = 0.0f;
    float height = 0.0f;
};

using MeasureFunc = std::function<Size(
    float width, MeasureMode widthMode,
    float height, MeasureMode heightMode
)>;

/**
 * Layout Node
 * 
 * Represents a single element in the layout tree.
 * Can be a container (with children) or a leaf (with measure function).
 */
class LayoutNode {
public:
    LayoutNode();
    ~LayoutNode();
    
    // Style
    Style& getStyle() { return style_; }
    const Style& getStyle() const { return style_; }
    
    // Layout results (read-only from outside)
    const LayoutResult& getLayout() const { return layout_; }
    
    // Children
    void addChild(LayoutNode* child);
    void insertChild(LayoutNode* child, size_t index);
    void removeChild(LayoutNode* child);
    void removeAllChildren();
    
    size_t getChildCount() const { return children_.size(); }
    LayoutNode* getChild(size_t index) const;
    const std::vector<LayoutNode*>& getChildren() const { return children_; }
    
    // Parent
    LayoutNode* getParent() const { return parent_; }
    
    // Measure function (for leaf nodes like text)
    void setMeasureFunc(MeasureFunc func);
    bool hasMeasureFunc() const { return measureFunc_ != nullptr; }
    
    // Native view association (for applying layout)
    void setNativeView(void* view) { nativeView_ = view; }
    void* getNativeView() const { return nativeView_; }
    
    // Mark dirty (needs layout recalculation)
    void markDirty();
    bool isDirty() const { return isDirty_; }
    
    // Calculate layout (main entry point)
    // Call on root node with available space
    void calculateLayout(float availableWidth, float availableHeight);
    
    // Debug
    void printTree(int indent = 0) const;

private:
    friend class LayoutEngine;
    
    // Internal layout computation
    Size measure(float width, MeasureMode widthMode, 
                 float height, MeasureMode heightMode);
    
    // Apply computed layout to this node
    void setLayout(const LayoutResult& result) { layout_ = result; }
    LayoutResult& getMutableLayout() { return layout_; }
    
    Style style_;
    LayoutResult layout_;
    
    LayoutNode* parent_ = nullptr;
    std::vector<LayoutNode*> children_;
    
    MeasureFunc measureFunc_;
    void* nativeView_ = nullptr;
    
    bool isDirty_ = true;
    
    // Non-copyable
    LayoutNode(const LayoutNode&) = delete;
    LayoutNode& operator=(const LayoutNode&) = delete;
};

} // namespace obsidian::layout
