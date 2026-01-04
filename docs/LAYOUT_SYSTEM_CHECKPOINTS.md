# Layout System Implementation Checkpoints

**IMPORTANT**: Each checkpoint must be 100% complete with all tests passing, builds working, and manual verification done before proceeding to the next checkpoint.

## Checkpoint Requirements (Apply to ALL Checkpoints)

### Mandatory Requirements
- ✅ All unit tests pass (`bazel test //tests/integration/...`)
- ✅ All GUI tests pass (`bazel test //tests/gui/... --test_tag_filters=requires-gui`)
- ✅ All builds succeed (`bazel build //...`)
- ✅ CI pipeline passes (`make ci` or `./scripts/ci.sh`)
- ✅ No linter errors
- ✅ No type errors
- ✅ No memory leaks (run with AddressSanitizer if possible)
- ✅ Example apps build and run successfully
- ✅ Manual GUI verification completed (for GUI features)
- ✅ Code follows existing patterns and style
- ✅ All public APIs documented in headers

### Code Quality Standards
- **Comments**: Minimal but essential. Only add comments for:
  - Non-obvious implementation details
  - Platform-specific quirks
  - Performance optimizations
  - Complex algorithms
  - Memory management decisions
- **No comment bloat**: Code should be self-documenting
- **Follow existing patterns**: Match the style of Button, Window, etc.

---

## CHECKPOINT 1: Foundation & Constraint Infrastructure

### Deliverables
1. Core layout constraint system
2. macOS NSLayoutConstraint FFI bindings
3. Basic constraint tests

### Files to Create
```
core/layout/
├── BUILD
├── constraint.h
├── constraint.cpp
├── layout_engine.h
├── layout_engine.cpp
└── alignment.h

packages/apple/
├── macos_layout.h
├── macos_layout.mm
├── macos_layout_ffi.h
└── macos_layout_ffi.cpp

tests/integration/
└── layout_constraint_test.cpp
```

### Files to Modify
```
packages/apple/BUILD (add layout targets)
core/BUILD (add layout targets)
```

### Implementation Tasks
1. Create `core/layout/constraint.h/cpp` - Constraint data structure
2. Create `core/layout/alignment.h` - Alignment enum and utilities
3. Create `packages/apple/macos_layout.h` - C interface for NSLayoutConstraint
4. Create `packages/apple/macos_layout.mm` - Objective-C++ implementation
5. Create `packages/apple/macos_layout_ffi.h/cpp` - C++ wrapper
6. Update BUILD files to include new targets
7. Write unit tests for constraint creation and validation

### Success Criteria
- [ ] Constraint can be created with all parameters
- [ ] Constraint can be activated/deactivated
- [ ] Constraint lifecycle is properly managed (no leaks)
- [ ] All unit tests pass
- [ ] All builds succeed
- [ ] CI passes
- [ ] No memory leaks detected

### Testing Requirements
- Unit tests for constraint creation
- Unit tests for constraint activation/deactivation
- Unit tests for constraint destruction
- Memory leak tests

### Manual Verification
- None required (no GUI yet)

### Ready for Next Checkpoint When
- All tests green
- CI green
- Code review ready
- No TODOs or FIXMEs

---

## CHECKPOINT 2: VStack Implementation

### Deliverables
1. Working VStack component
2. VStack unit tests
3. VStack GUI tests
4. Basic VStack example app

### Files to Create
```
include/obsidian/
└── vstack.h

src/obsidian/
└── vstack.cpp

tests/gui/
└── macos_vstack_test.cpp

examples/layout_showcase/
├── BUILD
├── Info.plist
└── main.cpp (basic VStack demo)
```

### Files to Modify
```
include/obsidian/obsidian.h (add vstack.h include)
src/obsidian/obsidian.cpp (if needed)
```

### Implementation Tasks
1. Create `include/obsidian/vstack.h` - Public API
2. Create `src/obsidian/vstack.cpp` - Implementation
3. Implement vertical stacking with spacing
4. Implement alignment (leading, center, trailing)
5. Implement padding support
6. Write unit tests
7. Write GUI tests
8. Create example app demonstrating VStack

### Success Criteria
- [ ] VStack arranges children vertically
- [ ] Spacing works correctly
- [ ] Alignment works correctly (leading, center, trailing)
- [ ] Padding works correctly
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] Example app builds and runs
- [ ] Manual GUI verification: Example app shows correct vertical layout
- [ ] All builds succeed
- [ ] CI passes

### Testing Requirements
- Unit tests for VStack API
- GUI tests for visual layout verification
- Example app with multiple buttons in VStack

### Manual Verification
**REQUIRED**: Launch example app and verify:
- Buttons are arranged vertically
- Spacing between buttons is correct
- Alignment is correct
- Padding is applied correctly
- Window resize updates layout correctly

