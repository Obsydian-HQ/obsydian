# CI/CD Pipeline Documentation

## Overview

Obsidian uses a comprehensive CI/CD pipeline that can be run locally during development and automatically in GitHub Actions. The pipeline ensures code quality, API validity, performance regression detection, and build validation across all platforms.

**Quick Start**: Run `make ci` to execute the complete pipeline locally.

## Quick Start

### Local Development

Run the complete CI/CD pipeline with a single command:

```bash
make ci
```

Or use the script directly:

```bash
./scripts/ci.sh
```

### What Gets Checked

The pipeline runs the following checks:

1. **Code Quality**
   - Bazel file formatting (buildifier)
   - TypeScript/JavaScript linting (CLI tool)
   - C++ syntax validation (via compilation)

2. **Build Validation**
   - Builds all targets (`//...`)
   - Builds all example applications
   - Builds CLI tool
   - Validates builds for all platforms

3. **Testing**
   - Unit tests (`//core/...`)
   - Integration tests (`//tests/integration/...`)
   - GUI tests (optional, requires display)
   - All tests exclude GUI tests by default for CI compatibility

4. **API Validation**
   - Validates public API headers compile independently
   - **Verifies all public headers are exposed in obsidian.h** (catches missing includes)
   - Ensures examples can compile with only public API
   - Validates API is self-contained (no internal dependencies leak)
   - Ensures API stability and compatibility

5. **Performance Benchmarks** (optional)
   - Runs performance regression tests
   - Compares against baseline metrics

## Usage

### Basic Usage

```bash
# Run full pipeline (recommended for development)
make ci

# Run with verbose output
make ci-verbose

# Run everything including GUI tests
make ci-full
```

### Advanced Options

```bash
# Run specific checks only
make build          # Build only
make test           # Test only
make quality        # Code quality only

# Platform-specific builds
make build-macos    # Build for macOS only
make build-ios      # Build for iOS only

# Include GUI tests (requires display)
./scripts/ci.sh --gui-tests

# Run performance benchmarks
./scripts/ci.sh --benchmarks

# Build for specific platform
./scripts/ci.sh --platform macos

# Skip specific steps
./scripts/ci.sh --skip-build    # Skip building
./scripts/ci.sh --skip-tests    # Skip testing
./scripts/ci.sh --skip-quality  # Skip quality checks
```

## How Real Teams Do CI/CD

### Industry Best Practices

**Local-First Development**: Run the same checks locally that run in CI. This catches issues before pushing, provides faster feedback, and reduces CI failures.

**Layered Testing Strategy**: Organize tests by speed and importance:
- **Fast checks** (every commit): Formatting, linting, unit tests (< 2 minutes)
- **Medium checks** (PRs): Integration tests, builds (5-15 minutes)
- **Slow checks** (main branch): Full test suite, benchmarks, multi-platform builds (30+ minutes)

**Fail Fast Principle**: Run the fastest, most likely-to-fail checks first. Don't waste time on slow checks if fast ones fail.

**Environment Parity**: Development, CI, and production use the same tools and versions. This ensures "works on my machine" becomes "works everywhere."

**Parallel Execution**: Run independent checks in parallel for faster overall execution.

### Our Implementation

- **Local script** (`scripts/ci.sh`) runs the same checks as CI
- **Makefile** provides convenient shortcuts (`make ci`)
- **GitHub Actions** mirrors local pipeline exactly
- **Fast feedback** with clear error messages
- **Caching** for faster builds in CI

## CI/CD in Development vs Production

### Development (Local)

**Purpose**: Fast feedback, catch issues early

**Characteristics**:
- Run manually when needed
- Can include GUI tests (if display available)
- Can skip slow checks (benchmarks)
- Interactive debugging
- Fast iteration

**Usage**:
```bash
# During active development
make ci              # Quick check before commit

# Before pushing
make ci-full         # Full check including GUI tests
```

### CI/CD (GitHub Actions)

**Purpose**: Automated validation, prevent regressions

