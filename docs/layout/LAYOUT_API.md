# Layout System API Reference

**Version**: 1.0  
**Date**: December 2025  
**Platform**: macOS (primary implementation)

## Overview

The Obsidian layout system provides declarative, constraint-based layout capabilities similar to SwiftUI's VStack/HStack/ZStack and Flutter's Row/Column/Stack. The layout system uses NSLayoutConstraint on macOS for native, performant layout.

## Core Concepts

### Layout Containers

Layout containers (`VStack`, `HStack`, `ZStack`) arrange child views according to their configuration. All containers support:
- **Alignment**: Controls how children are positioned within the container
- **Padding**: Optional spacing around the container's edges (VStack and HStack only)
- **Dynamic updates**: Layout automatically updates when children are added/removed
- **Window resize**: Layout automatically adapts when window size changes (handled by NSLayoutConstraint)

### Alignment

Alignment controls how child views are positioned within their container. Available alignments are defined in `layout::Alignment`:

```cpp
enum class Alignment {
    // Horizontal
    Leading,    // Left (LTR) / Right (RTL)
    Center,     // Center horizontally
    Trailing,   // Right (LTR) / Left (RTL)
    
    // Vertical
    Top,        // Top edge
    Middle,     // Center vertically
    Bottom,     // Bottom edge
    
    // Combined (for ZStack and complex layouts)
    TopLeading,
    TopCenter,
    TopTrailing,
    MiddleLeading,
    MiddleCenter,
    MiddleTrailing,
    BottomLeading,
    BottomCenter,
    BottomTrailing
};
```

### Padding

Padding is specified using the `Padding` struct, which provides spacing around container edges:

```cpp
struct Padding {
    double top;
    double bottom;
    double leading;
    double trailing;
    
    // Factory methods
    static Padding all(double value);
    static Padding symmetric(double vertical, double horizontal);
    static Padding only(double top, double bottom, double leading, double trailing);
};
```

## API Reference

### VStack (Vertical Stack)

Arranges child views vertically with configurable spacing, alignment, and padding.

#### Constructor

```cpp
VStack();
```

Creates a new VStack with default settings:
- Spacing: 0.0
- Alignment: `layout::Alignment::Leading`
- Padding: 0.0 on all sides

#### Configuration Methods

##### `void setSpacing(double spacing)`

Sets the spacing between child views in pixels.

**Parameters**:
- `spacing`: Space between children (must be >= 0.0)

**Example**:
```cpp
VStack vstack;
vstack.setSpacing(10.0);  // 10 pixels between children
```

##### `double getSpacing() const`

Returns the current spacing between child views.

##### `void setAlignment(layout::Alignment alignment)`

Sets the horizontal alignment of child views within the VStack.

**Valid alignments for VStack**:
- `layout::Alignment::Leading` (default)
- `layout::Alignment::Center`
- `layout::Alignment::Trailing`
- Combined alignments (e.g., `TopLeading`, `TopCenter`, etc.) affect container positioning in parent

**Example**:
```cpp
VStack vstack;
vstack.setAlignment(layout::Alignment::Center);  // Center children horizontally
```

##### `layout::Alignment getAlignment() const`

Returns the current alignment.

##### `void setPadding(const Padding& padding)`

Sets padding around the VStack.

**Example**:
```cpp
VStack vstack;
vstack.setPadding(Padding::all(20.0));  // 20 pixels on all sides
vstack.setPadding(Padding::symmetric(10.0, 20.0));  // 10px top/bottom, 20px leading/trailing
```

##### `Padding getPadding() const`

Returns the current padding.

#### Children Management

##### `void addChild(Button& button)`

Adds a button as a child to the VStack. The button is automatically positioned based on the VStack's configuration.

**Note**: A view can only have one parent. Adding a view that already has a parent will remove it from its previous parent first.

**Example**:
```cpp
VStack vstack;
Button button1;
button1.create("Button 1", 0, 0, 100, 30);
vstack.addChild(button1);
```

##### `void addChild(Spacer& spacer)`

Adds a spacer as a child to the VStack. Spacers expand to fill available space.

**Example**:
```cpp
VStack vstack;
Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt
vstack.addChild(spacer);
```

##### `void removeChild(Button& button)`

Removes a button child from the VStack. Layout is automatically updated.

##### `void removeChild(Spacer& spacer)`

Removes a spacer child from the VStack. Layout is automatically updated.

##### `void clearChildren()`

Removes all children from the VStack. Layout is automatically updated.

#### Window Integration

##### `void addToWindow(Window& window)`

Adds the VStack to a window's content view. The VStack is positioned according to its alignment settings.

**Example**:
```cpp
Window window;
window.create(800, 600, "My Window");
VStack vstack;
// ... configure vstack and add children ...
vstack.addToWindow(window);
window.show();
```

##### `void removeFromParent()`

Removes the VStack from its parent view (window or container).

#### Layout Control

##### `void updateLayout()`

Manually triggers a layout update. This is automatically called when:
- Children are added or removed
- Spacing, alignment, or padding is changed
- The stack is added to a window

