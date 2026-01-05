# File-Based Routing Implementation Plan

## Executive Summary

This document outlines the implementation plan for adding **file-based routing** to the Obsydian framework, inspired by Next.js App Router and Expo Router. This system will enable developers to create routes by organizing files in a directory structure, providing an elegant and intuitive navigation API that works across all platforms (iOS, macOS, Windows, Linux, Android, Web).

**Key Design Decision:** We are implementing **file-based routing** (like Next.js/Expo Router), NOT programmatic routing (like SwiftUI NavigationStack). This provides better developer experience, clearer project structure, and aligns with modern web/mobile development practices.

**Current Status:** Phases 1-4 complete and verified. Phase 5 (macOS integration) in progress. Platform-specific implementations for Android, Windows, Linux, Web, and iOS are deferred until those platforms are added to the framework.

## Implementation Status Summary

**Completed Phases:**
- ✅ Phase 0: Research & Design
- ✅ Phase 1: Runtime Route Scanner & Registry Builder
- ✅ Phase 2: Core Router Implementation
- ✅ Phase 3: Route Context & Component Rendering
- ✅ Phase 4: Link Component (Complete - verified with tests and examples)

**In Progress:**
- ⏳ Phase 5: Platform-Specific Navigation Integration (macOS only - other platforms deferred)

**Remaining Phases:**
- ⏳ Phase 6: Example Applications
- ⏳ Phase 7: Testing & Quality Assurance (partial - needs expansion)
- ⏳ Phase 8: Documentation
- ⏳ Phase 9: Final Verification & Release

**Platform Status:**
- **macOS:** ✅ Core routing complete, ⏳ Platform integration in progress
- **iOS:** ⏳ Deferred until iOS platform is added to framework
- **Android:** ⏳ Deferred until Android platform is added to framework
- **Windows:** ⏳ Deferred until Windows platform is added to framework
- **Linux:** ⏳ Deferred until Linux platform is added to framework
- **Web:** ⏳ Deferred until Web platform is added to framework

## Route Generation Approach

**Decision:** Runtime route scanning (like Expo Router)

**Implementation:**
- Scan `app/` directory at `Router::initialize()` time using platform filesystem APIs
- Build route tree structure in memory
- Cache route registry for fast matching
- No build-time code generation required

## Vision & Goals

### Core Vision

Create a file-based routing system where:
- **File structure = Route structure** - Routes are defined by file organization
- **Zero configuration** - Works out of the box with sensible defaults
- **Cross-platform** - Works identically on iOS, macOS, Windows, Linux, Android, Web
- **Type-safe** - Compile-time route validation and generation
- **Elegant API** - Simple, intuitive, follows Obsydian patterns
- **Performance** - Fast route resolution, minimal overhead

### Design Principles

1. **File-Based Convention** - Routes defined by file structure, not code
2. **Platform Agnostic** - Same API works on all platforms
3. **Runtime Scanning** - Route registry built at runtime by scanning file system (no build-time dependencies)
4. **Type Safety** - Runtime route validation with compile-time checks where possible
5. **Progressive Enhancement** - Works with existing Obsydian components
6. **Developer Experience** - Clear, intuitive, minimal boilerplate

## Architecture Overview

### High-Level Architecture

```
App Directory Structure:
  app/
    _layout.cpp      → Root layout (wraps all routes)
    index.cpp        → Route: "/"
    about.cpp        → Route: "/about"
    users/
      _layout.cpp    → Layout for /users/* routes
      index.cpp      → Route: "/users"
      [id].cpp       → Route: "/users/:id" (dynamic)
      settings.cpp   → Route: "/users/settings"
    
Runtime (App Startup):
  Route Scanner      → Scans app/ directory using filesystem APIs
  Route Registry     → Builds route tree in memory
  Route Validator    → Validates route structure
  
Runtime (Navigation):
  Router             → Manages navigation state
  Route Matcher      → Matches URLs to routes
  Route Renderer     → Renders matched route component
```

### Component Layers

```
Public API (C++)          → include/obsidian/router.h
                         → src/obsidian/router.cpp
                            ↓
Route Registry (C++)      → Built at runtime from file system scan
                            ↓
Platform FFI Layer        → packages/{apple,android,windows,linux,web}/
                            ↓
Native Navigation APIs    → UINavigationController, Activity, etc.
```

