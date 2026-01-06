# Router Patterns example app
# Demonstrates all navigation patterns

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
    name = "router_patterns_lib",
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
    name = "router_patterns_app",
    bundle_id = "com.obsidian.examples.routerpatterns",
    infoplists = [":Info.plist"],
    minimum_os_version = "11.0",
    deps = [":router_patterns_lib"],
)

# Also provide a simple binary for command-line testing
cc_binary(
    name = "router_patterns",
    srcs = ["main.cpp"],
    copts = ["-std=c++20"],
    deps = [
        ":routes",
        "//src:obsidian",
    ],
)
