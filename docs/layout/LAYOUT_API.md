# Layout Engine API

## Overview

The Obsidian Layout Engine is a flexbox-based layout system inspired by Yoga. It computes positions and sizes for all nodes in a layout tree based on their style properties. The engine is platform-agnostic and provides a bridge to native views through the LayoutManager.

## Core Components

### LayoutNode

A node in the layout tree that stores style properties (input) and computed layout results (output).

#### Construction

```cpp
LayoutNode();
```

Creates a new layout node. Nodes are non-copyable but can be moved.

#### Style Access

```cpp
Style& getStyle();
const Style& getStyle() const;
```

Get the style object for this node. Modify style properties to control layout behavior.

#### Layout Results

```cpp
const LayoutResult& getLayout() const;
```

Get the computed layout results. This is read-only from outside the layout engine.

#### Children Management

```cpp
void addChild(LayoutNode* child);
void insertChild(LayoutNode* child, size_t index);
void removeChild(LayoutNode* child);
void removeAllChildren();
size_t getChildCount() const;
LayoutNode* getChild(size_t index) const;
const std::vector<LayoutNode*>& getChildren() const;
```

Manage child nodes. The layout tree is built by adding children to parent nodes.

#### Parent Access

```cpp
LayoutNode* getParent() const;
```

Get the parent node, or `nullptr` if this is the root.

#### Measure Function

```cpp
void setMeasureFunc(MeasureFunc func);
bool hasMeasureFunc() const;
```

Set a measure function for leaf nodes that have intrinsic size (e.g., text). The measure function signature is:

```cpp
using MeasureFunc = std::function<Size(
    float width, MeasureMode widthMode,
    float height, MeasureMode heightMode
)>;
```

#### Native View Association

```cpp
void setNativeView(void* view);
void* getNativeView() const;
```

Associate a native view with this node. The layout engine will apply computed frames to this view.

#### Layout Calculation

```cpp
void calculateLayout(float availableWidth, float availableHeight);
```

Calculate layout for this node tree starting from this node. This is the main entry point for layout computation.

#### Dirty State

```cpp
void markDirty();
bool isDirty() const;
```

Mark a node as needing layout recalculation. The layout engine uses this for optimization.

### LayoutResult

Computed layout results for a node.

```cpp
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
    float contentWidth() const;
    float contentHeight() const;
    
    // Convenience
    float right() const;
    float bottom() const;
};
```

### Style

Style properties that control how nodes are sized and positioned.

#### Flex Container Properties

```cpp
FlexDirection flexDirection = FlexDirection::Column;
JustifyContent justifyContent = JustifyContent::FlexStart;
AlignItems alignItems = AlignItems::Stretch;
```

- `flexDirection`: How children are laid out (Column, ColumnReverse, Row, RowReverse)
- `justifyContent`: Main axis alignment (FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly)
- `alignItems`: Cross axis alignment (FlexStart, FlexEnd, Center, Stretch)

#### Flex Item Properties

```cpp
AlignSelf alignSelf = AlignSelf::Auto;
float flexGrow = 0.0f;      // How much this item grows to fill space
float flexShrink = 1.0f;    // How much this item shrinks when needed
LayoutValue flexBasis = LayoutValue::auto_();  // Base size before flex
```

#### Position

```cpp
PositionType positionType = PositionType::Relative;
LayoutValue position[4] = {};  // left, top, right, bottom
```

- `positionType`: Relative (normal flow) or Absolute (removed from flow, positioned relative to parent)

#### Dimensions

```cpp
LayoutValue width = LayoutValue::auto_();
LayoutValue height = LayoutValue::auto_();
LayoutValue minWidth = LayoutValue::undefined();
LayoutValue minHeight = LayoutValue::undefined();
LayoutValue maxWidth = LayoutValue::undefined();
LayoutValue maxHeight = LayoutValue::undefined();
```

#### Spacing

```cpp
LayoutValue padding[4] = {};   // left, top, right, bottom
LayoutValue margin[4] = {};    // left, top, right, bottom
float gap = 0.0f;              // Space between children
```

#### Helper Methods

```cpp
void setMargin(Edge edge, LayoutValue value);
void setPadding(Edge edge, LayoutValue value);
void setPosition(Edge edge, LayoutValue value);

LayoutValue getMargin(Edge edge) const;
LayoutValue getPadding(Edge edge) const;
LayoutValue getPosition(Edge edge) const;
```

### LayoutValue

A value that can be undefined, points, or percent.

```cpp
enum class Unit {
    Undefined,  // No value specified (auto)
    Point,      // Absolute pixels
    Percent     // Percentage of parent
};

struct LayoutValue {
    float value = 0.0f;
    Unit unit = Unit::Undefined;
    
    static LayoutValue undefined();
    static LayoutValue points(float v);
    static LayoutValue percent(float v);
    static LayoutValue auto_();
    
    bool isUndefined() const;
    bool isDefined() const;
    
    // Resolve to points given parent size
    float resolve(float parentSize) const;
};
```

### MeasureMode

How to interpret size constraints.

