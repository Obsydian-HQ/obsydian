# SplitView API Documentation

## Overview

The `SplitView` component provides a split view layout using `NSSplitView` with collapse/expand, resizing, and content customization capabilities. It enables you to create applications with a collapsible primary pane and a secondary pane.

## Key Features

- **Split View Layout**: Uses `NSSplitView` for split pane layout
- **Collapsible Primary Pane**: Collapse/expand primary pane programmatically or via user interaction
- **Resizable Divider**: Users can drag the divider to resize the primary pane
- **Width Constraints**: Set minimum and maximum primary pane widths
- **Content Flexibility**: Supports any Obsidian component (VStack, HStack, List, Button, ScrollView, etc.)
- **Position Support**: Primary pane can be positioned on the leading (left) or trailing (right) side

## Architecture

The SplitView follows Obsidian's standard architecture pattern:

```
Public API (C++)          → include/obsidian/splitview.h
                         → src/obsidian/splitview.cpp
                            ↓
C++ FFI Wrapper          → packages/apple/macos_ffi.cpp
                            ↓
C FFI Interface          → packages/apple/macos_splitview.h
                            ↓
Objective-C++ Bridge     → packages/apple/macos_splitview.mm
                            ↓
Native macOS APIs        → NSSplitView / NSSplitViewController
```

## Basic Usage

### Creating a SplitView

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

// Initialize the application
App app;
app.initialize();

// Create a window
Window window;
window.create(1000, 700, "My App");

// Create SplitView with primary pane on the leading (left) side
SplitView splitView;
splitView.create(SplitPosition::Leading);

// Add to window
splitView.addToWindow(window);

// Show window
window.show();

// Run application
app.run(callbacks);
```

### Setting Primary and Secondary Pane Content

```cpp
// Create primary pane content (VStack with buttons)
VStack primaryPaneVStack;
primaryPaneVStack.setSpacing(10.0);
primaryPaneVStack.setPadding(Padding::all(15.0));

Button navButton1;
navButton1.create("Home", 0, 0, 200, 30);
primaryPaneVStack.addChild(navButton1);

primaryPaneVStack.updateLayout();
splitView.setPrimaryPaneContent(primaryPaneVStack);

// Create secondary pane content (VStack)
VStack secondaryPaneVStack;
secondaryPaneVStack.setSpacing(15.0);
secondaryPaneVStack.setPadding(Padding::all(20.0));

Button secondaryButton;
secondaryButton.create("Action", 0, 0, 200, 35);
secondaryPaneVStack.addChild(secondaryButton);

secondaryPaneVStack.updateLayout();
splitView.setSecondaryPaneContent(secondaryPaneVStack);
```

## API Reference

### Class: `obsidian::SplitView`

#### Construction and Lifecycle

##### `SplitView()`
Creates a new SplitView instance. The SplitView is not valid until `create()` is called.

##### `~SplitView()`
Destructor. Automatically removes the SplitView from its parent view and cleans up resources.

##### `bool create(SplitPosition position = SplitPosition::Leading)`
Creates the SplitView with the specified split position.

**Parameters:**
- `position`: Split position (`SplitPosition::Leading` for left side, `SplitPosition::Trailing` for right side)

**Returns:** `true` if the SplitView was created successfully, `false` otherwise.

**Example:**
```cpp
SplitView splitView;
if (splitView.create(SplitPosition::Leading)) {
    // SplitView created successfully
}
```

#### Primary Pane Width Management

##### `void setPrimaryPaneWidth(double width)`
Sets the primary pane width in points. The width will be clamped to the minimum/maximum values if set.

**Parameters:**
- `width`: Desired primary pane width in points

**Note:** The width is applied when the SplitView is laid out. If called before adding to a window, the width will be applied when the SplitView is added to the window.

##### `double getPrimaryPaneWidth() const`
Gets the current primary pane width in points.

**Returns:** Current primary pane width in points, or `0.0` if the SplitView is invalid or not laid out yet.

##### `void setMinimumPrimaryPaneWidth(double width)`
Sets the minimum primary pane width in points.

**Parameters:**
- `width`: Minimum primary pane width in points (default: 150.0)

**Note:** When users resize the primary pane, they cannot make it smaller than this value.

##### `void setMaximumPrimaryPaneWidth(double width)`
Sets the maximum primary pane width in points.

**Parameters:**
- `width`: Maximum primary pane width in points (default: 400.0)

**Note:** When users resize the primary pane, they cannot make it larger than this value.

**Example:**
```cpp
splitView.setMinimumPrimaryPaneWidth(150.0);  // Minimum: 150pt
splitView.setMaximumPrimaryPaneWidth(400.0);  // Maximum: 400pt
splitView.setPrimaryPaneWidth(250.0);         // Initial: 250pt
```

#### Collapse/Expand Management

##### `void collapsePrimaryPane()`
Collapses the primary pane. The primary pane width will be reduced to zero (or minimal width).

**Note:** The primary pane can be expanded again using `expandPrimaryPane()` or `togglePrimaryPane()`.

##### `void expandPrimaryPane()`
Expands the primary pane, restoring it from the collapsed state.

**Note:** The primary pane will be expanded to its preferred width (typically the last width before collapsing, or the initial width).

##### `void togglePrimaryPane()`
Toggles the primary pane between collapsed and expanded states.

**Example:**
```cpp
// Collapse primary pane
splitView.collapsePrimaryPane();