### Ready for Next Checkpoint When
- All tests green
- CI green
- Manual GUI verification completed and approved
- Example app works perfectly
- Code review ready

---

## CHECKPOINT 3: HStack Implementation

### Deliverables
1. Working HStack component
2. HStack unit tests
3. HStack GUI tests
4. Updated example app with HStack

### Files to Create
```
include/obsidian/
└── hstack.h

src/obsidian/
└── hstack.cpp

tests/gui/
└── macos_hstack_test.cpp
```

### Files to Modify
```
include/obsidian/obsidian.h (add hstack.h include)
examples/layout_showcase/main.cpp (add HStack examples)
```

### Implementation Tasks
1. Create `include/obsidian/hstack.h` - Public API
2. Create `src/obsidian/hstack.cpp` - Implementation
3. Implement horizontal stacking with spacing
4. Implement vertical alignment (top, middle, bottom)
5. Reuse VStack logic, change axis
6. Write unit tests
7. Write GUI tests
8. Update example app with HStack demos

### Success Criteria
- [ ] HStack arranges children horizontally
- [ ] Spacing works correctly
- [ ] Vertical alignment works correctly (top, middle, bottom)
- [ ] Padding works correctly
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] Example app updated with HStack examples
- [ ] Manual GUI verification: Example app shows correct horizontal layout
- [ ] All builds succeed
- [ ] CI passes

### Testing Requirements
- Unit tests for HStack API
- GUI tests for visual layout verification
- Example app with HStack demos

### Manual Verification
**REQUIRED**: Launch example app and verify:
- Buttons are arranged horizontally
- Spacing between buttons is correct
- Vertical alignment is correct
- Padding is applied correctly
- Window resize updates layout correctly

### Ready for Next Checkpoint When
- All tests green
- CI green
- Manual GUI verification completed and approved
- Example app works perfectly
- Code review ready

---

## CHECKPOINT 4: ZStack Implementation

### Deliverables
1. Working ZStack component
2. ZStack unit tests
3. ZStack GUI tests
4. Updated example app with ZStack

### Files to Create
```
include/obsidian/
└── zstack.h

src/obsidian/
└── zstack.cpp

tests/gui/
└── macos_zstack_test.cpp
```

### Files to Modify
```
include/obsidian/obsidian.h (add zstack.h include)
examples/layout_showcase/main.cpp (add ZStack examples)
```

### Implementation Tasks
1. Create `include/obsidian/zstack.h` - Public API
2. Create `src/obsidian/zstack.cpp` - Implementation
3. Implement overlay stacking
4. Implement alignment-based positioning
5. Implement z-order management
6. Write unit tests
7. Write GUI tests
8. Update example app with ZStack demos

### Success Criteria
- [ ] ZStack overlays children correctly
- [ ] Alignment-based positioning works
- [ ] Z-order is correct (last child on top)
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] Example app updated with ZStack examples
- [ ] Manual GUI verification: Example app shows correct overlay layout
- [ ] All builds succeed
- [ ] CI passes

### Testing Requirements
- Unit tests for ZStack API
- GUI tests for visual overlay verification
- Example app with ZStack demos

### Manual Verification
**REQUIRED**: Launch example app and verify:
- Views are overlaid correctly
- Alignment positioning is correct
- Z-order is correct (last added is on top)
- Window resize updates layout correctly

### Ready for Next Checkpoint When
- All tests green
- CI green
- Manual GUI verification completed and approved
- Example app works perfectly
- Code review ready

---

## CHECKPOINT 5: Spacer & Padding Components

### Deliverables
1. Spacer component
2. Enhanced Padding support
3. Tests for Spacer
4. Updated example app

### Files to Create
```
include/obsidian/
└── spacer.h

src/obsidian/
└── spacer.cpp

tests/integration/
└── spacer_test.cpp
```

### Files to Modify
```
include/obsidian/obsidian.h (add spacer.h include)
examples/layout_showcase/main.cpp (add Spacer examples)
```

### Implementation Tasks
1. Create `include/obsidian/spacer.h` - Public API
2. Create `src/obsidian/spacer.cpp` - Implementation
3. Implement flexible spacing
4. Implement min/max size constraints
5. Write unit tests
6. Update example app with Spacer demos

### Success Criteria
- [ ] Spacer creates flexible space
- [ ] Spacer respects min/max constraints
- [ ] Spacer works in VStack, HStack
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] Example app updated with Spacer examples
- [ ] Manual GUI verification: Spacer works correctly
- [ ] All builds succeed
- [ ] CI passes

### Testing Requirements
- Unit tests for Spacer API
- GUI tests for Spacer in stacks
- Example app with Spacer demos

