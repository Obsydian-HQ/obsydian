# Performance Benchmarks for Obsidian
# Uses Google Benchmark for measuring performance

load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

# Window creation benchmark
# Measures time to create and destroy windows
cc_binary(
    name = "window_creation_bench",
    srcs = ["window_creation_bench.cpp"],
    copts = ["-std=c++20"],
    tags = ["benchmark"],
    deps = [
        "//core:runtime",
        "//include:obsidian_headers",
        "//src:obsidian_impl",  # Link against actual implementation
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

# UI render benchmark
# Measures time to render UI components
cc_binary(
    name = "ui_render_bench",
    srcs = ["ui_render_bench.cpp"],
    copts = ["-std=c++20"],
    tags = ["benchmark"],
    deps = [
        "//core:runtime",
        "//core:ui",
        "//include:obsidian_headers",
        "//src:obsidian_impl",  # Link against actual implementation
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

# Memory usage benchmark
# Measures memory consumption during operations
cc_binary(
    name = "memory_usage_bench",
    srcs = ["memory_usage_bench.cpp"],
    copts = ["-std=c++20"],
    tags = ["benchmark"],
    deps = [
        "//core:runtime",
        "//include:obsidian_headers",
        "//src:obsidian_impl",  # Link against actual implementation
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

# Startup time benchmark
# Measures application startup time
cc_binary(
    name = "startup_time_bench",
    srcs = ["startup_time_bench.cpp"],
    copts = ["-std=c++20"],
    tags = ["benchmark"],
    deps = [
        "//core:runtime",
        "//include:obsidian_headers",
        "//src:obsidian_impl",  # Link against actual implementation
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

# Router performance benchmark
# Measures router performance: initialization, route matching, navigation, history
cc_binary(
    name = "router_bench",
    srcs = ["router_bench.cpp"],
    copts = ["-std=c++20"],
    tags = ["benchmark"],
    deps = [
        "//core:runtime",
        "//core/routing:routing",
        "//include:obsidian_headers",
        "//src:obsidian_impl",  # Link against actual implementation
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

# Note: Benchmarks are cc_binary targets, not test targets
# They should be run with: bazel run //benchmarks:window_creation_bench
# Or via the CI script: ./scripts/ci.sh --benchmarks