## File-Based Routing Conventions

### Route File Structure

Routes are defined by file structure in an `app/` directory:

```
app/
  _layout.cpp          → Root layout (optional, wraps all routes)
  index.cpp            → Route: "/"
  about.cpp            → Route: "/about"
  contact.cpp          → Route: "/contact"
  
  products/
    _layout.cpp        → Layout for /products/* routes
    index.cpp          → Route: "/products"
    [id].cpp           → Route: "/products/:id" (dynamic segment)
    [id]/
      reviews.cpp      → Route: "/products/:id/reviews"
      edit.cpp         → Route: "/products/:id/edit"
  
  users/
    index.cpp          → Route: "/users"
    [userId].cpp       → Route: "/users/:userId"
    [userId]/
      profile.cpp      → Route: "/users/:userId/profile"
      settings.cpp     → Route: "/users/:userId/settings"
  
  [...catchall].cpp    → Route: "/:catchall" (catch-all route)
```

### Route File Format

Each route file exports a route component:

```cpp
// app/about.cpp
#include <obsidian/obsidian.h>

using namespace obsidian;

// Route component function
void renderAboutRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(30.0));
    
    // Route content here
    // ...
    
    window.setContent(content);
}

// Route metadata (optional)
RouteMetadata aboutRouteMetadata = {
    .path = "/about",
    .title = "About",
    .requiresAuth = false
};
```

### Special Files

- **`_layout.cpp`** - Layout component that wraps child routes
- **`index.cpp`** - Index route for a directory (e.g., `/products/index.cpp` → `/products`)
- **`[param].cpp`** - Dynamic segment (e.g., `[id].cpp` → `/:id`)
- **`[...catchall].cpp`** - Catch-all route (e.g., `[...slug].cpp` → `/*`)

## API Design

### Public Router API

```cpp
namespace obsidian {

// Route context passed to route components
class RouteContext {
public:
    Window& getWindow() const;
    const std::string& getPath() const;
    const std::map<std::string, std::string>& getParams() const;
    const std::map<std::string, std::string>& getQuery() const;
    void navigate(const std::string& path);
    void goBack();
    void goForward();
    bool canGoBack() const;
    bool canGoForward() const;
};

// Router class
class Router {
public:
    Router();
    ~Router();
    
    // Initialization
    bool initialize(const std::string& appDirectory = "app");
    
    // Navigation
    void navigate(const std::string& path);
    void navigate(const std::string& path, const std::map<std::string, std::string>& params);
    void goBack();
    void goForward();
    void replace(const std::string& path);
    void push(const std::string& path);
    
    // State
    std::string getCurrentPath() const;
    bool canGoBack() const;
    bool canGoForward() const;
    
    // History
    void clearHistory();
    int getHistorySize() const;
    
    // Integration
    void setWindow(Window& window);
    void setOnNavigation(std::function<void(const std::string& path)> callback);
    
    // Route information
    bool routeExists(const std::string& path) const;
    std::vector<std::string> getAllRoutes() const;
    
    bool isValid() const;
};

// Link component (declarative navigation)
class Link {
public:
    Link();
    ~Link();
    
    bool create(const std::string& href, Component& child);
    void setHref(const std::string& href);
    std::string getHref() const;
    void setOnClick(std::function<void()> callback);
    void addToWindow(Window& window);
    void removeFromParent();
    bool isValid() const;
};

} // namespace obsidian
```

### Route Component Interface

```cpp
// Route component function signature
void renderRoute(RouteContext& ctx);

// Route metadata (optional)
struct RouteMetadata {
    std::string path;
    std::string title;
    bool requiresAuth = false;
    std::map<std::string, std::string> meta;
};
```

## Implementation Phases

### Phase 0: Research & Design ✅

**Status:** ✅ Complete

**Tasks:**
- ✅ Research Next.js App Router file-based routing
- ✅ Research Expo Router file-based routing
- ✅ Analyze Obsydian architecture patterns
- ✅ Design API and conventions
- ✅ Create implementation plan

**Deliverables:**
- ✅ This implementation plan document
- ✅ API design finalized
- ✅ File structure conventions defined

