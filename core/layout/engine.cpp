/**
 * Obsidian Layout Engine - Core Algorithm Implementation
 * 
 * A simplified flexbox implementation inspired by Yoga.
 * Handles Column (VStack) and Row (HStack) layouts.
 */

#include "engine.h"
#include <algorithm>
#include <cmath>

namespace obsidian::layout {

// Helper to check if a value is defined
static bool isDefined(float value) {
    return !std::isnan(value) && value != 0.0f;
}

// Helper to get padding for a node
static void getPadding(const Style& style, float parentWidth, float parentHeight,
                       float& left, float& top, float& right, float& bottom) {
    left = style.padding[0].resolve(parentWidth);
    top = style.padding[1].resolve(parentHeight);
    right = style.padding[2].resolve(parentWidth);
    bottom = style.padding[3].resolve(parentHeight);
}

void LayoutEngine::calculateLayout(LayoutNode* root,
                                    float availableWidth,
                                    float availableHeight) {
    if (!root) return;
    
    // Start layout from root with given constraints
    layoutNode(root, availableWidth, MeasureMode::Exactly,
               availableHeight, MeasureMode::Exactly);
}

void LayoutEngine::layoutNode(LayoutNode* node,
                               float availableWidth, MeasureMode widthMode,
                               float availableHeight, MeasureMode heightMode) {
    if (!node) return;
    
    const Style& style = node->getStyle();
    LayoutResult& layout = node->getMutableLayout();
    
    // 1. Resolve width
    float resolvedWidth = 0.0f;
    if (style.width.isDefined()) {
        resolvedWidth = style.width.resolve(availableWidth);
    } else if (widthMode == MeasureMode::Exactly) {
        resolvedWidth = availableWidth;
    } else if (widthMode == MeasureMode::AtMost) {
        resolvedWidth = availableWidth;  // Start with max, may shrink
    }
    
    // Apply min/max constraints
    if (style.minWidth.isDefined()) {
        resolvedWidth = std::max(resolvedWidth, style.minWidth.resolve(availableWidth));
    }
    if (style.maxWidth.isDefined()) {
        resolvedWidth = std::min(resolvedWidth, style.maxWidth.resolve(availableWidth));
    }
    
    // 2. Resolve height
    float resolvedHeight = 0.0f;
    if (style.height.isDefined()) {
        resolvedHeight = style.height.resolve(availableHeight);
    } else if (heightMode == MeasureMode::Exactly) {
        resolvedHeight = availableHeight;
    } else if (heightMode == MeasureMode::AtMost) {
        resolvedHeight = availableHeight;  // Start with max, may shrink
    }
    
    // Apply min/max constraints
    if (style.minHeight.isDefined()) {
        resolvedHeight = std::max(resolvedHeight, style.minHeight.resolve(availableHeight));
    }
    if (style.maxHeight.isDefined()) {
        resolvedHeight = std::min(resolvedHeight, style.maxHeight.resolve(availableHeight));
    }
    
    // Store dimensions
    layout.width = resolvedWidth;
    layout.height = resolvedHeight;
    
    // 3. Resolve padding
    getPadding(style, resolvedWidth, resolvedHeight,
               layout.paddingLeft, layout.paddingTop,
               layout.paddingRight, layout.paddingBottom);
    
    // 4. Layout children if this is a container
    if (node->getChildCount() > 0) {
        layoutFlexContainer(node, resolvedWidth, MeasureMode::Exactly,
                           resolvedHeight, MeasureMode::Exactly);
    } else if (node->hasMeasureFunc()) {
        // Leaf node with measure function - measure it
        Size measured = node->measure(resolvedWidth, widthMode,
                                       resolvedHeight, heightMode);
        layout.width = measured.width;
        layout.height = measured.height;
    }
    
    // 5. Layout absolute positioned children
    layoutAbsoluteChildren(node);
}

void LayoutEngine::layoutFlexContainer(LayoutNode* node,
                                        float availableWidth, MeasureMode widthMode,
                                        float availableHeight, MeasureMode heightMode) {
    const Style& style = node->getStyle();
    LayoutResult& layout = node->getMutableLayout();
    
    bool isColumn = (style.flexDirection == FlexDirection::Column ||
                     style.flexDirection == FlexDirection::ColumnReverse);
    bool isReverse = (style.flexDirection == FlexDirection::ColumnReverse ||
                      style.flexDirection == FlexDirection::RowReverse);
    
    // Content area (excluding padding)
    // CRITICAL FIX: Following Yoga's BoundAxis.h approach - content dimensions must never be negative.
    // Yoga uses maxOrDefined(value, paddingAndBorderForAxis) to ensure this.
    // When padding exceeds available space, content should be 0, not negative.
    float contentWidth = std::max(0.0f, layout.width - layout.paddingLeft - layout.paddingRight);
    float contentHeight = std::max(0.0f, layout.height - layout.paddingTop - layout.paddingBottom);
    
    // Main axis is column (vertical) or row (horizontal)
    float mainAxisSize = isColumn ? contentHeight : contentWidth;
    float crossAxisSize = isColumn ? contentWidth : contentHeight;
    
    // Determine if cross axis needs to be calculated from children
    // (This is the KEY fix - like Yoga's SizingMode::MaxContent/FitContent)
    bool crossAxisFromChildren = (crossAxisSize <= 0);
    
    // Collect children that are in normal flow
    std::vector<LayoutNode*> flowChildren;
    for (auto* child : node->getChildren()) {
        if (child->getStyle().positionType == PositionType::Relative) {
            flowChildren.push_back(child);
        }
    }
    
    if (flowChildren.empty()) return;
    
    // Calculate total gap space
    float totalGap = style.gap * (flowChildren.size() - 1);
    
    // ==========================================================================
    // FIRST PASS: Measure children and calculate sizes (like Yoga's STEP 3 & 6)
    // ==========================================================================
    float totalFlexGrow = 0.0f;
    float totalFixedSize = 0.0f;
    float maxChildCrossSize = 0.0f;  // Track max cross dimension from children
    
    // Store measured sizes for second pass
    struct ChildMeasurement {
        float mainSize;
        float crossSize;
    };
    std::vector<ChildMeasurement> childMeasurements(flowChildren.size());
    
    for (size_t i = 0; i < flowChildren.size(); ++i) {
        auto* child = flowChildren[i];
        const Style& childStyle = child->getStyle();
        
        // Get child's base size on main axis
        float childMainSize = 0.0f;
        float childCrossSize = 0.0f;
        
        if (isColumn) {
            // Main axis is height, cross axis is width
            if (childStyle.height.isDefined()) {
                childMainSize = childStyle.height.resolve(contentHeight);
            }
            if (childStyle.width.isDefined()) {
                childCrossSize = childStyle.width.resolve(contentWidth);
            }
        } else {
            // Main axis is width, cross axis is height
            if (childStyle.width.isDefined()) {
                childMainSize = childStyle.width.resolve(contentWidth);
            }
            if (childStyle.height.isDefined()) {
                childCrossSize = childStyle.height.resolve(contentHeight);
            }
        }
        
        // If child has measure function, measure it
        if (child->hasMeasureFunc()) {
            Size measured = child->measure(
                contentWidth, MeasureMode::AtMost,
                contentHeight, MeasureMode::AtMost
            );
            if (childMainSize == 0.0f) {
                childMainSize = isColumn ? measured.height : measured.width;
            }
            if (childCrossSize == 0.0f) {
                childCrossSize = isColumn ? measured.width : measured.height;
            }
        }
        
        // Store measurements
        childMeasurements[i].mainSize = childMainSize;
        childMeasurements[i].crossSize = childCrossSize;
        
        // Track max cross size (like Yoga's justifyMainAxis)
        if (childCrossSize > maxChildCrossSize) {
            maxChildCrossSize = childCrossSize;
        }
        
        // Accumulate for flex calculation
        totalFlexGrow += childStyle.flexGrow;
        totalFixedSize += childMainSize;
    }
    
    // ==========================================================================
    // KEY FIX: If cross axis size is undefined, use max child cross size
    // (This is what Yoga does in STEP 9)
    // ==========================================================================
    if (crossAxisFromChildren && maxChildCrossSize > 0) {
        crossAxisSize = maxChildCrossSize;
        
        // Update layout dimensions
        if (isColumn) {
            layout.width = crossAxisSize + layout.paddingLeft + layout.paddingRight;
            contentWidth = crossAxisSize;
        } else {
            layout.height = crossAxisSize + layout.paddingTop + layout.paddingBottom;
            contentHeight = crossAxisSize;
        }
    }
    
    // Calculate remaining space on main axis
    float remainingSpace = mainAxisSize - totalFixedSize - totalGap;
    float flexGrowUnit = (totalFlexGrow > 0 && remainingSpace > 0) 
                         ? remainingSpace / totalFlexGrow 
                         : 0.0f;
    
    // ==========================================================================
    // SECOND PASS: Position children (like Yoga's STEP 7)
    // ==========================================================================
    float mainOffset = isColumn ? layout.paddingTop : layout.paddingLeft;
    
    // Handle justifyContent start offset
    if (totalFlexGrow == 0 && remainingSpace > 0) {
        switch (style.justifyContent) {
            case JustifyContent::FlexEnd:
                mainOffset += remainingSpace;
                break;
            case JustifyContent::Center:
                mainOffset += remainingSpace / 2.0f;
                break;
            case JustifyContent::SpaceBetween:
                // No initial offset, space distributed between
                break;
            case JustifyContent::SpaceAround:
                mainOffset += remainingSpace / (flowChildren.size() * 2);
                break;
            case JustifyContent::SpaceEvenly:
                mainOffset += remainingSpace / (flowChildren.size() + 1);
                break;
            default:
                break;
        }
    }
    
    // Calculate inter-item spacing for justify modes
    float interItemSpace = 0.0f;
    if (totalFlexGrow == 0 && remainingSpace > 0 && flowChildren.size() > 1) {
        switch (style.justifyContent) {
            case JustifyContent::SpaceBetween:
                interItemSpace = remainingSpace / (flowChildren.size() - 1);
                break;
            case JustifyContent::SpaceAround:
                interItemSpace = remainingSpace / flowChildren.size();
                break;
            case JustifyContent::SpaceEvenly:
                interItemSpace = remainingSpace / (flowChildren.size() + 1);
                break;
            default:
                break;
        }
    }
    
    // Reverse order if needed
    if (isReverse) {
        std::reverse(flowChildren.begin(), flowChildren.end());
        std::reverse(childMeasurements.begin(), childMeasurements.end());
    }
    
    for (size_t i = 0; i < flowChildren.size(); ++i) {
        auto* child = flowChildren[i];
        const Style& childStyle = child->getStyle();
        LayoutResult& childLayout = child->getMutableLayout();
        
        float childMainSize = childMeasurements[i].mainSize;
        float childCrossSize = childMeasurements[i].crossSize;
        
        // Add flex grow space
        if (childStyle.flexGrow > 0 && flexGrowUnit > 0) {
            childMainSize += childStyle.flexGrow * flexGrowUnit;
        }
        
        // Handle alignItems/alignSelf for cross axis
        AlignItems align = style.alignItems;
        if (childStyle.alignSelf != AlignSelf::Auto) {
            switch (childStyle.alignSelf) {
                case AlignSelf::FlexStart: align = AlignItems::FlexStart; break;
                case AlignSelf::FlexEnd: align = AlignItems::FlexEnd; break;
                case AlignSelf::Center: align = AlignItems::Center; break;
                case AlignSelf::Stretch: align = AlignItems::Stretch; break;
                default: break;
            }
        }
        
        // Determine final cross size
        float finalCrossSize = childCrossSize;
        if (align == AlignItems::Stretch && childCrossSize == 0) {
            // Stretch to fill container cross axis
            finalCrossSize = crossAxisSize;
        } else if (finalCrossSize == 0) {
            // If no explicit size, use natural size or stretch
            finalCrossSize = crossAxisSize;
        }
        
        // Calculate cross axis offset
        float crossOffset = isColumn ? layout.paddingLeft : layout.paddingTop;
        switch (align) {
            case AlignItems::FlexEnd:
                crossOffset += crossAxisSize - finalCrossSize;
                break;
            case AlignItems::Center:
                crossOffset += (crossAxisSize - finalCrossSize) / 2.0f;
                break;
            default:
                break;
        }
        
        // Set child position and size
        if (isColumn) {
            childLayout.left = crossOffset;
            childLayout.top = mainOffset;
            childLayout.width = finalCrossSize;
            childLayout.height = childMainSize;
        } else {
            childLayout.left = mainOffset;
            childLayout.top = crossOffset;
            childLayout.width = childMainSize;
            childLayout.height = finalCrossSize;
        }
        
        // Recursively layout child's children
        if (child->getChildCount() > 0) {
            float childContentWidth = childLayout.width;
            float childContentHeight = childLayout.height;
            layoutFlexContainer(child, childContentWidth, MeasureMode::Exactly,
                               childContentHeight, MeasureMode::Exactly);
        }
        
        // Advance main offset
        mainOffset += childMainSize + style.gap;
        
        // Add justify spacing
        if (i < flowChildren.size() - 1) {
            mainOffset += interItemSpace;
        }
    }
}

void LayoutEngine::layoutAbsoluteChildren(LayoutNode* node) {
    const LayoutResult& layout = node->getLayout();
    
    for (auto* child : node->getChildren()) {
        if (child->getStyle().positionType != PositionType::Absolute) {
            continue;
        }
        
        const Style& childStyle = child->getStyle();
        LayoutResult& childLayout = child->getMutableLayout();
        
        // Calculate size
        float width = childStyle.width.resolve(layout.width);
        float height = childStyle.height.resolve(layout.height);
        
        // Calculate position
        float left = 0.0f;
        float top = 0.0f;
        
        if (childStyle.position[0].isDefined()) {  // left
            left = childStyle.position[0].resolve(layout.width);
        } else if (childStyle.position[2].isDefined()) {  // right
            left = layout.width - width - childStyle.position[2].resolve(layout.width);
        }
        
        if (childStyle.position[1].isDefined()) {  // top
            top = childStyle.position[1].resolve(layout.height);
        } else if (childStyle.position[3].isDefined()) {  // bottom
            top = layout.height - height - childStyle.position[3].resolve(layout.height);
        }
        
        childLayout.left = left;
        childLayout.top = top;
        childLayout.width = width;
        childLayout.height = height;
        
        // Recursively layout absolute child's children
        if (child->getChildCount() > 0) {
            layoutFlexContainer(child, width, MeasureMode::Exactly,
                               height, MeasureMode::Exactly);
        }
    }
}

void LayoutEngine::applyLayout(LayoutNode* root, SetFrameFunc setFrameFunc) {
    if (!root || !setFrameFunc) return;
    
    applyLayoutRecursive(root, setFrameFunc, 0.0f, 0.0f);
}

void LayoutEngine::applyLayoutRecursive(LayoutNode* node,
                                         SetFrameFunc setFrameFunc,
                                         float parentAbsX,
                                         float parentAbsY) {
    const LayoutResult& layout = node->getLayout();
    
    // Calculate absolute position
    float absX = parentAbsX + layout.left;
    float absY = parentAbsY + layout.top;
    
    // Apply to native view if present
    if (node->getNativeView()) {
        setFrameFunc(node->getNativeView(), layout.left, layout.top,
                    layout.width, layout.height);
    }
    
    // Recurse to children
    for (auto* child : node->getChildren()) {
        applyLayoutRecursive(child, setFrameFunc, absX, absY);
    }
}

} // namespace obsidian::layout
