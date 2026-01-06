# TextView Showcase example app
# Demonstrates all TextView features and capabilities
# This example uses ONLY the public API - no internal headers

load("@rules_apple//apple:macos.bzl", "macos_application")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# C++ library containing main function
# Uses ONLY the public API - no internal dependencies
cc_library(
    name = "textview_showcase_lib",
    srcs = ["main.cpp"],
    hdrs = [],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only - no internal dependencies
    ],
)

# macOS application bundle
macos_application(
    name = "textview_showcase_app",
    bundle_id = "com.obsidian.examples.textviewshowcase",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":textview_showcase_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "textview_showcase",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only
    ],
)