// Expand primary pane
splitView.expandPrimaryPane();

// Toggle primary pane
splitView.togglePrimaryPane();
```

##### `bool isPrimaryPaneCollapsed() const`
Checks if the primary pane is currently collapsed.

**Returns:** `true` if the primary pane is collapsed, `false` if it is expanded.

#### Content Management

##### Primary Pane Content

The following methods set the primary pane content view:

- `void setPrimaryPaneContent(VStack& vstack)`
- `void setPrimaryPaneContent(HStack& hstack)`
- `void setPrimaryPaneContent(List& list)`
- `void setPrimaryPaneContent(Button& button)`
- `void setPrimaryPaneContent(ScrollView& scrollView)`

**Parameters:**
- Component reference: The component to use as primary pane content

**Note:** The component must be valid (created and added to a window) before setting it as primary pane content.

##### Secondary Pane Content

The following methods set the secondary pane content view:

- `void setSecondaryPaneContent(VStack& vstack)`
- `void setSecondaryPaneContent(HStack& hstack)`
- `void setSecondaryPaneContent(List& list)`
- `void setSecondaryPaneContent(Button& button)`
- `void setSecondaryPaneContent(ScrollView& scrollView)`

**Parameters:**
- Component reference: The component to use as secondary pane content

**Note:** The component must be valid (created and added to a window) before setting it as secondary pane content.

**Example:**
```cpp
// Set primary pane content
VStack primaryPane;
primaryPane.addToWindow(window);
// ... configure primary pane ...
splitView.setPrimaryPaneContent(primaryPane);

// Set secondary pane content
VStack secondaryPane;
secondaryPane.addToWindow(window);
// ... configure secondary pane ...
splitView.setSecondaryPaneContent(secondaryPane);
```

#### Window Integration

##### `void addToWindow(Window& window)`
Adds the SplitView to a window's content view. The SplitView will fill the entire content area.

**Parameters:**
- `window`: Window to add the SplitView to

**Note:** If the SplitView has not been created yet, it will be automatically created with the default position (`SplitPosition::Leading`).

**Example:**
```cpp
Window window;
window.create(1000, 700, "My App");

SplitView splitView;
splitView.create(SplitPosition::Leading);
splitView.addToWindow(window);
```

##### `void removeFromParent()`
Removes the SplitView from its parent view.

**Note:** The SplitView remains valid after removal and can be added to another window.

#### State

##### `bool isValid() const`
Checks if the SplitView is valid.

**Returns:** `true` if the SplitView is valid (created and ready to use), `false` otherwise.

#### Callbacks

##### `void setOnPrimaryPaneToggle(std::function<void(bool collapsed)> callback)`
Sets a callback function that will be called when the primary pane is collapsed or expanded.

**Parameters:**
- `callback`: Function that takes a `bool` parameter (`true` if collapsed, `false` if expanded)

**Note:** The callback is invoked by the `collapsePrimaryPane()`, `expandPrimaryPane()`, and `togglePrimaryPane()` methods. User-initiated collapse/expand (e.g., double-clicking the divider) may not trigger the callback in all cases.

**Example:**
```cpp
splitView.setOnPrimaryPaneToggle([](bool collapsed) {
    if (collapsed) {
        std::cout << "Primary pane collapsed\n";
    } else {
        std::cout << "Primary pane expanded\n";
    }
});
```

### Enumeration: `SplitPosition`

Defines the position of the primary pane within the SplitView.

**Values:**
- `SplitPosition::Leading`: Primary pane on the left side (default in LTR layouts)
- `SplitPosition::Trailing`: Primary pane on the right side (default in LTR layouts)

**Example:**
```cpp
// Leading (left) sidebar
splitView.create(SplitPosition::Leading);

