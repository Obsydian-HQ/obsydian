# Obsydian Renderer Architecture Plan

## The Problem

We've been building ad-hoc pieces instead of proper services. Every time we hit an issue, we create a tiny fix without understanding the full system:

1. Built router → realized we need layout
2. Built layout → realized we need lifecycle management
3. Built lifecycle → realized we need shadow tree
4. Built shadow tree → realized we need mounting manager
5. Built mounting → realized we need proper view registry with retention

**This stops now.**

React Native's Fabric architecture represents a decade of production experience. We should learn from it, not reinvent poorly.

---

## React Native's Architecture (What Works)

### Cross-Platform C++ (`ReactCommon/react/renderer/`)

```
renderer/
├── core/                   # ~70 files - Foundation
│   ├── ShadowNode.h/cpp    # Abstract tree node
│   ├── Props.h/cpp         # Component properties  
│   ├── State.h/cpp         # Component state
│   ├── EventEmitter.h/cpp  # Event handling
│   ├── EventDispatcher.h   # Event dispatch
│   ├── EventQueue.h/cpp    # Event queueing
│   ├── LayoutMetrics.h     # Layout output
│   ├── LayoutConstraints.h # Layout input
│   ├── LayoutableShadowNode.h  # Nodes that can be laid out
│   ├── RawProps.h/cpp      # Props before parsing
│   ├── ComponentDescriptor.h   # Component metadata
│   └── conversions.h       # Type conversions
│
├── mounting/               # ~25 files - Tree→Native
│   ├── ShadowTree.h/cpp    # The shadow tree structure
│   ├── ShadowView.h/cpp    # Lightweight view representation
│   ├── ShadowViewMutation.h/cpp  # Mutation operations
│   ├── Differentiator.h/cpp  # TREE DIFF ALGORITHM (56KB!)
│   ├── MountingCoordinator.h/cpp  # Coordinates commits
│   ├── MountingTransaction.h/cpp  # Atomic mutation batch
│   ├── ShadowTreeRegistry.h  # Multiple tree management
│   └── TelemetryController.h # Performance tracking
│
├── componentregistry/      # ~10 files - Component Types
│   ├── ComponentDescriptorRegistry.h/cpp
│   ├── ComponentDescriptorProvider.h
│   └── ComponentDescriptorFactory.h
│
├── components/             # Per-component shadow nodes
│   ├── view/               # Base view component
│   ├── text/               # Text rendering
│   ├── image/              # Image loading
│   ├── scrollview/         # Scroll container
│   └── textinput/          # Text input
│
├── scheduler/              # UI update scheduling
├── graphics/               # Color, Size, Rect primitives
└── css/                    # CSS parsing
```

### Platform-Specific (`React/Fabric/` for iOS/macOS)

```
Fabric/
├── Mounting/
│   ├── RCTComponentViewRegistry.h/mm    # Tag→View mapping, RETAINS views
│   ├── RCTComponentViewFactory.h/mm     # Creates view instances
│   ├── RCTComponentViewProtocol.h       # Protocol ALL views implement
│   ├── RCTComponentViewDescriptor.h     # View + metadata
│   ├── RCTMountingManager.h/mm          # Applies mutations to native
│   ├── UIView+ComponentViewProtocol.mm  # Default protocol impl
│   └── ComponentViews/
│       ├── View/
│       │   └── RCTViewComponentView.h/mm
│       ├── Switch/
│       │   └── RCTSwitchComponentView.h/mm
│       ├── ScrollView/
│       │   └── RCTScrollViewComponentView.h/mm
│       └── ...
│
├── Surface/                # Rendering surface management
│   ├── RCTSurfacePresenter.h/mm
│   └── RCTSurfaceRegistry.h/mm
│
└── RCTScheduler.h/mm       # Platform event beat
```

---

## Key Insights from React Native

### 1. **Views ARE Components** (No Wrappers)

```objc
// BAD (what we did)
@interface ObsidianButtonWrapper : NSObject
@property (strong) NSButton *button;  // Wrapper holds button
@end

// GOOD (React Native pattern)
@interface OBSButtonComponentView : NSView <OBSComponentViewProtocol>
// The view IS the component. No wrapper.
@end
```

### 2. **Registry OWNS Views with Strong References**

```objc
// React Native's RCTComponentViewDescriptor
class RCTComponentViewDescriptor {
    __strong UIView<RCTComponentViewProtocol> *const view;  // STRONG reference!
};

// Registry stores descriptors in unordered_map
std::unordered_map<Tag, RCTComponentViewDescriptor> _registry;
// The __strong in the struct ensures ARC retains the view
```

### 3. **Protocol Defines Lifecycle**

```objc
@protocol RCTComponentViewProtocol
// Called to mount child view
- (void)mountChildComponentView:(UIView *)childView index:(NSInteger)index;

// Called to unmount child view  
- (void)unmountChildComponentView:(UIView *)childView index:(NSInteger)index;

// Update properties
- (void)updateProps:(Props::Shared)props oldProps:(Props::Shared)oldProps;

// Update layout
- (void)updateLayoutMetrics:(LayoutMetrics)metrics oldMetrics:(LayoutMetrics)old;

// Called before recycling
- (void)prepareForRecycle;
@end
```

### 4. **Mutations are Atomic Transactions**