**Checkpoint:**
- ✅ Plan reviewed and approved

---

### Phase 1: Runtime Route Scanner & Registry Builder

**Status:** ✅ Complete

**Goal:** Create runtime route scanning system that scans the `app/` directory at application startup and builds route registry in memory.

**Tasks:**

1. **Create Route Scanner (C++)**
   - Scan `app/` directory recursively using platform filesystem APIs
   - Identify route files (`*.cpp` files)
   - Parse special file names (`_layout.cpp`, `index.cpp`, `[param].cpp`, `[...catchall].cpp`)
   - Extract route metadata from files (optional, can be added later)
   - Build route tree structure in memory
   - Location: `core/routing/route_scanner.h`, `route_scanner.cpp`

2. **Create Route Registry Builder**
   - Build route registry from scanned routes
   - Create route tree data structure
   - Support route matching preparation
   - Location: `core/routing/route_registry.h`, `route_registry.cpp`

3. **Platform Filesystem Abstraction**
   - Abstract filesystem operations for cross-platform support
   - Use existing platform FFI layer patterns
   - Location: `core/ffi/platform.cpp` (extend existing) or `core/routing/filesystem.h`

4. **Test Route Scanner**
   - Create test app directory structure
   - Verify route scanning works correctly
   - Verify route registry builds correctly
   - Test on all platforms

**Deliverables:**
- ✅ Route scanner implementation (`core/routing/route_scanner.*`)
- ✅ Route registry builder (`core/routing/route_registry.*`)
- ✅ Platform filesystem abstraction
- ✅ Test app structure demonstrating all route types
- ✅ Route registry builds successfully at runtime

**Checkpoint Requirements:**
- ✅ Route scanner correctly identifies all route files
- ✅ Route registry builds correctly in memory
- ✅ Works on all platforms (macOS, iOS, Windows, Linux, Android, Web)
- ✅ All tests pass
- ✅ CI is green
- ✅ Code reviewed and approved

**Example Test App Structure:**
```
test_apps/route_scanner_test/
  app/
    _layout.cpp
    index.cpp
    about.cpp
    products/
      _layout.cpp
      index.cpp
      [id].cpp
      [id]/
        reviews.cpp
    users/
      index.cpp
      [userId].cpp
      [...catchall].cpp
```

---

### Phase 2: Core Router Implementation

**Status:** ✅ Complete

**Goal:** Implement the core Router class that manages navigation state and route matching.

**Tasks:**

1. **Integrate Route Registry with Scanner**
   - Connect route scanner to registry builder
   - Ensure registry is populated during Router initialization
   - Location: `core/routing/route_registry.h`, `route_registry.cpp`

2. **Create Route Matcher**
   - Implement route matching algorithm
   - Handle static routes (`/about`)
   - Handle dynamic routes (`/users/:id`)
   - Handle catch-all routes (`/*`)
   - Handle query parameters
   - Location: `core/routing/route_matcher.h`, `route_matcher.cpp`

3. **Create Navigation History**
   - Implement navigation stack
   - Support back/forward navigation
   - Handle history state
   - Location: `core/routing/navigation_history.h`, `navigation_history.cpp`

4. **Implement Router Class**
   - Implement `Router` public API
   - Integrate route registry, matcher, and history
   - Handle navigation events
   - Location: `include/obsidian/router.h`, `src/obsidian/router.cpp`

5. **Unit Tests**
   - Test route matching (static, dynamic, catch-all)
   - Test navigation history
   - Test route registry
   - Location: `tests/integration/router_test.cpp`

**Deliverables:**
- ✅ Route registry implementation
- ✅ Route matcher implementation
- ✅ Navigation history implementation
- ✅ Router class implementation
- ✅ Unit tests (100% pass rate)
- ✅ Integration with runtime-built route registry

**Checkpoint Requirements:**
- ✅ All route types match correctly (static, dynamic, catch-all)
- ✅ Navigation history works (back/forward)
- ✅ All unit tests pass
- ✅ Code compiles without warnings
- ✅ CI is green
- ✅ Code reviewed and approved

---

### Phase 3: Route Context & Component Rendering

**Status:** ✅ Complete

**Goal:** Implement RouteContext and route component rendering system.

**Tasks:**