```cpp
enum class MeasureMode {
    Undefined,      // No constraint
    Exactly,        // Must be exactly this size
    AtMost          // Can be up to this size
};
```

### LayoutEngine

The core layout algorithm that computes positions and sizes.

#### Static Methods

```cpp
static void calculateLayout(LayoutNode* root, 
                            float availableWidth, 
                            float availableHeight);
```

Calculate layout for a node tree. This is the main entry point for layout computation.

```cpp
using SetFrameFunc = void(*)(void* nativeView, 
                              float x, float y, 
                              float width, float height);

static void applyLayout(LayoutNode* root, SetFrameFunc setFrameFunc);
```

Apply computed layout to native views. After `calculateLayout`, call this to apply the results to the native view hierarchy.

### LayoutManager

Bridges the Layout Engine to native views. Provides platform-agnostic interface for triggering layout calculation and applying results.

#### Singleton Access

```cpp
static LayoutManager& getInstance();
```

Get the singleton instance of the layout manager.

#### Platform Setup

```cpp
using NativeSetFrameFunc = void(*)(void* nativeView, 
                                    float x, float y, 
                                    float width, float height);

void setNativeSetFrameFunc(NativeSetFrameFunc func);
```

Set the platform-specific frame setter function. Must be called before any layout application.

#### Layout Calculation and Application

```cpp
void calculateAndApply(LayoutNode* root, float width, float height);
```

Calculate layout for a node tree and apply to native views in one call.

```cpp
void applyToNativeViews(LayoutNode* root);
```

Apply layout to native views (after `calculateLayout` has been called).

### ViewNode

A LayoutNode that is associated with a native view. Provides convenience methods for UI component integration.

#### Static Factory Methods

```cpp
static ViewNode* createContainer(FlexDirection direction, void* nativeView);
static ViewNode* createSpacer(void* nativeView);
static ViewNode* createLeaf(void* nativeView, MeasureFunc measureFunc);
```

#### Configuration Methods

```cpp
void configureAsVStack(float spacing, float paddingTop, float paddingBottom, 
                       float paddingLeading, float paddingTrailing);
void configureAsHStack(float spacing, float paddingTop, float paddingBottom,
                       float paddingLeading, float paddingTrailing);
void configureAsSpacer(float minWidth = 0, float minHeight = 0);
```

#### Layout and Apply

```cpp
void layoutAndApply(float width, float height);
```

Calculate layout and apply to native views. Call this on the root node.

#### Frame Setter Callback

```cpp
using SetFrameCallback = void(*)(void* view, float x, float y, float w, float h);
static void setSetFrameCallback(SetFrameCallback callback);
```

Set a global callback for when frame needs to be set on native view.

## Usage Example

```cpp
#include "core/layout/node.h"
#include "core/layout/manager.h"

using namespace obsidian::layout;

// Create root node
LayoutNode root;
root.getStyle().width = LayoutValue::points(800.0f);
root.getStyle().height = LayoutValue::points(600.0f);
root.getStyle().flexDirection = FlexDirection::Column;

// Create child node
LayoutNode* child = new LayoutNode();
child->getStyle().width = LayoutValue::points(200.0f);
child->getStyle().height = LayoutValue::points(100.0f);
root.addChild(child);

// Set up platform-specific frame setter
LayoutManager& manager = LayoutManager::getInstance();
manager.setNativeSetFrameFunc([](void* view, float x, float y, float w, float h) {
    // Platform-specific code to set frame
});

// Calculate and apply layout
manager.calculateAndApply(&root, 800.0f, 600.0f);
```

## Enumerations

### FlexDirection

```cpp
enum class FlexDirection {
    Column,         // Top to bottom (default)
    ColumnReverse,  // Bottom to top
    Row,            // Left to right
    RowReverse      // Right to left
};
```

### JustifyContent

```cpp
enum class JustifyContent {
    FlexStart,      // Pack at start (default)
    FlexEnd,        // Pack at end
    Center,         // Center on main axis
    SpaceBetween,   // Even spacing, no edge space
    SpaceAround,    // Even spacing, half edge space
    SpaceEvenly     // Even spacing, full edge space
};
```

### AlignItems

```cpp
enum class AlignItems {
    FlexStart,      // Align at start
    FlexEnd,        // Align at end
    Center,         // Center on cross axis
    Stretch         // Stretch to fill (default)
};
```

### AlignSelf

```cpp
enum class AlignSelf {
    Auto,           // Use parent's alignItems (default)
    FlexStart,
    FlexEnd,
    Center,
    Stretch
};
```

### PositionType

```cpp
enum class PositionType {
    Relative,       // Normal flow (default)
    Absolute        // Removed from flow, positioned relative to parent
};
```

### Edge

```cpp
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
```

## Notes

- The layout engine is inspired by Yoga but simplified for Obsidian's needs
- Layout calculation is deterministic and synchronous
- Nodes are non-copyable but movable
- The layout engine does not manage memory for nodes - you must manage node lifetimes
- Native views must be associated with nodes before applying layout
- The LayoutManager singleton must be configured with a platform-specific frame setter before use
