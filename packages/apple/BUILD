# Apple Platform Bindings (iOS, macOS, watchOS, tvOS)
# Provides FFI bindings for Swift/Objective-C APIs

load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_swift//swift:swift.bzl", "swift_interop_hint", "swift_library")

package(default_visibility = ["//visibility:public"])

# Swift interop hint to expose C headers to Swift
swift_interop_hint(
    name = "apple_c_headers_interop",
    module_name = "ObsidianMacOS",
)

# C headers library (exposes C functions to Swift)
cc_library(
    name = "apple_c_headers",
    hdrs = [
        "macos_platform.h",
        "macos_window.h",
        "macos_button.h",
        "macos_textfield.h",
        "macos_textview.h",
        "macos_scrollview.h",
        "macos_table.h",
        "macos_list.h",
        "macos_process.h",
        "macos_process_list.h",
        "macos_layout.h",
        "macos_vstack.h",
    ],
    aspect_hints = [":apple_c_headers_interop"],
    copts = ["-std=c++20"],
    includes = ["."],
)

# Objective-C++ bridge layer (C++ → Objective-C interop)
# This provides the low-level Objective-C++ implementation
objc_library(
    name = "apple_objc_bridge",
    srcs = [
        "macos_button.mm",
        "macos_platform.mm",
        "macos_window.mm",
        "macos_textfield.mm",
        "macos_textview.mm",
        "macos_scrollview.mm",
        "macos_table.mm",
        "macos_list.mm",
        "macos_process.mm",
        "macos_process_list.mm",
        "macos_layout.mm",
        "macos_vstack.mm",
    ],
    hdrs = [
        "macos_button.h",
        "macos_platform.h",
        "macos_window.h",
        "macos_textfield.h",
        "macos_textview.h",
        "macos_scrollview.h",
        "macos_table.h",
        "macos_list.h",
        "macos_process.h",
        "macos_process_list.h",
        "macos_layout.h",
        "macos_vstack.h",
    ],
    copts = [
        "-std=c++20",
        "-fobjc-arc",
        # Suppress false positive warnings about __bridge_retained/__bridge_transfer
        # These casts ARE effective when ARC is enabled (which it is via -fobjc-arc)
        # The linter incorrectly reports these warnings, but the compiler knows ARC is enabled
        "-Wno-error=arc-bridge-casts-disallowed-in-nonarc",
    ],
    includes = ["."],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
)

# C++ wrapper layer (provides C++ interface to Objective-C bridge)
cc_library(
    name = "apple_ffi",
    srcs = ["macos_ffi.cpp"],
    hdrs = ["macos_ffi.h"],
    copts = ["-std=c++20"],
    includes = ["."],
    linkopts = select({
        "@platforms//os:osx": [
            "-framework",
            "AppKit",
            "-framework",
            "Foundation",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":apple_objc_bridge",
        # Note: Does NOT depend on //core:ffi_base to avoid circular dependency
        # ffi_base depends on apple_ffi, not the other way around
    ],
)

# Layout FFI wrapper (C++ interface for layout constraints)
cc_library(
    name = "apple_layout_ffi",
    srcs = ["macos_layout_ffi.cpp"],
    hdrs = ["macos_layout_ffi.h"],
    copts = ["-std=c++20"],
    includes = ["."],
    linkopts = select({
        "@platforms//os:osx": [
            "-framework",
            "AppKit",
            "-framework",
            "Foundation",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":apple_objc_bridge",
    ],
)

# Swift bridge (high-level Swift interface)
swift_library(
    name = "apple_swift",
    srcs = [
        "ApplePlatform.swift",
    ],
    generates_header = True,
    deps = [
        ":apple_c_headers",
        ":apple_objc_bridge",
    ],
)
