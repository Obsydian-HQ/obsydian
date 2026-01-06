# Navigation API

## Overview

The Navigation API provides a file-based routing system for Obsidian applications, similar to Next.js App Router. It includes route discovery, matching, rendering, and navigation history management.

## Core Components

### Router

The main router class that manages file-based routing, navigation history, and route matching.

#### Construction

```cpp
Router();
```

Creates a new Router instance.

#### Initialization

```cpp
bool initialize(const std::string& appDirectory = "app");
```

Initialize the router by scanning the app directory for route files.

**Parameters:**
- `appDirectory`: Path to the app/ directory (default: "app")

**Returns:** `true` if initialization was successful, `false` otherwise.

**Note:** Route files register their components during static initialization. The router processes these registrations during `initialize()`.

#### Navigation

```cpp
void navigate(const std::string& path);
void navigate(const std::string& path, const std::map<std::string, std::string>& params);
```

Navigate to a route. The path can include query parameters (e.g., "/about?tab=settings").

**Parameters:**
- `path`: Route path (e.g., "/about", "/users/123")
- `params`: Route parameters (for dynamic routes, optional)

```cpp
void push(const std::string& path);
```

Push a new route onto history stack (same as `navigate`).

```cpp
void replace(const std::string& path);
```

Replace current route (doesn't add to history).

#### History Navigation

```cpp
void goBack();
void goForward();
bool canGoBack() const;
bool canGoForward() const;
```

Navigate through history stack.

```cpp
void clearHistory();
int getHistorySize() const;
std::string getCurrentPath() const;
```

Manage and query navigation history.

#### Route Information

```cpp
bool routeExists(const std::string& path) const;
std::vector<std::string> getAllRoutes() const;
```

Query available routes.

#### Window Integration

```cpp
void setWindow(Window& window);
```

Set the window for route rendering. This initializes the ScreenContainer and attaches it to the window.

#### Callbacks

```cpp
void setOnNavigation(std::function<void(const std::string& path)> callback);
```

Set callback for navigation events. Called whenever navigation occurs.

#### Route Registration

```cpp
void registerRouteComponent(const std::string& routePath, 
                            std::function<void(RouteContext&)> componentFunction);
```

Register a route component function. Route files should call this to register their component functions.

```cpp
void registerLayoutComponent(const std::string& routePath,
                              std::function<void(RouteContext&, std::function<void()>)> layoutFunction);
```

Register a layout component function. Layout files should call this to register their layout functions.

#### State

```cpp
bool isValid() const;
```

Check if router is valid and initialized.

### RouteContext

Provides route information and navigation methods to route components.

#### Construction

RouteContext is created by the router and passed to route component functions. You don't construct it directly.

#### Accessors

```cpp
Window& getWindow() const;
Screen* getScreen() const;
const std::string& getPath() const;
const std::map<std::string, std::string>& getParams() const;
const std::map<std::string, std::string>& getQuery() const;
Router& getRouter() const;
```

Get route information:
- `getWindow()`: Window for rendering
- `getScreen()`: Screen for this route (owned by ScreenContainer, may be nullptr)
- `getPath()`: Current route path
- `getParams()`: Route parameters (for dynamic routes)
- `getQuery()`: Query parameters
- `getRouter()`: Router reference (for capturing in lambdas)

#### Navigation Methods

```cpp
void navigate(const std::string& path);
void goBack();
void goForward();
bool canGoBack() const;
bool canGoForward() const;
```

Navigate from within a route component.

#### Content Rendering

```cpp
template<typename T>
void setContent(T& component);
```

Set content for the current route. If a content slot is set (by a parent layout), renders into the slot. Otherwise renders to screen if available, or window as fallback.

The component must have:
- `addToScreen(Screen&)` method
- `addToWindow(Window&)` method
- `addToParentView(void*)` method

#### Nested Layout Support

```cpp
void setContentSlot(void* slotViewHandle);
void* getContentSlot() const;
bool hasContentSlot() const;
```

Support for nested layout rendering:
- `setContentSlot()`: Called by layouts to establish where children should render
- `getContentSlot()`: Get the current content slot (nullptr if none)
- `hasContentSlot()`: Check if a content slot is set

## File-Based Routing

The router uses file-based routing conventions similar to Next.js:

### Route File Types

- **`index.cpp`**: Index route (e.g., `app/index.cpp` → `/`)
- **`[param].cpp`**: Dynamic route segment (e.g., `app/users/[id].cpp` → `/users/:id`)
- **`[...catchall].cpp`**: Catch-all route (e.g., `app/docs/[...path].cpp` → `/docs/*`)
- **`_layout.cpp`**: Layout component (e.g., `app/_layout.cpp` → layout for `/`)
- **Regular files**: Static routes (e.g., `app/about.cpp` → `/about`)

### Route Registration

Route files should register their components during static initialization:

```cpp
// app/about.cpp
#include <obsidian/router.h>
#include <obsidian/route_registry_helper.h>

void aboutRoute(obsidian::RouteContext& ctx) {
    obsidian::VStack content;
    // ... build content ...
    ctx.setContent(content);
}

// Register during static initialization
static bool _registered = []() {
    if (obsidian::g_router) {
        obsidian::g_router->registerRouteComponent("/about", aboutRoute);
    } else {
        obsidian::registerPendingRoute("/about", aboutRoute);
    }
    return true;
}();
```

### Layout Registration

Layout files should register their layout functions:

```cpp
// app/_layout.cpp
#include <obsidian/router.h>
#include <obsidian/route_registry_helper.h>

void rootLayout(obsidian::RouteContext& ctx, std::function<void()> renderChild) {
    obsidian::VStack layout;
    // ... add header, nav, etc. ...
    
    obsidian::VStack contentSlot;  // Where children will render
    layout.addChild(contentSlot);
    ctx.setContentSlot(contentSlot.getNativeViewHandle());  // Register slot
    
    ctx.setContent(layout);  // Add layout to screen
    renderChild();  // Now children render into contentSlot
}

// Register during static initialization
static bool _registered = []() {
    if (obsidian::g_router) {
        obsidian::g_router->registerLayoutComponent("/", rootLayout);
    } else {
        obsidian::registerPendingLayout("/", rootLayout);
    }
    return true;
}();
```

## Core Routing Components

### RouteScanner

Scans the app/ directory at runtime to discover route files.

```cpp
bool scan(const std::string& appDirectory);
const std::vector<RouteFile>& getRouteFiles() const;
bool isValid() const;
```

### RouteRegistry

Builds and maintains a route tree from scanned route files.

```cpp
bool buildFromScanner(const RouteScanner& scanner);
RouteNode* matchRoute(const std::string& path, std::map<std::string, std::string>& params) const;
std::vector<std::string> getAllRoutes() const;
bool routeExists(const std::string& path) const;
```

### RouteRenderer

Handles rendering of route components and layout nesting.

```cpp
void registerRouteComponent(const std::string& routePath, RouteComponentFunction componentFunction);
void registerLayoutComponent(const std::string& routePath, LayoutComponentFunction layoutFunction);
bool renderRoute(const RouteNode* routeNode, Window& window, ScreenContainer* screenContainer, 
                 Router& router, const std::map<std::string, std::string>& params,
                 const std::map<std::string, std::string>& query);
```

### NavigationHistory

Manages navigation history stack for back/forward navigation.

```cpp
void push(const std::string& path, 
          const std::map<std::string, std::string>& params = {},
          const std::map<std::string, std::string>& query = {});
void replace(const std::string& path,
             const std::map<std::string, std::string>& params = {},
             const std::map<std::string, std::string>& query = {});
const HistoryEntry* goBack();
const HistoryEntry* goForward();
const HistoryEntry* getCurrent() const;
bool canGoBack() const;
bool canGoForward() const;
void clear();
size_t size() const;
```

## Usage Example

```cpp
#include <obsidian/obsidian.h>

int main() {
    obsidian::App app;
    app.initialize();
    
    // Create window
    obsidian::Window window;
    window.create(1200, 800, "My App");
    window.show();
    
    // Create and initialize router
    obsidian::Router router;
    if (!router.initialize("app")) {
        std::cerr << "Failed to initialize router\n";
        return 1;
    }
    
    // Set window for rendering
    router.setWindow(window);
    
    // Optional: Set navigation callback
    router.setOnNavigation([](const std::string& path) {
        std::cout << "Navigated to: " << path << std::endl;
    });
    
    // Navigate to initial route
    router.navigate("/");
    
    // Run app
    app.run({});
    
    return 0;
}
```

## Route File Example

```cpp
// app/users/[id].cpp
#include <obsidian/router.h>
#include <obsidian/route_registry_helper.h>
#include <obsidian/vstack.h>
#include <obsidian/textview.h>
#include <iostream>

void userRoute(obsidian::RouteContext& ctx) {
    // Get route parameters
    const auto& params = ctx.getParams();
    const std::string& userId = params.at("id");
    
    // Get query parameters
    const auto& query = ctx.getQuery();
    std::string tab = query.count("tab") > 0 ? query.at("tab") : "profile";
    
    // Build content
    obsidian::VStack content;
    content.setSpacing(20.0);
    content.setPadding(obsidian::Padding::all(30.0));
    
    obsidian::TextView title;
    title.create("User: " + userId, 0, 0, 400, 40);
    content.addChild(title);
    
    obsidian::TextView tabView;
    tabView.create("Tab: " + tab, 0, 0, 400, 40);
    content.addChild(tabView);
    
    content.updateLayout();
    ctx.setContent(content);
}

// Register route
static bool _registered = []() {
    if (obsidian::g_router) {
        obsidian::g_router->registerRouteComponent("/users/:id", userRoute);
    } else {
        obsidian::registerPendingRoute("/users/:id", userRoute);
    }
    return true;
}();
```

## Layout File Example

```cpp
// app/_layout.cpp
#include <obsidian/router.h>
#include <obsidian/route_registry_helper.h>
#include <obsidian/vstack.h>
#include <obsidian/hstack.h>
#include <obsidian/button.h>

void rootLayout(obsidian::RouteContext& ctx, std::function<void()> renderChild) {
    obsidian::VStack layout;
    layout.setSpacing(0);
    
    // Header
    obsidian::HStack header;
    header.setPadding(obsidian::Padding::all(10.0));
    
    obsidian::Button homeButton;
    homeButton.create("Home", 0, 0, 80, 30);
    homeButton.setOnClick([&ctx]() {
        ctx.navigate("/");
    });
    header.addChild(homeButton);
    
    obsidian::Button aboutButton;
    aboutButton.create("About", 0, 0, 80, 30);
    aboutButton.setOnClick([&ctx]() {
        ctx.navigate("/about");
    });
    header.addChild(aboutButton);
    
    header.updateLayout();
    layout.addChild(header);
    
    // Content slot for children
    obsidian::VStack contentSlot;
    contentSlot.setSpacing(0);
    layout.addChild(contentSlot);
    layout.updateLayout();
    
    // Register content slot
    ctx.setContentSlot(contentSlot.getNativeViewHandle());
    
    // Add layout to screen
    ctx.setContent(layout);
    
    // Render child route into content slot
    renderChild();
}

// Register layout
static bool _registered = []() {
    if (obsidian::g_router) {
        obsidian::g_router->registerLayoutComponent("/", rootLayout);
    } else {
        obsidian::registerPendingLayout("/", rootLayout);
    }
    return true;
}();
```

## Notes

- Router is non-copyable but movable
- Route files must register their components during static initialization
- The router processes pending registrations during `initialize()`
- Navigation history is managed automatically
- Route matching supports dynamic parameters and catch-all routes
- Layout nesting is supported through content slots
- ScreenContainer manages route views and provides persistence until explicitly cleared
- The router must be initialized before navigation
- Window must be set before navigation for rendering to work
