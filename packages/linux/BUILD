# Linux Platform Bindings
# Provides POSIX and system API access

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Linux FFI bindings
cc_library(
    name = "linux_ffi",
    srcs = glob(
        ["**/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(
        [
            "**/*.h",
            "**/*.hpp",
        ],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["."],
    linkopts = select({
        "@platforms//os:linux": [
            "-lpthread",
            "-ldl",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//core:ffi_base",
    ],
)