// Trailing (right) sidebar
splitView.create(SplitPosition::Trailing);
```

## Usage Examples

### Complete Example: Application with Sidebar

```cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

int main(int argc, char* argv[]) {
    // Initialize application
    App app;
    app.initialize();
    
    // Create window
    Window window;
    window.create(1000, 700, "My Application");
    
    // Create SplitView with leading sidebar
    SplitView splitView;
    splitView.create(SplitPosition::Leading);
    
    // Configure sidebar width constraints
    splitView.setMinimumPrimaryPaneWidth(150.0);
    splitView.setMaximumPrimaryPaneWidth(400.0);
    splitView.setPrimaryPaneWidth(250.0);
    
    // Create sidebar content
    VStack sidebar;
    sidebar.setSpacing(10.0);
    sidebar.setPadding(Padding::all(15.0));
    sidebar.addToWindow(window);
    
    Button homeButton;
    homeButton.create("Home", 0, 0, 200, 30);
    homeButton.setOnClick([]() { std::cout << "Home clicked\n"; });
    sidebar.addChild(homeButton);
    
    Button settingsButton;
    settingsButton.create("Settings", 0, 0, 200, 30);
    settingsButton.setOnClick([]() { std::cout << "Settings clicked\n"; });
    sidebar.addChild(settingsButton);
    
    sidebar.updateLayout();
    splitView.setPrimaryPaneContent(sidebar);
    
    // Create main content
    VStack main;
    main.setSpacing(15.0);
    main.setPadding(Padding::all(20.0));
    main.addToWindow(window);
    
    Button actionButton;
    actionButton.create("Perform Action", 0, 0, 200, 35);
    actionButton.setOnClick([]() { std::cout << "Action performed\n"; });
    main.addChild(actionButton);
    
    main.updateLayout();
    splitView.setSecondaryPaneContent(main);
    
    // Add SplitView to window
    splitView.addToWindow(window);
    
    // Set up sidebar toggle callback
    splitView.setOnPrimaryPaneToggle([](bool collapsed) {
        std::cout << "Sidebar " << (collapsed ? "collapsed" : "expanded") << "\n";
    });
    
    // Show window
    window.show();
    
    // Run application
    AppCallbacks callbacks;
    app.run(callbacks);
    
    // Cleanup
    window.close();
    app.shutdown();
    
    return 0;
}
```

### Example: Sidebar with List Component

```cpp
// Create sidebar with List
List sidebarList;
sidebarList.addToWindow(window);
sidebarList.addItem("Item 1");
sidebarList.addItem("Item 2");
sidebarList.addItem("Item 3");

splitView.setPrimaryPaneContent(sidebarList);
```

### Example: Collapsible Sidebar with Keyboard Shortcut

```cpp
// Set up sidebar toggle callback
splitView.setOnPrimaryPaneToggle([](bool collapsed) {
    // Update UI state based on sidebar visibility
    // ...
});

// Toggle sidebar programmatically (e.g., from menu or keyboard shortcut)
splitView.togglePrimaryPane();
```

## Best Practices

### 1. Initialize Components Before Setting as Content

Always ensure components are created and added to a window before setting them as SplitView content:

```cpp
// Good: Component is valid before setting as content
VStack sidebar;
sidebar.addToWindow(window);
// ... configure sidebar ...
splitView.setPrimaryPaneContent(sidebar);

// Bad: Component is not valid
VStack sidebar;
splitView.setPrimaryPaneContent(sidebar);  // Component not yet valid
```

### 2. Set Width Constraints Before Setting Width

Set minimum/maximum widths before setting the initial width to ensure proper constraints:

```cpp
// Good: Set constraints first
splitView.setMinimumPrimaryPaneWidth(150.0);
splitView.setMaximumPrimaryPaneWidth(400.0);
splitView.setPrimaryPaneWidth(250.0);  // Will be clamped to 150-400

