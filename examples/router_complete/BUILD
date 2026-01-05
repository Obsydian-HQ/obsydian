# Router Complete example app
# A real-world example demonstrating all routing features

load("@rules_apple//apple:macos.bzl", "macos_application")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Route files library
# alwayslink ensures static initializers in route files are not dead-stripped by the linker
cc_library(
    name = "routes",
    srcs = glob(["app/**/*.cpp"]),
    copts = ["-std=c++20"],
    alwayslink = 1,
    deps = [
        "//src:obsidian",
    ],
)

# C++ library containing main function
cc_library(
    name = "router_complete_lib",
    srcs = ["main.cpp"],
    hdrs = [],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        ":routes",
    ],
)

# macOS application bundle
macos_application(
    name = "router_complete_app",
    bundle_id = "com.obsidian.examples.routercomplete",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":router_complete_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "router_complete",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
        ":routes",
    ],
)
