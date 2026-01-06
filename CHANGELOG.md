# Changelog

All notable changes to the Obsidian Framework will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- **BREAKING**: Reorganized project structure for better modularity
  - Renamed `src/` → `ui/` with `components/`, `containers/`, `navigation/` subdirectories
  - Renamed `packages/` → `platform/` with `apple/macos/`, `apple/ios/` structure
  - Created top-level `system/` directory for non-UI APIs
- Reorganized public headers into logical categories:
  - `include/obsidian/ui/` - UI components (Button, TextField, etc.)
  - `include/obsidian/layout/` - Layout components (VStack, HStack, etc.)
  - `include/obsidian/navigation/` - Routing system
  - `include/obsidian/system/` - System APIs (Process, FileSystem, Network)
  - `include/obsidian/style/` - Styling system (placeholder)
- Moved examples to separate repository: [obsydian-examples](https://github.com/Obsydian-HQ/obsydian-examples)
- Renamed `Fabric` renderer to `renderer` in `platform/apple/renderer/`

### Added
- Styling system placeholder API (`include/obsidian/style/`)
- FileSystem API placeholder (`include/obsidian/system/file_system.h`)
- Network API placeholder (`include/obsidian/system/network.h`)
- CONTRIBUTING.md with development guidelines
- This CHANGELOG.md file

### Removed
- All tests (to be re-implemented with proper strategy)
- Benchmarks (to be re-implemented)

## [2.2.1] - 2025-12-XX

### Added
- Shadow Tree architecture for efficient UI reconciliation
- Renderer/Mounting system for native view management
- File-based routing system
- Sidebar navigation component
- SplitView container
- Process management APIs

### Changed
- Improved layout engine
- Better constraint handling

## [2.0.0] - 2025-XX-XX

### Added
- Initial public release
- Core UI components (Button, TextField, TextView, etc.)
- Layout containers (VStack, HStack, ZStack, ScrollView)
- Window management
- macOS platform support

---

[Unreleased]: https://github.com/Obsydian-HQ/obsydian/compare/v2.2.1...HEAD
[2.2.1]: https://github.com/Obsydian-HQ/obsydian/releases/tag/v2.2.1
[2.0.0]: https://github.com/Obsydian-HQ/obsydian/releases/tag/v2.0.0
