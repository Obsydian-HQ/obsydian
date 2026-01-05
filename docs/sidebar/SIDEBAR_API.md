# Obsidian Sidebar API

## Overview

The Obsidian Sidebar provides a **native macOS sidebar** using `NSSplitViewController` and `NSSplitViewItem.sidebar()` with:

- **Material background** (native macOS vibrancy effect)
- **Native collapse/expand button** (via `NSToolbarToggleSidebarItemIdentifier`)
- **Full-height sidebar integration** (extends into the titlebar area)
- **Automatic window control integration** (traffic lights)
- **Proper collapse behavior** (sidebar collapses within the window, window size stays fixed)

## Quick Start

```cpp
#include <obsidian/obsidian.h>

int main() {
    obsidian::App app;
    app.initialize();
    
    // Create window
    obsidian::Window window;
    window.create(1200, 800, "My App");
    
    // IMPORTANT: Setup toolbar BEFORE creating sidebar
    // This provides the native collapse button
    window.setupToolbarForSidebar();
    
    // Create sidebar
    obsidian::Sidebar sidebar;
    sidebar.create();
    
    // Create sidebar content
    obsidian::List sidebarList;
    sidebarList.create(0, 0, 250, 600);
    sidebarList.addItem("Home");
    sidebarList.addItem("Projects");
    sidebarList.addItem("Settings");
    
    // Set sidebar content
    sidebar.setSidebarContent(sidebarList);
    
    // Create main content
    obsidian::VStack mainContent;
    mainContent.setSpacing(20.0);
    mainContent.setPadding(obsidian::Padding::all(30.0));
    
    obsidian::Button button;
    button.create("Click Me", 0, 0, 200, 40);
    mainContent.addChild(button);
    mainContent.updateLayout();
    
    // Set main content
    sidebar.setMainContent(mainContent);
    
    // Add to window
    sidebar.addToWindow(window);
    
    // Show and run
    window.show();
    app.run({});
    
    return 0;
}
```

## API Reference

### `class Sidebar`

The main sidebar class that manages a native macOS split view with a sidebar pane and a main content pane.

#### Constructor

```cpp
Sidebar();
```

Creates a new sidebar instance. Call `create()` to initialize it.

#### Methods

##### `bool create()`

Initializes the sidebar's internal split view controller and view controllers.

**Returns:** `true` if creation was successful, `false` otherwise.

**Example:**
```cpp
obsidian::Sidebar sidebar;
if (!sidebar.create()) {
    std::cerr << "Failed to create sidebar\n";
    return 1;
}
```

##### `void setSidebarContent(VStack& vstack)`
##### `void setSidebarContent(HStack& hstack)`
##### `void setSidebarContent(List& list)`
##### `void setSidebarContent(Button& button)`
##### `void setSidebarContent(ScrollView& scrollView)`

Sets the content of the sidebar pane. The provided component will be wrapped in an NSViewController with native sidebar material background (vibrancy effect).

**Parameters:**
- Component to use as sidebar content (VStack, HStack, List, Button, or ScrollView)

**Example:**
```cpp
obsidian::List sidebarList;
sidebarList.create(0, 0, 250, 600);
sidebarList.addItem("Item 1");
sidebarList.addItem("Item 2");
sidebar.setSidebarContent(sidebarList);
```

##### `void setMainContent(VStack& vstack)`
##### `void setMainContent(HStack& hstack)`
##### `void setMainContent(List& list)`
##### `void setMainContent(Button& button)`
##### `void setMainContent(ScrollView& scrollView)`

Sets the content of the main content pane (right side of the split view).

**Parameters:**
- Component to use as main content (VStack, HStack, List, Button, or ScrollView)

**Example:**
```cpp
obsidian::VStack mainContent;
mainContent.setSpacing(20.0);
// Add children to mainContent...
sidebar.setMainContent(mainContent);
```

##### `void setMinimumSidebarWidth(double width)`

Sets the minimum width of the sidebar in points.

**Parameters:**
- `width`: Minimum width in points (default: 150.0)

