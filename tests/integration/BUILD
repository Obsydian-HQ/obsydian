# Integration tests
# Test component interactions and end-to-end behavior

load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

# FFI to Platform integration test
# This will test that C++ can actually call native platform APIs
cc_test(
    name = "ffi_platform_test",
    size = "medium",
    srcs = ["ffi_platform_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//core:ffi_base",
        "//packages/apple:apple_ffi",  # macOS FFI bindings
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# UI to Runtime integration test
cc_test(
    name = "ui_runtime_test",
    size = "small",
    srcs = ["ui_runtime_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//core:runtime",
        "//core:ui",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Process execution API integration test
cc_test(
    name = "process_test",
    size = "medium",
    srcs = ["process_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Process management API integration test
cc_test(
    name = "process_list_test",
    size = "medium",
    srcs = ["process_list_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Layout constraint API integration test
cc_test(
    name = "layout_constraint_test",
    size = "small",
    srcs = ["layout_constraint_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//packages/apple:apple_layout_ffi",
        "//packages/apple:apple_ffi",
        "//packages/apple:apple_c_headers",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# VStack API integration test
cc_test(
    name = "vstack_test",
    size = "small",
    srcs = ["vstack_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        "//packages/apple:apple_ffi",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Spacer API integration test
cc_test(
    name = "spacer_test",
    size = "small",
    srcs = ["spacer_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        "//packages/apple:apple_ffi",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Route Scanner and Registry integration test
cc_test(
    name = "route_scanner_test",
    size = "small",
    srcs = ["route_scanner_test.cpp"],
    copts = ["-std=c++20"],
    data = [
        "//test_apps/route_scanner_test:app",
    ],
    deps = [
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Router integration test
cc_test(
    name = "router_test",
    size = "small",
    srcs = ["router_test.cpp"],
    copts = ["-std=c++20"],
    data = [
        "//test_apps/route_scanner_test:app",
    ],
    deps = [
        "//src:obsidian",
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Route rendering integration test
cc_test(
    name = "route_rendering_test",
    size = "small",
    srcs = ["route_rendering_test.cpp"],
    copts = ["-std=c++20"],
    data = [
        "//test_apps/route_scanner_test:app",
    ],
    deps = [
        "//src:obsidian",
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Link component integration test
cc_test(
    name = "link_test",
    size = "small",
    srcs = ["link_test.cpp"],
    copts = ["-std=c++20"],
    data = [
        "//test_apps/route_scanner_test:app",
    ],
    deps = [
        "//src:obsidian",
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Route matcher integration test
cc_test(
    name = "route_matcher_test",
    size = "small",
    srcs = ["route_matcher_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# Navigation history integration test
cc_test(
    name = "navigation_history_test",
    size = "small",
    srcs = ["navigation_history_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//core/routing:routing",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
