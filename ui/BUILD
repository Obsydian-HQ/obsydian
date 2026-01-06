# Obsidian UI Layer
# Public API implementations for UI components, containers, and navigation

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# UI Components - Interactive UI elements
cc_library(
    name = "components",
    srcs = glob(["components/*.cpp"]),
    copts = ["-std=c++20"],
    deps = [
        "//core:ffi_base",
        "//include:obsidian_headers",
        "//platform/apple/macos:apple_ffi",
    ],
)

# UI Containers - Layout and container elements
cc_library(
    name = "containers",
    srcs = glob(["containers/*.cpp"]),
    copts = ["-std=c++20"],
    deps = [
        ":components",
        "//core:ffi_base",
        "//core/layout",
        "//include:obsidian_headers",
        "//platform/apple/macos:apple_ffi",
        "//platform/apple/renderer:fabric",
    ],
)

# UI Navigation - Router and navigation components
cc_library(
    name = "navigation",
    srcs = glob(["navigation/*.cpp"]),
    copts = ["-std=c++20"],
    deps = [
        ":components",
        ":containers",
        "//core:ffi_base",
        "//core/routing",
        "//include:obsidian_headers",
        "//platform/apple/macos:apple_ffi",
    ],
)

# App lifecycle
cc_library(
    name = "app",
    srcs = ["app.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//core:runtime",
        "//include:obsidian_headers",
    ],
)

# Combined UI library
cc_library(
    name = "ui",
    deps = [
        ":app",
        ":components",
        ":containers",
        ":navigation",
    ],
)
