# Obsidian Architecture

## Project Structure

```
obsidian/
├── MODULE.bazel              # Bazel module configuration with all dependencies
├── .bazelrc                   # Bazel build configuration
├── .bazelversion              # Bazel version pin (8.1.0)
├── BUILD                      # Root BUILD file
├── platforms/                 # Platform definitions (iOS, macOS, Android, Windows, Linux)
│   └── BUILD
├── core/                      # Core Obsidian runtime
│   ├── BUILD
│   ├── runtime/              # Main runtime (obsidian.h/cpp)
│   ├── ffi/                  # Platform FFI abstraction layer
│   ├── ui/                   # Cross-platform UI abstraction
│   └── compiler/             # Obsidian language compiler (future)
├── packages/                  # Obsidian standard library
│   ├── BUILD
│   ├── apple/                # Apple platform bindings (iOS/macOS)
│   ├── android/              # Android platform bindings
│   ├── windows/              # Windows platform bindings
│   └── linux/                # Linux platform bindings
├── cli/                       # create-obsidian-app CLI tool
│   ├── BUILD
│   ├── package.json
│   ├── tsconfig.json
│   └── src/
│       └── index.ts
└── tools/                     # Build tools and code generation
    └── BUILD
```

## Key Components

### 1. Core Runtime (`core/runtime/`)
- **obsidian.h/cpp**: Main runtime entry point
- Provides application lifecycle management
- HMR (Hot Module Reload) support
- Platform-agnostic application framework

### 2. FFI Layer (`core/ffi/`)
- **platform.h/cpp**: Platform detection and abstraction
- Unified interface for accessing native platform APIs
- Platform-specific implementations in `packages/`

### 3. Platform Packages (`packages/`)
Each platform package provides:
- **Apple** (`packages/apple/`): Swift/Objective-C interop via `objc_library` and `swift_library`
- **Android** (`packages/android/`): JNI bindings via `cc_library` and `kt_android_library`
- **Windows** (`packages/windows/`): Direct Win32 API access via `cc_library`
- **Linux** (`packages/linux/`): POSIX/system API access via `cc_library`

### 4. CLI Tool (`cli/`)
- TypeScript/Node.js tool for scaffolding new Obsidian apps
- Generates Bazel workspace structure
- Sets up platform-specific targets
- Configures HMR dev servers

## Build System: Bazel

### Why Bazel?
- **Multi-language**: Native support for C++, Swift, Kotlin, Java
- **Multi-platform**: Single build system for all platforms
- **Scalability**: Handles large monorepos efficiently
- **Caching**: Content-addressable caching for fast incremental builds
- **Extensibility**: Custom Starlark rules for Obsidian-specific needs

### Dependencies (MODULE.bazel)
- `rules_apple`: iOS/macOS builds
- `rules_swift`: Swift compilation
- `rules_android`: Android builds
- `rules_kotlin`: Kotlin compilation
- `rules_cc`: C/C++ compilation
- `rules_js`/`rules_ts`: CLI tooling

## Development Workflow

### Building
```bash
# Build everything
bazel build //...

# Build for specific platform
bazel build //... --config=macos
bazel build //... --config=ios
bazel build //... --config=android
```

### Creating a New App
```bash
# Using the CLI (when implemented)
npx create-obsidian-app@latest my-app --platforms macos ios
```

## Next Steps

1. **Implement Platform FFI Bindings**
   - Complete Apple platform interop
   - Complete Android JNI bindings
   - Complete Windows Win32 bindings
   - Complete Linux system bindings

2. **UI Abstraction Layer**
   - Implement platform-specific UI backends
   - Create unified component API
   - Add layout system

3. **HMR System**
   - Implement hot reload for each platform
   - Create dev server infrastructure
   - Add file watching and change detection

4. **CLI Tool**
   - Complete scaffolding logic
   - Generate Bazel workspace templates
   - Set up dev server orchestration

5. **Glass Integration**
   - Set up Glass to use Obsidian as local dependency
   - Test real-world usage
   - Iterate based on Glass's needs

## Glass Application

Glass is a real application at `/Users/naaiyy/Developer/Glass` that uses Obsidian. It serves as:
- First real-world test of Obsidian
- Reference implementation
- Driver for Obsidian's feature development

See [GLASS.md](./GLASS.md) for more details.

