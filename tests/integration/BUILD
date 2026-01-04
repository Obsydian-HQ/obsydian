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
    size = "medium",
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
