# SplitView API

## Overview

The SplitView component provides a split view layout using `NSSplitView` with collapse/expand, resizing, and content customization capabilities. Unlike Sidebar, SplitView is a general-purpose split view that doesn't use the native sidebar styling.

## Features

- **Split view layout**: Two-pane layout with resizable divider
- **Collapse/expand**: Programmatic and user-controlled collapse of primary pane
- **Width constraints**: Minimum and maximum width for primary pane
- **Position control**: Primary pane can be on leading or trailing side
- **Content customization**: Any Obsidian component can be used as pane content

## Class: SplitView

### Construction

```cpp
SplitView();
```

Creates a new SplitView instance. Call `create()` to initialize it.

### Creation

```cpp
bool create(SplitPosition position = SplitPosition::Leading);
```

Creates the SplitView with the given split position.

**Parameters:**
- `position`: Split position (Leading or Trailing). Default is Leading.

**Returns:** `true` if SplitView was created successfully, `false` otherwise.

### Split Position

```cpp
enum class SplitPosition {
    Leading,   // Primary pane on left side (LTR)
    Trailing   // Primary pane on right side (LTR)
};
```

### Primary Pane Width Management

```cpp
void setPrimaryPaneWidth(double width);
double getPrimaryPaneWidth() const;
```

Set/get the current primary pane width in points. The width will be clamped to min/max values.

```cpp
void setMinimumPrimaryPaneWidth(double width);
void setMaximumPrimaryPaneWidth(double width);
```

Set minimum and maximum primary pane width in points.

**Defaults:**
- Minimum: 150.0 points
- Maximum: 400.0 points
- Initial width: 200.0 points

### Collapse/Expand Management

```cpp
void collapsePrimaryPane();
void expandPrimaryPane();
void togglePrimaryPane();
bool isPrimaryPaneCollapsed() const;
```

Control the primary pane collapse state.

- `collapsePrimaryPane()`: Collapses the primary pane
- `expandPrimaryPane()`: Expands the primary pane (restore from collapsed state)
- `togglePrimaryPane()`: Toggles primary pane collapse/expand state
- `isPrimaryPaneCollapsed()`: Returns `true` if primary pane is currently collapsed

### Content Management

#### Primary Pane Content

```cpp
void setPrimaryPaneContent(VStack& vstack);
void setPrimaryPaneContent(HStack& hstack);
void setPrimaryPaneContent(List& list);
void setPrimaryPaneContent(Button& button);
void setPrimaryPaneContent(ScrollView& scrollView);
```

Sets the primary pane content view. The provided component will be added as the primary pane's view.

#### Secondary Pane Content

```cpp
void setSecondaryPaneContent(VStack& vstack);
void setSecondaryPaneContent(HStack& hstack);
void setSecondaryPaneContent(List& list);
void setSecondaryPaneContent(Button& button);
void setSecondaryPaneContent(ScrollView& scrollView);
```

Sets the secondary pane content view. The provided component will be added as the secondary pane's view.

### Window Integration

```cpp
void addToWindow(Window& window);
```

Adds SplitView to a window's content view. If the SplitView hasn't been created yet, it will be auto-created with the default Leading position.

```cpp
void removeFromParent();
```

Removes SplitView from its parent view.

### State

```cpp
bool isValid() const;
```

Checks if SplitView is valid and ready to use.

**Returns:** `true` if valid, `false` otherwise.

### Callbacks

```cpp
void setOnPrimaryPaneToggle(std::function<void(bool collapsed)> callback);
```

Sets a callback function that will be called when the primary pane is collapsed or expanded.

**Parameters:**
- `callback`: Function that takes a bool parameter (`true` if collapsed, `false` if expanded)

## Usage Example

```cpp
#include <obsidian/obsidian.h>

int main() {
    obsidian::App app;
    app.initialize();
    
    // Create window
    obsidian::Window window;
    window.create(1200, 800, "My App");
    
    // Create SplitView with primary pane on leading side
    obsidian::SplitView splitView;
    splitView.create(obsidian::SplitPosition::Leading);
    
    // Set width constraints
    splitView.setMinimumPrimaryPaneWidth(150.0);
    splitView.setMaximumPrimaryPaneWidth(400.0);
    splitView.setPrimaryPaneWidth(250.0);
    
    // Create primary pane content
    obsidian::VStack primaryContent;
    primaryContent.setSpacing(10.0);
    primaryContent.setPadding(obsidian::Padding::all(20.0));
    
    obsidian::Button primaryButton;
    primaryButton.create("Primary Pane", 0, 0, 200, 40);
    primaryContent.addChild(primaryButton);
    primaryContent.updateLayout();
    
    splitView.setPrimaryPaneContent(primaryContent);
    
    // Create secondary pane content
    obsidian::VStack secondaryContent;
    secondaryContent.setSpacing(10.0);
    secondaryContent.setPadding(obsidian::Padding::all(20.0));
    
    obsidian::Button secondaryButton;
    secondaryButton.create("Secondary Pane", 0, 0, 200, 40);
    secondaryContent.addChild(secondaryButton);
    secondaryContent.updateLayout();
    
    splitView.setSecondaryPaneContent(secondaryContent);
    
    // Optional: Set callback for pane toggle events
    splitView.setOnPrimaryPaneToggle([](bool collapsed) {
        if (collapsed) {
            std::cout << "Primary pane collapsed\n";
        } else {
            std::cout << "Primary pane expanded\n";
        }
    });
    
    // Add to window
    splitView.addToWindow(window);
    
    // Show and run
    window.show();
    app.run({});
    
    return 0;
}
```

## SplitView vs Sidebar

| Feature | SplitView | Sidebar |
|---------|-----------|---------|
| Native sidebar styling | No | Yes (material background) |
| Native collapse button | No | Yes (via toolbar) |
| Position control | Yes (Leading/Trailing) | No (always leading) |
| Use case | General split view | Native macOS sidebar |
| Window setup required | No | Yes (toolbar setup) |

## Implementation Details

The SplitView uses:
- `NSSplitView` for the split view management
- Two `NSSplitViewItem` instances for the primary and secondary panes
- Native views wrapping the Obsidian components

The SplitView is created with default values:
- Primary pane width: 200.0 points
- Minimum width: 150.0 points
- Maximum width: 400.0 points
- Position: Leading (left side in LTR)

## Notes

- SplitView is macOS-only (uses `#ifdef __APPLE__`)
- SplitView is non-copyable but movable
- The SplitView must be created before setting content (or use `addToWindow` which auto-creates)
- Content components must be valid (created) before being set
- The divider between panes is resizable by the user (within min/max constraints)
- Primary pane collapse removes it from the layout, giving full space to secondary pane
