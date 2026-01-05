# macOS Sidebar Implementation Plan

## Executive Summary

This document outlines the implementation plan for adding native macOS sidebar support to the Obsidian framework. The sidebar will leverage `NSSplitView` and `NSSplitViewController` to provide a production-ready, native macOS sidebar with collapse/expand, resizing, and content customization capabilities.

## Implementation Status

**Completed Phases:**
- ✅ **Phase 0:** Prerequisites - Added `getNativeViewHandle()` to VStack, HStack, List, and ScrollView
- ✅ **Phase 1:** Foundation (C FFI Layer) - `macos_splitview.h` and `macos_splitview.mm` implemented and building
- ✅ **Phase 2:** C++ FFI Wrapper - `obsidian::ffi::macos::SplitView` class implemented in `macos_ffi.cpp`
- ✅ **Phase 3:** Public API - `splitview.h` and `splitview.cpp` implemented
- ✅ **Phase 4:** Build System - BUILD files updated, compiles successfully

**Remaining Phases:**
- ⏳ **Phase 5:** Example Application
- ⏳ **Phase 6:** Testing
- ⏳ **Phase 7:** Documentation

**Key Implementation Notes:**
- All NSSplitView API calls validated against actual macOS APIs (no guessing)
- Fixed compilation errors related to `insertSubview:atIndex:` and `positionOfDividerAtIndex:` (these methods don't exist on NSSplitView)
- Used frame-based calculations for sidebar width and collapsed state detection
- Proper view ordering achieved through remove/re-add pattern with `addSubview:`
- Build verified: `//packages/apple:apple_objc_bridge` compiles successfully with no errors or warnings

## Architecture Overview

Following Obsidian's established architecture pattern:

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

## Research Findings

### Native macOS APIs

1. **NSSplitView** (Foundation)
   - Creates resizable split views with dividers
   - Supports horizontal and vertical orientations
   - Handles collapse/expand behavior
   - Minimum and maximum sizes for panes
   - Divider styling and behavior

2. **NSSplitViewController** (macOS 10.11+)
   - Higher-level controller for split views
   - Better state management
   - Automatic sidebar behavior support
   - Integration with auto layout

3. **NSOutlineView** (Optional, for hierarchical sidebar content)
   - Already implemented in Obsidian as `List` component
   - Can be used inside sidebar pane

### Key Features to Support

1. **Native Behaviors**
   - Collapse/expand sidebar pane
   - Resizable divider (drag to resize)
   - Minimum/maximum sidebar widths
   - Automatic collapse on small windows (optional)
   - Keyboard shortcuts (Cmd+Option+S for sidebar toggle)

2. **Content Support**
   - Any Obsidian component (VStack, HStack, Button, List, etc.)
   - Custom views via public API
   - Scrollable content (ScrollView integration)

3. **Configuration**
   - Initial sidebar width
   - Minimum sidebar width (default: ~150pt)
   - Maximum sidebar width (default: ~400pt)
   - Sidebar position (leading/trailing)
   - Collapse behavior (auto/manual)

## API Design

### Public API (C++)

```cpp
namespace obsidian {

class SplitView {
public:
    SplitView();
    ~SplitView();
    
    // Creation
    bool create(SidebarPosition position = SidebarPosition::Leading);
    
    // Sidebar management
    void setSidebarWidth(double width);
    double getSidebarWidth() const;
    void setMinimumSidebarWidth(double width);
    void setMaximumSidebarWidth(double width);
    
    // Collapse/expand
    void collapseSidebar();
    void expandSidebar();
    void toggleSidebar();
    bool isSidebarCollapsed() const;
    
    // Content management
    void setSidebarContent(VStack& vstack);
    void setSidebarContent(HStack& hstack);
    void setSidebarContent(List& list);
    void setSidebarContent(Button& button);
    // ... other component types
    
    void setMainContent(VStack& vstack);
    void setMainContent(HStack& hstack);
    // ... other component types
    
    // Window integration
    void addToWindow(Window& window);
    void removeFromParent();
    
    // State
    bool isValid() const;
    
    // Callbacks
    void setOnSidebarToggle(std::function<void(bool collapsed)> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

enum class SidebarPosition {
    Leading,   // Left side (LTR)
    Trailing   // Right side (LTR)
};

} // namespace obsidian
```

## Implementation Steps

### Phase 0: Prerequisites - Add getNativeViewHandle() to Components ✅

1. ✅ **Add `getNativeViewHandle()` to VStack**
   - Added method declaration to `include/obsidian/vstack.h`
   - Implemented in `src/obsidian/vstack.cpp` using `obsidian_macos_vstack_get_view`

2. ✅ **Add `getNativeViewHandle()` to HStack**
   - Added method declaration to `include/obsidian/hstack.h`
   - Implemented in `src/obsidian/hstack.cpp` using `obsidian_macos_hstack_get_view`

3. ✅ **Add `getNativeViewHandle()` to List**
   - Added method declaration to `include/obsidian/list.h`
   - Implemented in `src/obsidian/list.cpp` using `obsidian_macos_list_get_view`

4. ✅ **Add `getNativeViewHandle()` to ScrollView**
   - Added method declaration to `include/obsidian/scrollview.h`
   - Implemented in `src/obsidian/scrollview.cpp` using `obsidian_macos_scrollview_get_view`

5. ✅ **Update SplitView to use getNativeViewHandle()**
   - Updated all `setSidebarContent()` methods to use `getNativeViewHandle()`
   - Updated all `setMainContent()` methods to use `getNativeViewHandle()`

### Phase 1: Foundation (C FFI Layer) ✅

1. ✅ **Create `macos_splitview.h` C FFI Interface**
   - Created C-compatible header with opaque handle type
   - Defined `ObsidianSplitViewHandle`, `ObsidianSidebarPosition` enum, and `ObsidianSplitViewParams` struct
   - Declared all C FFI functions for SplitView management
   - Location: `packages/apple/macos_splitview.h`

2. ✅ **Implement `macos_splitview.mm`**
   - Created `ObsidianSplitViewWrapper` Objective-C class
   - Uses `NSSplitView` for the split view with vertical orientation (horizontal divider)
   - Implemented sidebar pane management with proper view ordering
   - Handles collapse/expand logic using `setSubview:isCollapsed:` and frame-based calculations
   - Supports minimum/maximum widths via delegate methods
   - Implemented divider behavior with NSSplitViewDelegate protocol
   - Handles view hierarchy management with Auto Layout constraints
   - **Important fixes applied:**
     - Fixed `insertSubview:atIndex:` usage (replaced with `addSubview:` and proper view reordering)
     - Fixed `positionOfDividerAtIndex:` usage (replaced with frame-based collapsed state detection)
     - Removed unused variable warnings
   - Location: `packages/apple/macos_splitview.mm`
   - **Build Status:** ✅ Compiles successfully with no errors or warnings

### Phase 2: C++ FFI Wrapper ✅

3. ✅ **Update `macos_ffi.h` and `macos_ffi.cpp`**
   - Added `obsidian::ffi::macos::SplitView` class to `macos_ffi.h`
   - Implemented C++ wrapper in `macos_ffi.cpp` (starting at line ~965)
   - Wrapped all C FFI functions in C++ interface with RAII pattern
   - Handles memory management properly (automatic cleanup in destructor)
   - Location: `packages/apple/macos_ffi.h` and `packages/apple/macos_ffi.cpp`

### Phase 3: Public API ✅

4. ✅ **Create `include/obsidian/splitview.h`**
   - Defined public `obsidian::SplitView` class
   - Defined `SidebarPosition` enum (Leading/Trailing)
   - Documented API with comprehensive comments
   - Includes all necessary methods for sidebar management
   - Location: `include/obsidian/splitview.h`

5. ✅ **Implement `src/obsidian/splitview.cpp`**
   - Implemented public API using FFI wrapper
   - Handles component type conversions (VStack, HStack, List, Button, ScrollView)
   - Manages lifecycle with RAII pattern
   - Implements all setSidebarContent/setMainContent overloads using `getNativeViewHandle()`
   - Location: `src/obsidian/splitview.cpp`

### Phase 4: Build System ✅

6. ✅ **Update BUILD files**
   - Added `macos_splitview.h` to `apple_c_headers` in `packages/apple/BUILD`
   - Added `macos_splitview.mm` to `apple_objc_bridge` srcs in `packages/apple/BUILD`
   - Added `macos_splitview.h` to `apple_objc_bridge` hdrs in `packages/apple/BUILD`
   - Public API BUILD files (`include/BUILD` and `src/BUILD`) use `glob()` patterns, so `splitview.h` and `splitview.cpp` are automatically included
   - Added `#include "obsidian/splitview.h"` to `include/obsidian/obsidian.h` for public API integration
   - **Build Status:** ✅ All targets build successfully (`//include:obsidian_headers`, `//src:obsidian_impl`, `//packages/apple:apple_objc_bridge`)

### Phase 5: Example Application

7. **Create `examples/sidebar_showcase/`**
   - Demonstrate sidebar with VStack containing buttons
   - Show List component in sidebar
   - Demonstrate collapse/expand
   - Show main content area with multiple components
   - Test resizing behavior

### Phase 6: Testing

8. **Create `tests/gui/macos_splitview_test.cpp`**
   - Test creation and destruction
   - Test sidebar width management
   - Test collapse/expand
   - Test content setting
   - Test window integration
   - Test minimum/maximum width constraints

### Phase 7: Documentation

9. **Write comprehensive documentation**
   - API reference documentation
   - Usage examples
   - Best practices guide
   - Integration with other components

## Technical Details

### NSSplitView Configuration

```objc
// Orientation: Vertical for left/right split (sidebar + main content)
NSSplitView* splitView = [[NSSplitView alloc] initWithFrame:frame];
[splitView setVertical:YES];  // Vertical = horizontal divider (left/right)

// Divider style
[splitView setDividerStyle:NSSplitViewDividerStyleThin];

// Delegate for collapse behavior
[splitView setDelegate:self];

// Minimum sizes
[[splitView arrangedSubviews][0] setMinSize:150.0];  // Sidebar min width
[[splitView arrangedSubviews][1] setMinSize:300.0];  // Main content min width
```

### Memory Management

- Use ARC (Automatic Reference Counting) in Objective-C++ code
- Use `__bridge_retained` / `__bridge_transfer` for C interop
- Follow RAII pattern in C++ code
- Ensure proper cleanup in destructors

### Auto Layout Integration

- Use Auto Layout constraints for proper sizing
- Respect intrinsic content sizes of child views
- Handle window resizing gracefully
- Support dynamic content changes

## Testing Strategy

### Unit Tests
- Creation/destruction
- Width management
- Collapse/expand state
- Content management

### Integration Tests
- Window integration
- Component composition (VStack, List, etc.)
- Resize behavior
- Minimum/maximum width enforcement

### Manual Testing
- Visual verification of native appearance
- Drag-to-resize behavior
- Keyboard shortcuts
- Multiple sidebar instances

## Success Criteria

1. ✅ All tests pass (100% green)
2. ✅ Example app demonstrates all features
3. ✅ Documentation is complete
4. ✅ API follows Obsidian patterns
5. ✅ Native macOS behaviors work correctly
6. ✅ Memory leaks checked (valgrind/Instruments)
7. ✅ Performance acceptable (no layout lag)

## Timeline Estimate

- Phase 1-2 (FFI Layer): 2-3 hours
- Phase 3 (Public API): 2-3 hours
- Phase 4 (Build System): 30 minutes
- Phase 5 (Example App): 1-2 hours
- Phase 6 (Testing): 2-3 hours
- Phase 7 (Documentation): 1-2 hours

**Total: 9-14 hours**

## Future Enhancements

- Support for multiple sidebars (leading + trailing)
- Animated collapse/expand
- Sidebar state persistence (user defaults)
- Custom divider styling
- Three-pane layouts (sidebar + main + inspector)

## References

- [Apple NSSplitView Documentation](https://developer.apple.com/documentation/appkit/nssplitview)
- [Apple NSSplitViewController Documentation](https://developer.apple.com/documentation/appkit/nssplitviewcontroller)
- Existing Obsidian components (VStack, HStack, List) for API patterns
- macOS Human Interface Guidelines - Sidebars section