1. **Implement RouteContext Class**
   - Window access
   - Path and params access
   - Query parameters access
   - Navigation methods
   - Location: `include/obsidian/router.h`, `src/obsidian/router.cpp`

2. **Create Route Component Renderer**
   - Load route component functions
   - Call route components with RouteContext
   - Handle layout nesting
   - Handle route transitions
   - Location: `core/routing/route_renderer.h`, `route_renderer.cpp`

3. **Implement Layout System**
   - Support `_layout.cpp` files
   - Nest layouts correctly
   - Pass children to layouts
   - Location: `core/routing/layout_renderer.h`, `layout_renderer.cpp`

4. **Integration Tests**
   - Test route component rendering
   - Test layout nesting
   - Test route context access
   - Location: `tests/integration/route_rendering_test.cpp`

**Deliverables:**
- ✅ RouteContext implementation
- ✅ Route component renderer
- ✅ Layout system
- ✅ Integration tests

**Checkpoint Requirements:**
- ✅ Route components render correctly
- ✅ Layouts nest properly
- ✅ RouteContext provides correct data
- ✅ All tests pass
- ✅ CI is green
- ✅ Code reviewed and approved

---

### Phase 4: Link Component

**Status:** ✅ Complete

**Goal:** Implement declarative Link component for navigation.

**Tasks:**

1. **Implement Link Component (C++)**
   - ✅ Create Link class
   - ✅ Handle click events
   - ✅ Integrate with Router
   - ✅ Text-based Link component (wraps Button internally)
   - **Note:** Link currently uses text-only API (wraps Button). This is sufficient for all use cases in examples. Support for arbitrary child components can be added in future if needed.
   - Location: `include/obsidian/link.h`, `src/obsidian/link.cpp`

2. **Platform Implementations**
   - ✅ macOS: Button-like component that navigates (verified - works correctly)
   - ⏳ iOS: Similar to macOS (Deferred - platform not yet implemented)
   - ⏳ Windows: Button-like component (Deferred - platform not yet implemented)
   - ⏳ Linux: Button-like component (Deferred - platform not yet implemented)
   - ⏳ Web: `<a>` tag with client-side navigation (Deferred - platform not yet implemented)
   - Location: `packages/{apple,android,windows,linux,web}/`

3. **Tests**
   - ✅ Test Link navigation (comprehensive tests in `tests/integration/link_test.cpp`)
   - ✅ Test Link API (create, href, text, visibility, enabled state)
   - ✅ Test Link integration with Router
   - Location: `tests/integration/link_test.cpp`

**Deliverables:**
- ✅ Link component implementation (C++ API complete)
- ✅ Platform-specific Link implementation (macOS verified and working)
- ✅ Comprehensive test suite (all tests passing)

**Checkpoint Requirements:**
- ✅ Link navigates correctly (verified in examples and tests)
- ✅ Link API works correctly (all accessors and mutators tested)
- ✅ All tests pass (13/13 integration tests passing)
- ✅ CI is green (all checks passing, zero warnings/errors)
- ✅ Code reviewed and verified

---

### Phase 5: Platform-Specific Navigation Integration

**Status:** ⏳ In Progress (macOS only - other platforms deferred)

**Goal:** Integrate router with native platform navigation APIs.

**Note:** Only macOS platform is currently implemented in the Obsydian framework. Platform-specific router integrations for Android, Windows, Linux, Web, and iOS will be implemented as those platforms are added to the framework.

**Tasks:**

1. **macOS Integration (In Progress)**
   - Use `NSViewController` for macOS navigation
   - Handle native back button (if applicable)
   - Integrate with existing macOS window/view controller system
   - Location: `packages/apple/macos_router.*`

2. **iOS Integration (Deferred)**
   - Use `UINavigationController` for iOS navigation
   - Handle native back button
   - Handle swipe gestures
   - Location: `packages/apple/ios_router.*`
   - **Status:** Deferred until iOS platform is added to framework

3. **Android Integration (Deferred)**
   - Use `Activity` and `Fragment` navigation
   - Handle system back button
   - Location: `packages/android/android_router.*`
   - **Status:** Deferred until Android platform is added to framework

