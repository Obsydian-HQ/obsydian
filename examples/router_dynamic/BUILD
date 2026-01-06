# Router Dynamic example app
# Demonstrates dynamic route segments

load("@rules_apple//apple:macos.bzl", "macos_application")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Route files library
# alwayslink ensures static initializers in route files are not dead-stripped by the linker
cc_library(
    name = "routes",
    srcs = glob(["app/**/*.cpp"]),
    copts = ["-std=c++20"],
    deps = [
        "//src:obsidian",
    ],
    alwayslink = 1,
)

# C++ library containing main function
cc_library(
    name = "router_dynamic_lib",
    srcs = ["main.cpp"],
    hdrs = [],
    copts = ["-std=c++20"],
    deps = [
        ":routes",
        "//src:obsidian",
    ],
)

# macOS application bundle
macos_application(
    name = "router_dynamic_app",
    bundle_id = "com.obsidian.examples.routerdynamic",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":router_dynamic_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "router_dynamic",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        ":routes",
        "//src:obsidian",
    ],
)