You typically don't need to call this manually, but it's available if needed.

##### `bool isValid() const`

Returns `true` if the VStack is valid and can be used. A VStack becomes valid when it's created (implicitly when the first child is added or when `addToWindow` is called).

---

### HStack (Horizontal Stack)

Arranges child views horizontally with configurable spacing, alignment, and padding. The API is identical to VStack, but the alignment controls vertical positioning instead of horizontal.

#### API Summary

All methods have the same signatures as VStack:

- `HStack()` - Constructor
- `void setSpacing(double spacing)` - Set horizontal spacing
- `double getSpacing() const` - Get horizontal spacing
- `void setAlignment(layout::Alignment alignment)` - Set vertical alignment (Top, Middle, Bottom)
- `layout::Alignment getAlignment() const` - Get alignment
- `void setPadding(const Padding& padding)` - Set padding
- `Padding getPadding() const` - Get padding
- `void addChild(Button& button)` - Add button child
- `void addChild(Spacer& spacer)` - Add spacer child
- `void removeChild(Button& button)` - Remove button child
- `void removeChild(Spacer& spacer)` - Remove spacer child
- `void clearChildren()` - Remove all children
- `void addToWindow(Window& window)` - Add to window
- `void removeFromParent()` - Remove from parent
- `void updateLayout()` - Update layout manually
- `bool isValid() const` - Check if valid

**Valid alignments for HStack**:
- `layout::Alignment::Top`
- `layout::Alignment::Middle` (default)
- `layout::Alignment::Bottom`
- Combined alignments affect container positioning

**Example**:
```cpp
HStack hstack;
hstack.setSpacing(15.0);  // 15 pixels between children horizontally
hstack.setAlignment(layout::Alignment::Top);  // Align children at top
hstack.setPadding(Padding::all(10.0));

Button button1, button2, button3;
button1.create("Button 1", 0, 0, 80, 30);
button2.create("Button 2", 0, 0, 80, 30);
button3.create("Button 3", 0, 0, 80, 30);

hstack.addChild(button1);
hstack.addChild(button2);
hstack.addChild(button3);

Window window;
window.create(800, 600, "HStack Demo");
hstack.addToWindow(window);
window.show();
```

---

### ZStack (Overlay Stack)

Arranges child views on top of each other with alignment-based positioning. The last child added is on top (highest z-order).

#### API Summary

- `ZStack()` - Constructor
- `void setAlignment(layout::Alignment alignment)` - Set alignment for positioning (default: `Center`)
- `layout::Alignment getAlignment() const` - Get alignment
- `void addChild(Button& button)` - Add button child
- `void removeChild(Button& button)` - Remove button child
- `void clearChildren()` - Remove all children
- `void addToWindow(Window& window)` - Add to window (fills window)
- `void removeFromParent()` - Remove from parent
- `void updateLayout()` - Update layout manually
- `bool isValid() const` - Check if valid

**Note**: ZStack does not support spacing or padding. All children are positioned at the same alignment point (e.g., all centered, all at TopLeading, etc.).

**Z-order**: Children are stacked in the order they were added. The last child added appears on top.

**Example**:
```cpp
ZStack zstack;
zstack.setAlignment(layout::Alignment::Center);  // Center all children

Button button1, button2, button3;
button1.create("Bottom", 0, 0, 150, 50);  // Largest, will be behind
button2.create("Middle", 0, 0, 120, 40);  // Medium, in middle
button3.create("Top", 0, 0, 100, 30);     // Smallest, on top

zstack.addChild(button1);  // Added first, bottom layer
zstack.addChild(button2);  // Added second, middle layer
zstack.addChild(button3);  // Added last, top layer (visible)

Window window;
window.create(800, 600, "ZStack Demo");
zstack.addToWindow(window);
window.show();
```

---

### Spacer

A flexible spacing component that expands to fill available space in layout containers (VStack, HStack). Supports optional minimum and maximum size constraints.

#### Constructor

```cpp
Spacer();
```

Creates a new Spacer with no constraints.

#### Configuration Methods

##### `bool create(double minWidth = 0.0, double minHeight = 0.0, double maxWidth = 0.0, double maxHeight = 0.0)`

Creates the spacer with optional size constraints.

**Parameters**:
- `minWidth`: Minimum width in pixels (0.0 for no minimum)
- `minHeight`: Minimum height in pixels (0.0 for no minimum)
- `maxWidth`: Maximum width in pixels (0.0 for no maximum)
- `maxHeight`: Maximum height in pixels (0.0 for no maximum)

**Returns**: `true` if spacer was created successfully

**Example**:
```cpp
Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt, no other constraints
```

##### `void setMinWidth(double minWidth)`

Sets the minimum width constraint. Use 0.0 to remove the constraint.

##### `double getMinWidth() const`

Returns the minimum width constraint.

##### `void setMinHeight(double minHeight)`

Sets the minimum height constraint. Use 0.0 to remove the constraint.

##### `double getMinHeight() const`

Returns the minimum height constraint.

##### `void setMaxWidth(double maxWidth)`

