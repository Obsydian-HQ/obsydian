# Layout Showcase example app
# Demonstrates VStack, HStack, and other layout components

load("@rules_apple//apple:macos.bzl", "macos_application")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# C++ library containing main function
cc_library(
    name = "layout_showcase_lib",
    srcs = ["main.cpp"],
    hdrs = [],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only - no internal dependencies
    ],
)

# macOS application bundle
macos_application(
    name = "layout_showcase_app",
    bundle_id = "com.obsidian.examples.layoutshowcase",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":layout_showcase_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "layout_showcase",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only
    ],
)
