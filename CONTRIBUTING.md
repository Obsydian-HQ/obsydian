# Contributing to Obsidian

Thank you for your interest in contributing to the Obsidian Framework!

## Getting Started

### Prerequisites

- **Bazel 8.1.0+** - Build system
- **Xcode 15+** - For macOS/iOS development
- **C++20 compatible compiler**

### Setup

```bash
# Clone the repository
git clone https://github.com/Obsydian-HQ/obsydian.git
cd obsydian

# Verify Bazel is installed
bazel version

# Build everything
bazel build //...

# Run CI checks
make ci
```

## Project Structure

```
obsydian/
├── core/           # Framework core (runtime, layout, routing, shadow tree)
├── ui/             # UI layer (components, containers, navigation)
├── system/         # System APIs (process, filesystem, network)
├── platform/       # Platform bindings (apple/macos, android, etc.)
├── include/        # Public API headers
├── docs/           # Documentation
└── scripts/        # Build and CI scripts
```

## Development Workflow

### Making Changes

1. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**
   - Follow the existing code style
   - Add appropriate documentation
   - Keep changes focused and atomic

3. **Build and test**
   ```bash
   make ci
   ```

4. **Submit a pull request**
   - Describe your changes clearly
   - Reference any related issues

### Code Style

- **C++20** standard
- Use `obsidian::` namespace
- Follow existing patterns in the codebase
- Document public APIs with Doxygen-style comments

### Architecture Guidelines

#### Public API (`include/obsidian/`)
- Headers only - no implementation
- Must be self-contained (no internal includes)
- Organized by category: `ui/`, `layout/`, `navigation/`, `system/`, `style/`

#### Implementation (`ui/`, `system/`)
- Implementations reference public headers
- Use PIMPL pattern for ABI stability
- Platform-specific code goes in `platform/`

#### Platform Bindings (`platform/`)
- Each platform has its own subdirectory
- Implement the FFI layer for platform-specific APIs
- Follow platform conventions (AppKit for macOS, etc.)

## Submitting Issues

- Use the issue templates
- Provide minimal reproducible examples
- Include system information (OS, Bazel version, etc.)

## Code of Conduct

Be respectful and constructive. We're all here to build something great.

## License

By contributing, you agree that your contributions will be licensed under the project's license.
