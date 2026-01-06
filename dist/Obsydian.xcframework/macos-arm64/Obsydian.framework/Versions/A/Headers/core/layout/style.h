/**
 * Obsidian Layout Engine - Style Properties
 * 
 * Defines style properties for layout nodes, inspired by Yoga/Flexbox.
 * These properties control how nodes are sized and positioned.
 */

#pragma once

#include <cstdint>
#include <limits>

namespace obsidian::layout {

/**
 * Layout value unit
 */
enum class Unit {
    Undefined,  // No value specified (auto)
    Point,      // Absolute pixels
    Percent     // Percentage of parent
};

/**
 * Layout value - can be undefined, points, or percent
 */
struct LayoutValue {
    float value = 0.0f;
    Unit unit = Unit::Undefined;
    
    static LayoutValue undefined() { return {0.0f, Unit::Undefined}; }
    static LayoutValue points(float v) { return {v, Unit::Point}; }
    static LayoutValue percent(float v) { return {v, Unit::Percent}; }
    static LayoutValue auto_() { return undefined(); }
    
    bool isUndefined() const { return unit == Unit::Undefined; }
    bool isDefined() const { return unit != Unit::Undefined; }
    
    // Resolve to points given parent size
    float resolve(float parentSize) const {
        if (unit == Unit::Point) return value;
        if (unit == Unit::Percent) return value * parentSize / 100.0f;
        return 0.0f;  // Undefined resolves to 0
    }
};

/**
 * Flex direction - how children are laid out
 */
enum class FlexDirection {
    Column,         // Top to bottom (default)
    ColumnReverse,  // Bottom to top
    Row,            // Left to right
    RowReverse      // Right to left
};

/**
 * Justify content - main axis alignment
 */
enum class JustifyContent {
    FlexStart,      // Pack at start (default)
    FlexEnd,        // Pack at end
    Center,         // Center on main axis
    SpaceBetween,   // Even spacing, no edge space
    SpaceAround,    // Even spacing, half edge space
    SpaceEvenly     // Even spacing, full edge space
};

/**
 * Align items - cross axis alignment
 */
enum class AlignItems {
    FlexStart,      // Align at start
    FlexEnd,        // Align at end
    Center,         // Center on cross axis
    Stretch         // Stretch to fill (default)
};

/**
 * Align self - override parent's alignItems for this child
 */
enum class AlignSelf {
    Auto,           // Use parent's alignItems (default)
    FlexStart,
    FlexEnd,
    Center,
    Stretch
};

/**
 * Position type
 */
enum class PositionType {
    Relative,       // Normal flow (default)
    Absolute        // Removed from flow, positioned relative to parent
};

/**
 * Edge indices for padding/margin/border
 */
enum class Edge {
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
    Start = 4,      // Leading (respects RTL)
    End = 5,        // Trailing (respects RTL)
    Horizontal = 6, // Left + Right
    Vertical = 7,   // Top + Bottom
    All = 8
};

/**
 * Style properties for a layout node
 * 
 * Modeled after Yoga's style system but simplified for our needs.
 */
struct Style {
    // Flex container properties
    FlexDirection flexDirection = FlexDirection::Column;
    JustifyContent justifyContent = JustifyContent::FlexStart;
    AlignItems alignItems = AlignItems::Stretch;
    
    // Flex item properties
    AlignSelf alignSelf = AlignSelf::Auto;
    float flexGrow = 0.0f;      // How much this item grows to fill space
    float flexShrink = 1.0f;    // How much this item shrinks when needed
    LayoutValue flexBasis = LayoutValue::auto_();  // Base size before flex
    
    // Position
    PositionType positionType = PositionType::Relative;
    LayoutValue position[4] = {};  // left, top, right, bottom
    
    // Dimensions
    LayoutValue width = LayoutValue::auto_();
    LayoutValue height = LayoutValue::auto_();
    LayoutValue minWidth = LayoutValue::undefined();
    LayoutValue minHeight = LayoutValue::undefined();
    LayoutValue maxWidth = LayoutValue::undefined();
    LayoutValue maxHeight = LayoutValue::undefined();
    
    // Spacing
    LayoutValue padding[4] = {};   // left, top, right, bottom
    LayoutValue margin[4] = {};    // left, top, right, bottom
    float gap = 0.0f;              // Space between children
    
    // Aspect ratio (width/height, 0 = none)
    float aspectRatio = 0.0f;
    
    // Helper methods
    void setMargin(Edge edge, LayoutValue value);
    void setPadding(Edge edge, LayoutValue value);
    void setPosition(Edge edge, LayoutValue value);
    
    LayoutValue getMargin(Edge edge) const;
    LayoutValue getPadding(Edge edge) const;
    LayoutValue getPosition(Edge edge) const;
};

} // namespace obsidian::layout
