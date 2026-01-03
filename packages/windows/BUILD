# Windows Platform Bindings
# Provides direct Win32 API access

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Windows FFI bindings
cc_library(
    name = "windows_ffi",
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
        "@platforms//os:windows": [
            "-luser32",
            "-lkernel32",
            "-lgdi32",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//core:ffi_base",
    ],
)