##### `double getMinimumSidebarWidth() const`

Gets the minimum sidebar width in points.

**Returns:** Minimum width in points.

##### `void setMaximumSidebarWidth(double width)`

Sets the maximum width of the sidebar in points.

**Parameters:**
- `width`: Maximum width in points (default: 400.0)

##### `double getMaximumSidebarWidth() const`

Gets the maximum sidebar width in points.

**Returns:** Maximum width in points.

##### `void collapseSidebar()`

Programmatically collapses the sidebar.

##### `void expandSidebar()`

Programmatically expands the sidebar.

##### `void toggleSidebar()`

Toggles the sidebar between collapsed and expanded states.

##### `bool isSidebarCollapsed() const`

Checks if the sidebar is currently collapsed.

**Returns:** `true` if collapsed, `false` if expanded.

##### `void addToWindow(Window& window)`

Adds the sidebar to a window. The split view controller's view becomes the window's content view.

**Parameters:**
- `window`: Window to add the sidebar to

**Important:** Call `window.setupToolbarForSidebar()` **before** calling this method for proper native collapse button functionality.

##### `void removeFromParent()`

Removes the sidebar from its parent window.

##### `bool isValid() const`

Checks if the sidebar has been successfully created and is ready to use.

**Returns:** `true` if valid, `false` otherwise.

##### `void setOnSidebarToggle(std::function<void(bool collapsed)> callback)`

Sets a callback function that will be called when the sidebar is toggled (collapsed or expanded).

**Parameters:**
- `callback`: Function that receives `true` if sidebar was collapsed, `false` if expanded

**Example:**
```cpp
sidebar.setOnSidebarToggle([](bool collapsed) {
    if (collapsed) {
        std::cout << "Sidebar collapsed\n";
    } else {
        std::cout << "Sidebar expanded\n";
    }
});
```

## Window Setup for Sidebar

For proper native sidebar functionality, the window must be configured correctly. Use `Window::setupToolbarForSidebar()` to:

1. Configure the window with `NSWindowStyleMaskFullSizeContentView`
2. Hide the window title and make the titlebar transparent
3. Create and configure the toolbar with the native sidebar toggle button
4. Set the unified toolbar style

```cpp
obsidian::Window window;
window.create(1200, 800, "My App");

// MUST be called before creating/adding sidebar
if (!window.setupToolbarForSidebar()) {
    std::cerr << "Failed to setup toolbar\n";
}
```

## Collapse Behavior

The sidebar uses `NSSplitViewItemCollapseBehaviorPreferResizingSiblingsWithFixedSplitView` which ensures:

- When the sidebar collapses, the **window size stays fixed**
- The **main content area expands** to fill the space where the sidebar was
- This is the correct behavior for native macOS sidebars (like Finder, Xcode, etc.)

Previous versions incorrectly used `NSSplitViewItemCollapseBehaviorPreferResizingSplitViewWithFixedSiblings` which caused the window to shrink instead of the sidebar collapsing within the window.

## Best Practices

1. **Always call `window.setupToolbarForSidebar()` before adding the sidebar** - This ensures the native collapse button works correctly.

2. **Use List for sidebar navigation** - The `List` component automatically gets source list styling when placed in a sidebar.

3. **Set reasonable width constraints** - Use `setMinimumSidebarWidth()` and `setMaximumSidebarWidth()` to prevent the sidebar from becoming too small or too large.

4. **Handle toggle events** - Use `setOnSidebarToggle()` to respond to collapse/expand events if your app needs to adjust content.

5. **Create content before setting** - Make sure your content components (List, VStack, etc.) are fully created and populated before calling `setSidebarContent()` or `setMainContent()`.

## Related APIs

- `Window::setupToolbarForSidebar()` - Configure window for sidebar integration
- `Window::setToolbarStyle()` - Set toolbar style (Unified recommended for sidebars)
- `List` - Recommended component for sidebar navigation
- `VStack/HStack` - Layout components for organizing sidebar/main content

## Example Applications

See `/examples/sidebar_showcase/main.cpp` for a complete working example.
