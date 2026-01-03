# Hello World example app
# Minimal app that demonstrates basic Obsidian usage
# This example uses ONLY the public API - no internal headers

load("@rules_apple//apple:macos.bzl", "macos_application")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# C++ library containing main function
# Uses ONLY the public API
cc_library(
    name = "hello_world_lib",
    srcs = ["main.cpp"],
    hdrs = [],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only - no internal dependencies
    ],
)

# macOS application bundle
macos_application(
    name = "hello_world_app",
    bundle_id = "com.obsidian.examples.helloworld",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":hello_world_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "hello_world",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",  # Public API only
    ],
)