**Characteristics**:
- Runs automatically on push/PR
- Excludes GUI tests by default (headless)
- Runs on multiple platforms
- Generates reports and artifacts
- Blocks merges on failure

**Workflow Structure**:
1. **Fast Checks** (every commit)
   - Code quality
   - Unit tests
   - Basic builds

2. **Full Tests** (PRs and main)
   - Integration tests
   - Example builds
   - API validation

3. **Platform Builds** (PRs and main)
   - Multi-platform validation
   - Platform-specific tests

4. **Benchmarks** (main branch only)
   - Performance regression detection
   - Baseline comparison

### Key Differences

| Aspect | Development | CI/CD |
|--------|------------|-------|
| **Trigger** | Manual | Automatic |
| **GUI Tests** | Optional | Excluded (headless) |
| **Platforms** | Current platform | All platforms |
| **Benchmarks** | Optional | Scheduled/main branch |
| **Speed** | Optimized for dev | Optimized for reliability |
| **Debugging** | Interactive | Logs/artifacts |

## Framework-Specific Considerations

Since Obsidian is a **framework** (not an application), our CI/CD has special requirements:

### 1. API Stability

- **Public API validation**: Ensures breaking changes are caught
- **Example compatibility**: Examples must use only public API
- **Backward compatibility**: Track API changes over time

### 2. Multi-Platform Support

- **Cross-platform builds**: Test on macOS, Linux, Windows, iOS, Android
- **Platform-specific tests**: Validate platform bindings
- **FFI validation**: Ensure foreign function interfaces work correctly

### 3. Performance Regression

- **Benchmark tracking**: Detect performance regressions
- **Memory leak detection**: Critical for framework stability
- **Startup time**: Framework initialization performance

### 4. Example Applications

- **Build validation**: All examples must build
- **Runtime validation**: Examples must run correctly
- **Reference implementations**: Examples serve as documentation

## Setting Up CI/CD

### Initial Setup

1. **Install development tools**:
   ```bash
   ./scripts/install-tools.sh
   ```

2. **Verify setup**:
   ```bash
   make dev-setup
   ```

3. **Run first CI check**:
   ```bash
   make ci
   ```

### GitHub Actions Setup

The GitHub Actions workflow (`.github/workflows/ci.yml`) is already configured and will automatically run when you:

1. Push to `main` or `develop` branches
2. Open a pull request
3. Manually trigger via GitHub UI

**No additional setup required!** Just push your code.

## Troubleshooting

### Common Issues

**Build fails locally but works in CI**:
- Check Bazel version matches (`.bazelversion`)
- Clear Bazel cache: `bazel clean --expunge`
- Verify platform-specific configs

**Tests pass locally but fail in CI**:
- Check for platform-specific code
- Verify test tags (GUI tests excluded in CI)
- Check for timing-dependent tests

**CI is slow**:
- Enable Bazel remote caching
- Use build caching in GitHub Actions
- Optimize test execution

### Getting Help

- Check pipeline logs for specific errors
- Run `make ci-verbose` for detailed output
- Review `.github/workflows/ci.yml` for CI configuration
- Check `scripts/ci.sh` for local pipeline logic

## Future Enhancements

Planned improvements:

- [ ] Performance regression detection with baselines
- [ ] Memory leak detection (Valgrind, AddressSanitizer)
- [ ] Automated API documentation generation
- [ ] Release automation
- [ ] Nightly builds for all platforms
- [ ] Automated dependency updates

## CI/CD for Frameworks vs Applications

Since Obsidian is a **framework** (not an application), our CI/CD focuses on:

- **API Stability**: Public API validation, backward compatibility tracking
- **Multi-Platform Support**: Cross-platform builds, platform-specific tests, FFI validation
- **Performance Regression**: Benchmark tracking, memory leak detection, startup time
- **Example Applications**: Build validation, runtime validation, reference implementations

Unlike applications that deploy to production, frameworks are distributed as libraries. Our pipeline validates quality and compatibility rather than deploying code.