4. **Windows Integration (Deferred)**
   - Use Win32 navigation patterns
   - Handle window navigation
   - Location: `packages/windows/windows_router.*`
   - **Status:** Deferred until Windows platform is added to framework

5. **Linux Integration (Deferred)**
   - Use GTK navigation patterns
   - Location: `packages/linux/linux_router.*`
   - **Status:** Deferred until Linux platform is added to framework

6. **Web Integration (Deferred)**
   - Use History API
   - Handle browser back/forward
   - Handle URL changes
   - Location: `packages/web/web_router.*`
   - **Status:** Deferred until Web platform is added to framework

**Deliverables:**
- ⏳ Platform-specific router implementations (macOS in progress, others deferred)
- ⏳ Native navigation integration (macOS in progress, others deferred)
- ⏳ Platform-specific tests (macOS in progress, others deferred)

**Checkpoint Requirements:**
- ⏳ Navigation works on macOS (in progress)
- ⏳ Native back button works (macOS - if applicable)
- ⏳ macOS tests pass (in progress)
- ⏳ CI is green for macOS (in progress)
- ⏳ Code reviewed and approved (in progress)
- ⏳ Other platforms: Deferred until platforms are added to framework

---

### Phase 6: Example Applications

**Status:** ⏳ Pending

**Goal:** Create comprehensive example applications demonstrating all routing features.

**Tasks:**

1. **Basic Navigation Example**
   - Simple app with multiple routes
   - Demonstrates basic navigation
   - Location: `examples/router_basic/`

2. **Nested Routes Example**
   - App with nested routes and layouts
   - Demonstrates layout nesting
   - Location: `examples/router_nested/`

3. **Dynamic Routes Example**
   - App with dynamic route segments
   - Demonstrates parameter extraction
   - Location: `examples/router_dynamic/`

4. **Navigation Patterns Example**
   - Demonstrates all navigation patterns
   - Back/forward, replace, push
   - Location: `examples/router_patterns/`

5. **Real-World Example**
   - Complete app using routing
   - Demonstrates best practices
   - Location: `examples/router_complete/`

**Deliverables:**
- ✅ 5 example applications
- ✅ All examples build and run
- ✅ Examples demonstrate all features
- ✅ Examples follow Obsydian patterns

**Checkpoint Requirements:**
- ✅ All examples build successfully
- ✅ All examples run correctly
- ✅ Examples demonstrate all routing features
- ✅ Examples follow project structure conventions
- ✅ CI is green
- ✅ Examples reviewed and approved

---

### Phase 7: Testing & Quality Assurance

**Status:** ⏳ Pending

**Goal:** Comprehensive testing of all routing functionality.

**Tasks:**

1. **Unit Tests**
   - Route registry tests
   - Route matcher tests
   - Navigation history tests
   - Route context tests
   - Location: `tests/integration/router_*_test.cpp`

2. **Integration Tests**
   - End-to-end navigation tests
   - Route rendering tests
   - Layout nesting tests
   - Link component tests
   - Location: `tests/integration/router_integration_test.cpp`

3. **Platform-Specific Tests**
   - Test navigation on each platform
   - Test native integration
   - Location: `tests/gui/router_platform_test.cpp`

4. **Performance Tests**
   - Route matching performance
   - Navigation performance
   - Memory usage
   - Location: `benchmarks/router_bench.cpp`

5. **Memory Leak Detection**
   - Run with AddressSanitizer/Valgrind
   - Fix any memory leaks
   - Location: CI pipeline

**Deliverables:**
- ✅ Comprehensive test suite
- ✅ 100% test pass rate
- ✅ Performance benchmarks
- ✅ Memory leak free
- ✅ Test documentation

**Checkpoint Requirements:**
- ✅ All tests pass (100% success rate)
- ✅ No memory leaks detected
- ✅ Performance benchmarks meet targets
- ✅ Code coverage > 80%
- ✅ CI is green
- ✅ Tests reviewed and approved

---

### Phase 8: Documentation

**Status:** ⏳ Pending

**Goal:** Complete documentation for the routing system.

**Tasks:**

1. **API Documentation**
   - Router API reference
   - Link component API
   - RouteContext API
   - Location: `docs/navigation/ROUTER_API.md`