Sets the maximum width constraint. Use 0.0 to remove the constraint.

##### `double getMaxWidth() const`

Returns the maximum width constraint.

##### `void setMaxHeight(double maxHeight)`

Sets the maximum height constraint. Use 0.0 to remove the constraint.

##### `double getMaxHeight() const`

Returns the maximum height constraint.

#### Utility Methods

##### `bool isValid() const`

Returns `true` if the Spacer is valid (has been created).

##### `void* getNativeViewHandle() const`

Returns the native view handle (for internal use by layout system).

**Example**:
```cpp
VStack vstack;
vstack.setSpacing(10.0);

Button button1, button2;
button1.create("Top", 0, 0, 100, 30);
button2.create("Bottom", 0, 0, 100, 30);

Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt

vstack.addChild(button1);
vstack.addChild(spacer);  // Spacer expands to fill space between buttons
vstack.addChild(button2);

Window window;
window.create(800, 600, "Spacer Demo");
vstack.addToWindow(window);
window.show();
```

---

## Common Patterns

### Basic Vertical Layout

```cpp
VStack vstack;
vstack.setSpacing(10.0);
vstack.setAlignment(layout::Alignment::Center);
vstack.setPadding(Padding::all(20.0));

Button button1, button2, button3;
button1.create("Button 1", 0, 0, 100, 30);
button2.create("Button 2", 0, 0, 100, 30);
button3.create("Button 3", 0, 0, 100, 30);

vstack.addChild(button1);
vstack.addChild(button2);
vstack.addChild(button3);

Window window;
window.create(800, 600, "My App");
vstack.addToWindow(window);
window.show();
```

### Basic Horizontal Layout

```cpp
HStack hstack;
hstack.setSpacing(15.0);
hstack.setAlignment(layout::Alignment::Middle);

Button button1, button2, button3;
button1.create("Left", 0, 0, 80, 30);
button2.create("Center", 0, 0, 80, 30);
button3.create("Right", 0, 0, 80, 30);

hstack.addChild(button1);
hstack.addChild(button2);
hstack.addChild(button3);

Window window;
window.create(800, 600, "My App");
hstack.addToWindow(window);
window.show();
```

### Using Spacers for Flexible Layouts

```cpp
VStack vstack;
vstack.setSpacing(10.0);

Button topButton, bottomButton;
topButton.create("Top", 0, 0, 100, 30);
bottomButton.create("Bottom", 0, 0, 100, 30);

Spacer spacer;
spacer.create(0.0, 20.0, 0.0, 0.0);  // min height 20pt

vstack.addChild(topButton);
vstack.addChild(spacer);  // Expands to fill space
vstack.addChild(bottomButton);

Window window;
window.create(800, 600, "Spacer Demo");
vstack.addToWindow(window);
window.show();
```

### Overlay Layout with ZStack

```cpp
ZStack zstack;
zstack.setAlignment(layout::Alignment::Center);

Button background, foreground;
background.create("Background", 0, 0, 200, 100);
foreground.create("Foreground", 0, 0, 150, 50);

zstack.addChild(background);  // Bottom layer
zstack.addChild(foreground);  // Top layer

Window window;
window.create(800, 600, "ZStack Demo");
zstack.addToWindow(window);
window.show();
```

## Performance Considerations

### Automatic Layout Updates

The layout system automatically updates when:
- Children are added or removed
- Spacing, alignment, or padding is changed
- Window size changes (handled by NSLayoutConstraint)

You typically don't need to call `updateLayout()` manually.

### Layout Performance

- Layout calculations use NSLayoutConstraint (native macOS Auto Layout)
- Layout updates are fast (< 10ms for typical layouts)
- Constraint generation is optimized for common cases
- Window resize handling is automatic and efficient

### Best Practices

1. **Set configuration before adding children**: Set spacing, alignment, and padding before adding children for optimal performance.

2. **Batch updates**: If you're adding/removing multiple children, do it in a single batch rather than one at a time (the system handles this automatically).

3. **Use appropriate alignment**: Use simple alignments (`Leading`, `Center`, `Trailing`, `Top`, `Middle`, `Bottom`) when possible for better performance.

4. **Avoid excessive nesting**: While nested stacks are supported, keep nesting depth reasonable (< 5 levels) for best performance.

## Thread Safety

- **All layout operations must be on the main thread** (macOS requirement)
- Layout APIs are not thread-safe
- Window creation and modification must be on the main thread

## Platform Support

- **macOS**: Fully supported (uses NSLayoutConstraint)
- **iOS**: Planned (will use NSLayoutConstraint)
- **Android**: Planned (will use ConstraintLayout)
- **Windows**: Planned (will use XAML layout)
- **Linux**: Planned (will use GTK layout)

---

**See Also**:
- [Layout Examples](LAYOUT_EXAMPLES.md) - Comprehensive usage examples
- [Layout System Plan](LAYOUT_SYSTEM_PLAN.md) - Implementation details
- [Layout Checkpoints](LAYOUT_SYSTEM_CHECKPOINTS.md) - Development progress
