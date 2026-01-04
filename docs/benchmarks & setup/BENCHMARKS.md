# Performance Benchmarks and Quality Metrics

This document describes the performance benchmarking and memory leak detection infrastructure for Obsidian.

## Performance Benchmarks

### Overview

Performance benchmarks are located in the `benchmarks/` directory and use [Google Benchmark](https://github.com/google/benchmark) for measuring performance.

### Available Benchmarks

1. **Window Creation Benchmark** (`window_creation_bench`)
   - Measures time to create and destroy windows
   - Tests single and multiple window creation scenarios
   - Measures window property access performance

2. **UI Render Benchmark** (`ui_render_bench`)
   - Measures time to create UI components (buttons, etc.)
   - Tests single and multiple component creation
   - Measures component property access performance

3. **Memory Usage Benchmark** (`memory_usage_bench`)
   - Measures memory consumption during operations
   - Tracks memory usage for window creation
   - Platform-specific memory tracking (macOS/Linux)

4. **Startup Time Benchmark** (`startup_time_bench`)
   - Measures application initialization time
   - Tests full application lifecycle performance

### Running Benchmarks

```bash
# Run all benchmarks
make benchmarks

# Or via CI script
./scripts/ci.sh --benchmarks

# Run specific benchmark
bazel run //benchmarks:window_creation_bench
```

### Performance Regression Detection

The CI pipeline includes automatic performance regression detection. It compares current benchmark results against a baseline and fails if performance degrades by more than 10%.

**Baseline Management:**
- Baselines are stored in `.benchmark_baselines/baseline.txt`
- First run creates the baseline automatically
- Subsequent runs compare against the baseline
- Use `scripts/benchmark_regression.sh` for manual regression checks

**Regression Threshold:**
- Default: 10% performance degradation
- Configurable in `scripts/benchmark_regression.sh`

## Memory Leak Detection

### AddressSanitizer (ASan)

AddressSanitizer is enabled via Bazel configuration flags. It detects:
- Memory leaks
- Use-after-free errors
- Buffer overflows
- Stack and heap buffer overflows

**Usage:**
```bash
# Run tests with AddressSanitizer
bazel test //core/... --config=asan

# Via CI script
./scripts/ci.sh --memory-check
make memory-check
```

**Configuration:**
- Enabled in `.bazelrc` with `--config=asan`
- ASAN_OPTIONS can be customized via environment variables

### Valgrind (Linux)

On Linux systems, Valgrind is used for memory leak detection when available.

**Usage:**
```bash
# Valgrind is automatically used on Linux if available
./scripts/ci.sh --memory-check
```

**Note:** Valgrind requires Linux and may slow down test execution significantly.

### Other Sanitizers

Additional sanitizers are configured in `.bazelrc`:
- **ThreadSanitizer (TSan)**: `--config=tsan` - Detects data races
- **MemorySanitizer (MSan)**: `--config=msan` - Detects uninitialized memory reads
- **UndefinedBehaviorSanitizer (UBSan)**: `--config=ubsan` - Detects undefined behavior

## CI Integration

All these features are integrated into the CI pipeline:

```bash
# Full CI with all checks
make ci-full

# Individual checks
make benchmarks      # Performance benchmarks
make memory-check    # Memory leak detection
```

### CI Script Options

```bash
./scripts/ci.sh [OPTIONS]

Options:
  -b, --benchmarks        Run performance benchmarks
  -m, --memory-check      Run memory leak detection
  -g, --gui-tests         Include GUI tests
  -v, --verbose           Verbose output
```

## Best Practices

1. **Baseline Updates:** Update performance baselines when intentional performance changes are made
2. **Memory Checks:** Run memory checks before releases and after major refactoring
3. **Regression Thresholds:** Adjust regression thresholds based on project needs
5. **Sanitizer Usage:** Use different sanitizers for different types of issues:
   - ASan for memory errors
   - TSan for threading issues
   - UBSan for undefined behavior

## Troubleshooting

### Benchmark Failures

- Check that Google Benchmark is properly configured in `MODULE.bazel`
- Verify benchmark binaries compile: `bazel build //benchmarks/...`

### Memory Leak Detection Issues

- **ASan not working:** Ensure `--config=asan` is used
- **Valgrind not found:** Install Valgrind on Linux: `sudo apt-get install valgrind`
- **False positives:** Some third-party libraries may trigger false positives

## References

- [Google Benchmark Documentation](https://github.com/google/benchmark)
- [AddressSanitizer Documentation](https://github.com/google/sanitizers/wiki/AddressSanitizer)