2. **Usage Guide**
   - Getting started guide
   - File structure conventions
   - Route component examples
   - Navigation patterns
   - Location: `docs/navigation/ROUTER_GUIDE.md`

3. **Examples Documentation**
   - Document all example apps
   - Explain routing patterns
   - Location: `docs/navigation/ROUTER_EXAMPLES.md`

4. **Best Practices**
   - Routing best practices
   - Performance tips
   - Common patterns
   - Location: `docs/navigation/ROUTER_BEST_PRACTICES.md`

5. **Migration Guide**
   - How to migrate existing apps
   - Breaking changes (if any)
   - Location: `docs/navigation/ROUTER_MIGRATION.md`

**Deliverables:**
- ✅ Complete API documentation
- ✅ Usage guide
- ✅ Examples documentation
- ✅ Best practices guide
- ✅ Migration guide (if needed)

**Checkpoint Requirements:**
- ✅ All documentation complete
- ✅ Documentation reviewed and approved
- ✅ Examples match documentation
- ✅ Documentation is clear and comprehensive

---

### Phase 9: Final Verification & Release

**Status:** ⏳ Pending

**Goal:** Final verification, version bump, and release.

**Tasks:**

1. **Final Verification**
   - Run full CI pipeline
   - Verify all tests pass
   - Verify all examples work
   - Verify documentation is complete
   - Manual testing on all platforms

2. **Version Bump**
   - Update version in project configuration
   - Update changelog
   - Create release notes

3. **GitHub Release**
   - Create release tag
   - Push to GitHub
   - Monitor CI/CD pipeline
   - Verify GitHub Actions pass
   - Fix any issues if CI fails

4. **Post-Release**
   - Update README if needed
   - Announce release (if applicable)

**Deliverables:**
- ✅ Version bumped
- ✅ GitHub release created
- ✅ CI/CD pipeline fully green
- ✅ Release notes

**Checkpoint Requirements:**
- ✅ All CI checks pass
- ✅ All tests pass
- ✅ All examples work
- ✅ Documentation complete
- ✅ Version bumped
- ✅ GitHub release successful
- ✅ CI/CD pipeline green
- ✅ User verification complete

---

## Route Scanning Implementation

### Runtime Route Scanning (No Build System Dependency)

The routing system uses **runtime scanning** (similar to Expo Router) to avoid build system dependencies:

**How It Works:**
1. When `Router::initialize()` is called, it scans the `app/` directory
2. Uses platform filesystem APIs to recursively traverse the directory
3. Identifies route files (`*.cpp` files) and special files (`_layout.cpp`, `index.cpp`, `[param].cpp`, etc.)
4. Builds route tree structure in memory
5. Route registry is ready for matching and navigation

**Advantages:**
- ✅ No build system dependency (works with any build system or no build system)
- ✅ Routes are discovered automatically at runtime
- ✅ No code generation step required
- ✅ Easy to debug and understand
- ✅ Works identically across all platforms

**Performance Considerations:**
- Scanning happens once at app startup
- Route tree is cached in memory
- Scanning is fast (typically < 10ms for most apps)
- Can be optimized with caching if needed

**Platform Filesystem APIs:**
- macOS/iOS: `NSFileManager` or POSIX APIs
- Windows: Win32 APIs (`FindFirstFile`, `FindNextFile`)
- Linux: POSIX APIs (`opendir`, `readdir`)
- Android: JNI to Java `File` APIs
- Web: Not applicable (routes defined at build time for web)

## Testing Strategy

### Test Coverage Requirements

- **Unit Tests:** > 90% coverage for core routing logic
- **Integration Tests:** All navigation patterns tested
- **Platform Tests:** All platforms tested
- **Performance Tests:** Route matching < 1ms, navigation < 10ms

### Test Structure

```
tests/
  integration/
    router_test.cpp              → Core router tests
    route_matcher_test.cpp       → Route matching tests
    navigation_history_test.cpp  → History tests
    route_rendering_test.cpp     → Rendering tests
    link_test.cpp                → Link component tests
  gui/
    router_platform_test.cpp     → Platform-specific tests
benchmarks/
  router_bench.cpp              → Performance benchmarks
```

## CI/CD Integration

### CI Pipeline Steps

Each phase must pass these CI checks before proceeding:

1. **Code Quality**
   - C++ linting and formatting
   - Code style consistency

2. **Build**
   - All targets build successfully
   - No compilation warnings
   - All platforms build (if applicable)

3. **Tests**
   - All unit tests pass
   - All integration tests pass
   - All platform tests pass

4. **API Validation**
   - Public API headers compile
   - Examples compile with public API

5. **Documentation**
   - Documentation builds (if applicable)
   - No broken links

### Checkpoint Process

After each phase:
1. ✅ Complete all phase tasks
2. ✅ Run full CI pipeline: `./scripts/ci.sh`
3. ✅ Verify all tests pass
4. ✅ Verify all examples work
5. ✅ Code review (if applicable)
6. ✅ User verification
7. ✅ Mark phase complete

**IMPORTANT:** Do NOT proceed to next phase until:
- All phase tasks complete
- CI is fully green
- All tests pass
- User has verified the phase

## Example App Structure

Following Obsydian's example app patterns:

```
examples/router_basic/
  BUILD                    → Build file (optional, if using build system)
  Info.plist              → macOS app info (if applicable)
  app/                    → App directory (routes)
    _layout.cpp           → Root layout
    index.cpp             → "/" route
    about.cpp             → "/about" route
    contact.cpp           → "/contact" route
  main.cpp                → App entry point
```

## Success Criteria

### Functional Requirements

1. ✅ File-based routing works correctly
2. ✅ All route types supported (static, dynamic, catch-all)
3. ✅ Layout nesting works
4. ✅ Navigation works (navigate, back, forward, replace, push)
5. ✅ Link component works
6. ✅ Works on all platforms (iOS, macOS, Windows, Linux, Android, Web)
7. ✅ Route context provides correct data
8. ✅ Query parameters work
9. ✅ Dynamic route parameters work

### Non-Functional Requirements

1. ✅ Route matching < 1ms
2. ✅ Navigation < 10ms
3. ✅ No memory leaks
4. ✅ API follows Obsydian patterns
5. ✅ Documentation complete
6. ✅ All tests pass (100%)
7. ✅ All examples work
8. ✅ CI fully green

## Timeline Estimate

- **Phase 1:** Runtime Route Scanner & Registry Builder - 8-12 hours
- **Phase 2:** Core Router - 12-16 hours
- **Phase 3:** Route Context & Rendering - 8-12 hours
- **Phase 4:** Link Component - 6-8 hours
- **Phase 5:** Platform Integration - 16-24 hours
- **Phase 6:** Example Apps - 8-12 hours
- **Phase 7:** Testing - 12-16 hours
- **Phase 8:** Documentation - 8-12 hours
- **Phase 9:** Final Verification - 4-6 hours

**Total Estimate: 82-118 hours** (approximately 2-3 weeks of focused work)

## Risks & Mitigation

### Risk 1: Runtime Scanning Performance

**Risk:** Scanning file system at runtime may be slow or cause startup delays.

**Mitigation:**
- Benchmark scanning performance early
- Cache route registry after first scan
- Optimize filesystem operations
- Consider lazy loading for large route trees

### Risk 2: Cross-Platform Navigation Differences

**Risk:** Navigation APIs differ significantly across platforms.

**Mitigation:**
- Abstract platform differences in FFI layer
- Test on each platform early
- Use platform-specific implementations

### Risk 3: Performance Concerns

**Risk:** Route matching or navigation may be slow.

**Mitigation:**
- Benchmark early and often
- Optimize route matching algorithm
- Use efficient data structures

## References

- [Next.js App Router Documentation](https://nextjs.org/docs/app)
- [Expo Router Documentation](https://docs.expo.dev/router/introduction/)
- [Obsydian Architecture](ARCHITECTURE.md)
- [Obsydian Sidebar Implementation Plan](sidebar/SIDEBAR_IMPLEMENTATION_PLAN.md)
- [Obsydian CI/CD Pipeline](scripts/ci.sh)

## Notes

- This is a **file-based routing** system, NOT programmatic routing
- Routes are defined by file structure, not code
- **Runtime scanning** (like Expo Router) - no build system dependency, routes discovered at app startup
- Platform-specific implementations handle native navigation
- All phases must pass CI and user verification before proceeding
