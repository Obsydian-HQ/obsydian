# Sidebar API

## Overview

The Sidebar component provides a native macOS sidebar using `NSSplitViewController` and `NSSplitViewItem.sidebar()` with material background, native collapse/expand button, and full-height sidebar integration.

## Features

- **Material background**: Native macOS vibrancy effect
- **Native collapse/expand button**: Via `NSToolbarToggleSidebarItemIdentifier`
- **Full-height sidebar integration**: Extends into the titlebar area
- **Automatic window control integration**: Traffic lights
- **Proper collapse behavior**: Sidebar collapses within the window, window size stays fixed

## Class: Sidebar

### Construction

```cpp
Sidebar();
```

Creates a new Sidebar instance. Call `create()` to initialize it.

### Creation

```cpp
bool create();
```

Initializes the sidebar's internal split view controller and view controllers.

**Returns:** `true` if creation was successful, `false` otherwise.

### Setting Sidebar Content

```cpp
void setSidebarContent(VStack& vstack);
void setSidebarContent(HStack& hstack);
void setSidebarContent(List& list);
void setSidebarContent(Button& button);
void setSidebarContent(ScrollView& scrollView);
```

Sets the content of the sidebar pane. The provided component will be wrapped in an `NSViewController` with native sidebar material background (vibrancy effect).

**Parameters:**
- Component to use as sidebar content (VStack, HStack, List, Button, or ScrollView)

### Setting Main Content

```cpp
void setMainContent(VStack& vstack);
void setMainContent(HStack& hstack);
void setMainContent(List& list);
void setMainContent(Button& button);
void setMainContent(ScrollView& scrollView);
```

Sets the content of the main content pane (right side of the split view).

**Parameters:**
- Component to use as main content (VStack, HStack, List, Button, or ScrollView)

### Width Management

```cpp
void setMinimumSidebarWidth(double width);
double getMinimumSidebarWidth() const;
```

Set/get the minimum width of the sidebar in points.

**Default:** 200.0 points

```cpp
void setMaximumSidebarWidth(double width);
double getMaximumSidebarWidth() const;
```

Set/get the maximum width of the sidebar in points.

**Default:** 400.0 points

### Collapse/Expand Control

```cpp
void collapseSidebar();
void expandSidebar();
void toggleSidebar();
bool isSidebarCollapsed() const;
```

Control the sidebar collapse state.

- `collapseSidebar()`: Programmatically collapses the sidebar
- `expandSidebar()`: Programmatically expands the sidebar
- `toggleSidebar()`: Toggles the sidebar between collapsed and expanded states
- `isSidebarCollapsed()`: Returns `true` if collapsed, `false` if expanded

### Window Integration

```cpp
void addToWindow(Window& window);
```

Adds the sidebar to a window. The split view controller's view becomes the window's content view.

**Important:** Call `window.setupToolbarForSidebar()` **before** calling this method for proper native collapse button functionality.

```cpp
void removeFromParent();
```

Removes the sidebar from its parent window.

### State

```cpp
bool isValid() const;
```

Checks if the sidebar has been successfully created and is ready to use.

**Returns:** `true` if valid, `false` otherwise.

### Callbacks

```cpp
void setOnSidebarToggle(std::function<void(bool collapsed)> callback);
```

Sets a callback function that will be called when the sidebar is toggled (collapsed or expanded).

**Parameters:**
- `callback`: Function that receives `true` if sidebar was collapsed, `false` if expanded

## Window Setup for Sidebar

For proper native sidebar functionality, the window must be configured correctly. Use `Window::setupToolbarForSidebar()` to:

1. Configure the window with `NSWindowStyleMaskFullSizeContentView`
2. Hide the window title and make the titlebar transparent
3. Create and configure the toolbar with the native sidebar toggle button

## Usage Example

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
    
    // Optional: Set callback for sidebar toggle events
    sidebar.setOnSidebarToggle([](bool collapsed) {
        if (collapsed) {
            std::cout << "Sidebar collapsed\n";
        } else {
            std::cout << "Sidebar expanded\n";
        }
    });
    
    // Show and run
    window.show();
    app.run({});
    
    return 0;
}
```

## Implementation Details

The Sidebar implementation is based on real working apps (Watt editor, HelloAppKit):

1. Create split view controller
2. Set its view frame size BEFORE adding items
3. Add items
4. Add to window

The sidebar uses:
- `NSSplitViewController` for the split view management
- `NSSplitViewItem.sidebar()` for the sidebar pane with material background
- `NSSplitViewItem.contentList()` for the main content pane
- Native view controllers wrapping the Obsidian components

## Notes

- Sidebar is macOS-only (uses `#ifdef __APPLE__`)
- Sidebar is non-copyable but movable
- The sidebar must be created before setting content
- Window toolbar setup is required for native collapse button functionality
- Sidebar content components must be valid (created) before being set
