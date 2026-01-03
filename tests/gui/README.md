# GUI Tests

These tests require a graphical environment (display server) to run. They test actual GUI functionality like window creation, which requires `NSApplication` and a connection to the window server.

## Running GUI Tests

### On a Machine with Display (Recommended)

```bash
# Run all GUI tests
bazel test //tests/gui:macos_window_test --config=macos --test_tag_filters=requires-gui

# Run specific GUI test
bazel test //tests/gui:macos_window_test --config=macos
```

### In CI with Display

If your CI environment has a display (e.g., macOS runners with GUI support):

```bash
bazel test //tests/gui/... --config=macos --test_tag_filters=requires-gui
```

### In Headless CI (Default)

GUI tests are **automatically skipped** in headless environments because they're tagged with:
- `requires-gui` - Requires a display server
- `manual` - Don't run automatically in CI

To run all tests except GUI tests (default CI behavior):
```bash
bazel test //tests/... --config=macos --test_tag_filters=-requires-gui
```

## Test Structure

- **Integration tests** (`tests/integration/`) - Run in headless CI, test non-GUI functionality
- **GUI tests** (`tests/gui/`) - Require display, run manually or in GUI-enabled CI

## Why Separate?

1. **NSApplication requires GUI**: macOS GUI operations need a display server
2. **CI compatibility**: Most CI environments are headless
3. **Industry practice**: Frameworks like Flutter, React Native separate GUI tests
4. **Clean separation**: Unit/integration tests run fast in CI, GUI tests run when needed

## Manual Testing

For manual verification of GUI functionality, run the hello_world example:

```bash
bazel run //examples/hello_world:hello_world_app --config=macos
```

This will create and display a window, allowing you to verify GUI functionality works correctly.

