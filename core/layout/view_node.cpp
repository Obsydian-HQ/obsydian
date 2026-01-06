/**
 * Obsidian Layout Engine - View Node Implementation
 */

#include "view_node.h"

namespace obsidian::layout {

ViewNode::SetFrameCallback ViewNode::setFrameCallback_ = nullptr;

ViewNode* ViewNode::createContainer(FlexDirection direction, void* nativeView) {
    auto* node = new ViewNode();
    node->getStyle().flexDirection = direction;
    node->setNativeView(nativeView);
    return node;
}

ViewNode* ViewNode::createSpacer(void* nativeView) {
    auto* node = new ViewNode();
    node->getStyle().flexGrow = 1.0f;  // Spacers fill available space
    node->setNativeView(nativeView);
    return node;
}

ViewNode* ViewNode::createLeaf(void* nativeView, MeasureFunc measureFunc) {
    auto* node = new ViewNode();
    node->setNativeView(nativeView);
    if (measureFunc) {
        node->setMeasureFunc(std::move(measureFunc));
    }
    return node;
}

void ViewNode::configureAsVStack(float spacing, float paddingTop, float paddingBottom,
                                  float paddingLeading, float paddingTrailing) {
    auto& style = getStyle();
    style.flexDirection = FlexDirection::Column;
    style.gap = spacing;
    style.padding[0] = LayoutValue::points(paddingLeading);
    style.padding[1] = LayoutValue::points(paddingTop);
    style.padding[2] = LayoutValue::points(paddingTrailing);
    style.padding[3] = LayoutValue::points(paddingBottom);
    markDirty();
}

void ViewNode::configureAsHStack(float spacing, float paddingTop, float paddingBottom,
                                  float paddingLeading, float paddingTrailing) {
    auto& style = getStyle();
    style.flexDirection = FlexDirection::Row;
    style.gap = spacing;
    style.padding[0] = LayoutValue::points(paddingLeading);
    style.padding[1] = LayoutValue::points(paddingTop);
    style.padding[2] = LayoutValue::points(paddingTrailing);
    style.padding[3] = LayoutValue::points(paddingBottom);
    markDirty();
}

void ViewNode::configureAsSpacer(float minWidth, float minHeight) {
    auto& style = getStyle();
    style.flexGrow = 1.0f;
    if (minWidth > 0) {
        style.minWidth = LayoutValue::points(minWidth);
    }
    if (minHeight > 0) {
        style.minHeight = LayoutValue::points(minHeight);
    }
    markDirty();
}

void ViewNode::layoutAndApply(float width, float height) {
    // Calculate layout
    calculateLayout(width, height);
    
    // Apply to native views
    if (setFrameCallback_) {
        LayoutEngine::applyLayout(this, setFrameCallback_);
    }
}

void ViewNode::setSetFrameCallback(SetFrameCallback callback) {
    setFrameCallback_ = callback;
}

} // namespace obsidian::layout
