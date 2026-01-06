# macOS Platform Bindings
# FFI bindings for macOS (AppKit)

load("@rules_cc//cc:defs.bzl", "cc_library", "objc_library")
load("@rules_swift//swift:swift.bzl", "swift_interop_hint")

package(default_visibility = ["//visibility:public"])

# Swift interop hint to expose C headers to Swift
swift_interop_hint(
    name = "apple_c_headers_interop",
    module_name = "ObsidianMacOS",
)

# C headers library (exposes C functions to Swift)
cc_library(
    name = "apple_c_headers",
    hdrs = glob(["*.h"]),
    aspect_hints = [":apple_c_headers_interop"],
    copts = ["-std=c++20"],
    includes = ["."],
)

# Objective-C++ bridge layer (C++ → Objective-C interop)
objc_library(
    name = "apple_objc_bridge",
    srcs = glob(["*.mm"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++20",
        "-fobjc-arc",
        "-Wno-error=arc-bridge-casts-disallowed-in-nonarc",
    ],
    includes = [
        ".",
        "../renderer",
    ],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
    deps = [
        "//platform/apple/renderer:fabric",
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
    ],
)

# Swift bridge (high-level Swift interface)
# Currently disabled due to C++/Swift interop issues with cstdint headers
# swift_library(
#     name = "apple_swift",
#     srcs = ["ApplePlatform.swift"],
#     generates_header = True,
#     deps = [
#         ":apple_c_headers",
#         ":apple_objc_bridge",
#     ],
# )
