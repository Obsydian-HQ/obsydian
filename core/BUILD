# Obsidian Core Runtime
# This is the heart of Obsidian - the runtime and language layer

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Core runtime library (C++)
cc_library(
    name = "runtime",
    srcs = glob(
        ["runtime/**/*.cpp"],
        exclude = ["runtime/**/*_test.cpp"],
    ),
    hdrs = glob(["runtime/**/*.h"]) + glob(
        ["runtime/**/*.hpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["runtime"],
    deps = [
        ":ffi_base",
        "//include:obsidian_headers",  # For AppCallbacks definition
        "//packages/apple/renderer:fabric",  # For renderer/mounting system
    ],
)

# FFI base layer
cc_library(
    name = "ffi_base",
    srcs = glob(
        ["ffi/**/*.cpp"],
        exclude = ["ffi/**/*_test.cpp"],
    ),
    hdrs = glob(["ffi/**/*.h"]) + glob(
        ["ffi/**/*.hpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["ffi"],
    deps = [
        # Platform-specific FFI implementations
        "//packages/apple:apple_ffi",
    ],
)

# UI abstraction layer
cc_library(
    name = "ui",
    srcs = glob(
        ["ui/**/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(["ui/**/*.h"]) + glob(
        ["ui/**/*.hpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["ui"],
    deps = [
        ":runtime",
    ],
)

# Layout engine (delegated to core/layout/BUILD)
# This target is defined in core/layout/BUILD

# Compiler (for Obsidian language features)
cc_library(
    name = "compiler",
    srcs = glob(
        ["compiler/**/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(
        ["compiler/**/*.h"],
        allow_empty = True,
    ) + glob(
        ["compiler/**/*.hpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["compiler"],
    deps = [
        ":runtime",
    ],
)