// Bad: Width set before constraints
splitView.setPrimaryPaneWidth(250.0);
splitView.setMinimumPrimaryPaneWidth(150.0);  // Width already set
```

### 3. Update Layout After Modifying Content

When adding children to VStack/HStack components used as SplitView content, call `updateLayout()`:

```cpp
VStack sidebar;
sidebar.addToWindow(window);
sidebar.addChild(button1);
sidebar.addChild(button2);
sidebar.updateLayout();  // Update layout before setting as content
splitView.setPrimaryPaneContent(sidebar);
```

### 4. Handle Sidebar State

Use the `isPrimaryPaneCollapsed()` method to check sidebar state before performing operations:

```cpp
if (!splitView.isPrimaryPaneCollapsed()) {
    // Sidebar is visible, perform operations
    splitView.setPrimaryPaneWidth(300.0);
}
```

### 5. Clean Up Properly

The SplitView will automatically clean up when destroyed (RAII), but you can explicitly remove it:

```cpp
splitView.removeFromParent();  // Remove from parent view
// SplitView can still be reused or will be cleaned up on destruction
```

## Integration with Other Components

### VStack/HStack

VStack and HStack components work seamlessly with SplitView:

```cpp
VStack sidebar;
sidebar.setSpacing(10.0);
sidebar.setPadding(Padding::all(15.0));
// ... add children ...
sidebar.updateLayout();
splitView.setPrimaryPaneContent(sidebar);
```

### List

List components can be used for navigation-style sidebars:

```cpp
List sidebarList;
sidebarList.addItem("Home");
sidebarList.addItem("Projects");
sidebarList.addItem("Settings");
splitView.setPrimaryPaneContent(sidebarList);
```

### ScrollView

ScrollView components can be used when sidebar content might overflow:

```cpp
ScrollView scrollView;
scrollView.create(0, 0, 250, 600);
// ... set document view ...
splitView.setPrimaryPaneContent(scrollView);
```

### Button

Button components can be used for simple sidebars with navigation buttons:

```cpp
Button navButton;
navButton.create("Navigate", 0, 0, 200, 30);
splitView.setPrimaryPaneContent(navButton);
```

## Technical Notes

### NSSplitView Implementation

The SplitView uses `NSSplitView` with the following configuration:
- **Orientation**: Vertical (horizontal divider, left/right panes)
- **Divider Style**: Thin divider (`NSSplitViewDividerStyleThin`)
- **Delegate**: Implements `NSSplitViewDelegate` for collapse behavior and width constraints
- **Auto Layout**: Uses Auto Layout constraints for proper sizing

### Memory Management

- Uses ARC (Automatic Reference Counting) in Objective-C++ code
- Uses RAII pattern in C++ code
- Proper cleanup in destructors

### Performance Considerations

- SplitView layout is handled by macOS's native layout system (Auto Layout)
- No performance overhead beyond native NSSplitView behavior
- Resizing and collapse/expand operations are native macOS operations

## Limitations

1. **macOS Only**: Currently only available on macOS. Other platforms are not yet implemented.
2. **Two Panes Only**: Supports sidebar + main content (two panes). Three-pane layouts are not currently supported.
3. **Single Sidebar**: Only one sidebar per SplitView (leading or trailing, not both).

## Future Enhancements

- Support for multiple sidebars (leading + trailing)
- Animated collapse/expand
- Sidebar state persistence (User Defaults)
- Custom divider styling
- Three-pane layouts (sidebar + main + inspector)

## See Also

- [Sidebar Implementation Plan](../sidebar/SIDEBAR_IMPLEMENTATION_PLAN.md) - Detailed implementation documentation
- [Layout API Documentation](../layout/LAYOUT_API.md) - Layout system documentation
- [Example: sidebar_showcase](../../examples/sidebar_showcase/main.cpp) - Complete example application

## References

- [Apple NSSplitView Documentation](https://developer.apple.com/documentation/appkit/nssplitview)
- [Apple NSSplitViewController Documentation](https://developer.apple.com/documentation/appkit/nssplitviewcontroller)
- [macOS Human Interface Guidelines - Sidebars](https://developer.apple.com/design/human-interface-guidelines/components/layout-and-structure/sidebars/)
