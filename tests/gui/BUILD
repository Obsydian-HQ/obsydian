# GUI Tests
# Tests that require a graphical environment (display server)
# These tests are separate from unit/integration tests and should be run manually
# or in CI environments with a display.

load("@rules_cc//cc:defs.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

# macOS Window GUI tests
# These tests require NSApplication and a GUI environment
cc_test(
    name = "macos_window_test",
    size = "large",  # GUI tests may take longer
    srcs = ["macos_window_test.cpp"],
    copts = ["-std=c++20"],
    tags = [
        "manual",  # Don't run automatically in CI
        "requires-gui",  # Mark as requiring GUI - will be skipped in headless CI
    ],
    deps = [
        "//packages/apple:apple_ffi",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# macOS Button GUI tests
# These tests require NSApplication and a GUI environment
cc_test(
    name = "macos_button_test",
    size = "large",  # GUI tests may take longer
    srcs = ["macos_button_test.cpp"],
    copts = ["-std=c++20"],
    tags = [
        "manual",  # Don't run automatically in CI
        "requires-gui",  # Mark as requiring GUI - will be skipped in headless CI
    ],
    deps = [
        "//packages/apple:apple_ffi",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# macOS VStack GUI tests
# These tests require NSApplication and a GUI environment
cc_test(
    name = "macos_vstack_test",
    size = "medium",  # GUI tests - medium is sufficient for current test duration
    srcs = ["macos_vstack_test.cpp"],
    copts = ["-std=c++20"],
    tags = [
        "manual",  # Don't run automatically in CI
        "requires-gui",  # Mark as requiring GUI - will be skipped in headless CI
    ],
    deps = [
        "//src:obsidian",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
