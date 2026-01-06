# Obsydian

**A high-performance cross-platform native app framework using C++ with direct FFI bindings.**

Build native applications for iOS, macOS, Android, Windows, and Linux from a single C++ codebase.

## Why Obsydian?

Unlike React Native or Flutter which add layers of abstraction, Obsydian provides:

- **Direct Native Access** - C++ talks directly to platform APIs via FFI
- **Zero Bridge Overhead** - No JavaScript engine, no serialization
- **Compile-Time Safety** - Full C++ type checking
- **Deterministic Performance** - No garbage collector pauses
- **Small Binaries** - No runtime dependencies

## Architecture

```
Your App (C++)
     │
     ▼
┌─────────────────────────────┐
│     Obsydian Public API      │  include/obsidian/
│  (obsidian::Window, Button)  │
└─────────────────────────────┘
     │
     ▼
┌─────────────────────────────┐
│       Platform FFI           │  packages/{apple,android,windows,linux}/
│    (C interface layer)       │
└─────────────────────────────┘
     │
     ▼
┌─────────────────────────────┐
│      Native Platform         │  NSWindow, UIView, HWND, etc.
│         APIs                 │
└─────────────────────────────┘
```

## Quick Start

```bash
# Create a new Obsydian app
npx create-obsydian-app@latest my-app --platforms macos

# Build with Bazel
cd my-app
bazel build //... --config=macos

# Or open in Xcode
open my-app.xcodeproj
```

## Example

```cpp
#include <obsidian/obsidian.h>

int main() {
    obsidian::App app;
    
    // Create a window
    obsidian::Window window;
    window.create("Hello Obsydian", 800, 600);
    
    // Create a button
    obsidian::Button button;
    button.create("Click Me!", 100, 100, 150, 40);
    button.setOnClick([]() {
        std::cout << "Button clicked!" << std::endl;
    });
    button.addToWindow(window);
    
    // Run the app
    app.run();
    return 0;
}
```

## Platform Support

| Platform | Status | Bindings |
|----------|--------|----------|
| macOS    | ✅ Working | Objective-C++ / AppKit |
| iOS      | 🚧 In Progress | Objective-C++ / UIKit |
| Android  | 📋 Planned | JNI / Android SDK |
| Windows  | 📋 Planned | Win32 API |
| Linux    | 📋 Planned | GTK / X11 |

## Building from Source

### Prerequisites

- Bazel 8.1.0+
- C++20 compatible compiler
- Platform SDK (Xcode for macOS/iOS)

### Build

```bash
# Clone the repository
git clone https://github.com/Obsydian-HQ/obsydian.git
cd obsydian

# Build everything
bazel build //...

# Run tests
bazel test //...

# Run an example
bazel run //examples/hello_world:hello_world_app --config=macos
```

## Project Structure

```
obsydian/
├── include/obsidian/      # Public API headers
├── src/                   # Public API implementation
├── core/
│   ├── runtime/           # Application lifecycle
│   └── ffi/              # Platform abstraction
├── packages/
│   ├── apple/            # macOS/iOS bindings
│   ├── android/          # Android bindings (planned)
│   ├── windows/          # Windows bindings (planned)
│   └── linux/            # Linux bindings (planned)
├── examples/             # Example applications
├── tests/               # Test suites
└── docs/               # Documentation
```

## Related Projects

- [create-obsydian-app](https://github.com/Obsydian-HQ/create-obsydian-app) - CLI scaffolding tool
- [obsydian-devtools](https://github.com/Obsydian-HQ/obsydian-devtools) - Developer tools

## License

MIT License - see [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.