```cpp
// ShadowViewMutation - one atomic operation
struct ShadowViewMutation {
    enum Type { Create, Delete, Insert, Remove, Update };
    Type type;
    Tag parentTag;
    ShadowView oldChildShadowView;
    ShadowView newChildShadowView;
    int index;
};

// MountingTransaction - batch of mutations
class MountingTransaction {
    ShadowViewMutationList mutations;
    // Applied atomically
};
```

### 5. **Differentiator is Complex** (56KB!)

The tree diff algorithm is NOT trivial. React Native's `Differentiator.cpp` is 56KB because:
- Handles reparenting
- Handles reordering  
- Generates minimal mutation set
- Handles flattening/unflattening
- Handles view recycling

---

## Proposed Obsydian Architecture

### Directory Structure

```
obsydian/
├── renderer/                    # Cross-platform C++ (like ReactCommon/renderer)
│   ├── core/                    # Foundation types
│   │   ├── ShadowNode.h/cpp
│   │   ├── Props.h/cpp
│   │   ├── State.h/cpp
│   │   ├── EventEmitter.h/cpp
│   │   ├── LayoutMetrics.h/cpp
│   │   └── ComponentDescriptor.h/cpp
│   │
│   ├── mounting/                # Tree→Native bridge
│   │   ├── ShadowTree.h/cpp
│   │   ├── ShadowView.h/cpp
│   │   ├── ShadowViewMutation.h/cpp
│   │   ├── Differentiator.h/cpp
│   │   ├── MountingCoordinator.h/cpp
│   │   └── MountingTransaction.h/cpp
│   │
│   ├── componentregistry/       # Component type registry
│   │   ├── ComponentDescriptorRegistry.h/cpp
│   │   └── ComponentDescriptorProvider.h
│   │
│   ├── components/              # Component shadow nodes
│   │   ├── view/
│   │   ├── button/
│   │   ├── text/
│   │   └── stack/
│   │
│   └── layout/                  # Layout engine (Yoga equivalent)
│       ├── LayoutEngine.h/cpp
│       └── Style.h/cpp
│
├── platform/                    # Platform-specific code
│   ├── apple/
│   │   ├── Fabric/              # Like React/Fabric
│   │   │   ├── Mounting/
│   │   │   │   ├── OBSComponentViewRegistry.h/mm
│   │   │   │   ├── OBSComponentViewFactory.h/mm
│   │   │   │   ├── OBSComponentViewProtocol.h
│   │   │   │   ├── OBSComponentViewDescriptor.h
│   │   │   │   ├── OBSMountingManager.h/mm
│   │   │   │   ├── NSView+ComponentViewProtocol.h/mm
│   │   │   │   └── ComponentViews/
│   │   │   │       ├── Button/
│   │   │   │       │   └── OBSButtonComponentView.h/mm
│   │   │   │       ├── Text/
│   │   │   │       │   └── OBSTextComponentView.h/mm
│   │   │   │       ├── Stack/
│   │   │   │       │   ├── OBSVStackComponentView.h/mm
│   │   │   │       │   └── OBSHStackComponentView.h/mm
│   │   │   │       └── ...
│   │   │   │
│   │   │   └── Surface/
│   │   │       └── OBSSurfacePresenter.h/mm
│   │   │
│   │   └── Components/          # High-level component wrappers (if needed)
│   │
│   ├── android/                 # Future
│   └── windows/                 # Future
│
├── runtime/                     # App initialization
│   └── ObsydianRuntime.h/cpp
│
├── include/                     # Public C++ API
│   └── obsidian/
│       ├── Button.h
│       ├── Text.h
│       └── ...
│
└── src/                         # C++ component implementations
    └── obsidian/
        ├── Button.cpp
        └── ...
```

### Migration Path

**Phase 1: Foundation**
1. Create `renderer/core/` with proper ShadowNode, Props, State
2. Create `renderer/mounting/` with ShadowTree, ShadowView, Differentiator
3. Create `platform/apple/Fabric/Mounting/` with proper ComponentViewProtocol

**Phase 2: Components**
4. Create ComponentViews for each component type
5. Migrate existing components to new architecture
6. Remove old wrapper patterns

**Phase 3: Integration**
7. Wire up MountingManager to process mutations
8. Implement proper view recycling
9. Add telemetry/debugging

---

## Immediate Next Steps

1. **Create `renderer/core/ShadowNode.h`** - Study React Native's ShadowNode.h (10KB)
2. **Create `renderer/mounting/ShadowViewMutation.h`** - Copy the mutation types exactly
3. **Create `platform/apple/Fabric/Mounting/OBSComponentViewProtocol.h`** - Define the protocol
4. **Create `OBSComponentViewRegistry`** - With proper `__strong` ARC retention

---

## References

- React Native Fabric: `facebook/react-native/packages/react-native/ReactCommon/react/renderer/`
- iOS Mounting: `facebook/react-native/packages/react-native/React/Fabric/Mounting/`
- Key files to study:
  - `ShadowNode.h` (10KB) - Core abstraction
  - `Differentiator.cpp` (56KB) - Tree diff algorithm  
  - `RCTMountingManager.mm` (13KB) - Applies mutations
  - `RCTComponentViewRegistry.mm` (4KB) - View storage with retention