### Manual Verification
**REQUIRED**: Launch example app and verify:
- Spacer creates flexible space
- Spacer expands/contracts correctly
- Spacer respects constraints
- Window resize updates Spacer correctly

### Ready for Next Checkpoint When
- All tests green
- CI green
- Manual GUI verification completed and approved
- Example app works perfectly
- Code review ready

---

## CHECKPOINT 6: Advanced Features & Polish

### Deliverables
1. Dynamic layout updates
2. Window resize handling
3. Performance optimizations
4. Comprehensive documentation
5. Final example app

### Files to Modify
```
src/obsidian/vstack.cpp (add updateLayout, window resize handling)
src/obsidian/hstack.cpp (add updateLayout, window resize handling)
src/obsidian/zstack.cpp (add updateLayout, window resize handling)
examples/layout_showcase/main.cpp (comprehensive examples)
docs/LAYOUT_API.md (create)
docs/LAYOUT_EXAMPLES.md (create)
```

### Implementation Tasks
1. Implement `updateLayout()` for all stacks
2. Add window resize event handling
3. Implement constraint caching for performance
4. Implement batch constraint updates
5. Add layout invalidation system
6. Write comprehensive documentation
7. Create comprehensive example app
8. Performance benchmarks
9. Memory leak verification

### Success Criteria
- [ ] Layout updates when children added/removed
- [ ] Layout updates when window resizes
- [ ] Performance is acceptable (< 10ms for typical layouts)
- [ ] No memory leaks
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] Comprehensive example app works
- [ ] Documentation is complete
- [ ] Manual GUI verification: All features work correctly
- [ ] All builds succeed
- [ ] CI passes
- [ ] Benchmarks show acceptable performance

### Testing Requirements
- Unit tests for dynamic updates
- GUI tests for window resize
- Performance benchmarks
- Memory leak tests (valgrind/AddressSanitizer)
- Comprehensive example app

### Manual Verification
**REQUIRED**: Launch example app and verify:
- Adding/removing children updates layout correctly
- Window resize updates layout correctly
- All alignment options work
- All spacing options work
- All padding options work
- Nested stacks work correctly
- Complex layouts work correctly
- Performance feels smooth

### Ready for Release When
- All tests green
- CI green
- Manual GUI verification completed and approved
- Documentation complete
- Performance benchmarks acceptable
- No memory leaks
- Code review approved
- Ready for version bump to 0.2.0

---

## Final Checklist Before Release

### Code Quality
- [ ] All code follows existing patterns
- [ ] No comment bloat (only essential comments)
- [ ] All public APIs documented
- [ ] No TODOs or FIXMEs
- [ ] No compiler warnings
- [ ] No linter errors

### Testing
- [ ] All unit tests pass
- [ ] All GUI tests pass
- [ ] All integration tests pass
- [ ] Example apps work correctly
- [ ] Performance benchmarks pass
- [ ] Memory leak tests pass

### Build & CI
- [ ] All builds succeed
- [ ] CI pipeline passes
- [ ] All platforms build (macOS at minimum)
- [ ] No build errors or warnings

### Documentation
- [ ] API documentation complete
- [ ] Usage examples complete
- [ ] README updated (if needed)
- [ ] Example apps documented

### Manual Verification
- [ ] All GUI features manually verified
- [ ] Example apps manually tested
- [ ] Window resize tested
- [ ] Dynamic updates tested
- [ ] Complex layouts tested

### Release Preparation
- [ ] Version number updated (0.2.0)
- [ ] CHANGELOG updated (if exists)
- [ ] Release notes prepared
- [ ] Ready for merge to main

---

## Agent Instructions

### For Each Checkpoint

1. **Read the checkpoint requirements carefully**
2. **Implement only what's specified** - don't add extra features
3. **Test thoroughly** - all tests must pass
4. **Run CI locally** - `make ci` must pass
5. **Build example apps** - must build and run
6. **Manual verification** - launch GUI apps and verify visually
7. **Document as you go** - add essential comments only
8. **Follow existing patterns** - match Button, Window, etc.

### Code Style
- Match existing code style exactly
- Use pImpl pattern for public APIs
- Use RAII for memory management
- Follow existing naming conventions
- Keep comments minimal but essential

### Testing Philosophy
- Write tests for all new functionality
- GUI tests for visual verification
- Unit tests for logic
- Integration tests for interactions

### Before Moving to Next Checkpoint
- ✅ All tests pass
- ✅ CI passes
- ✅ Manual verification done (if required)
- ✅ Code review ready
- ✅ No TODOs or incomplete work

### If You Get Stuck
- Review existing code (Button, Window) for patterns
- Check documentation in `docs/`
- Ensure you understand NSLayoutConstraint API
- Test incrementally - don't implement everything at once

---

**Remember**: Each checkpoint must be 100% complete before moving to the next. Quality over speed.
