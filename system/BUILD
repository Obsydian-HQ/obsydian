# Obsidian System APIs
# Non-UI system functionality: processes, filesystem, network, etc.

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Process management
cc_library(
    name = "process",
    srcs = glob(["process/*.cpp"]),
    copts = ["-std=c++20"],
    deps = [
        "//include:obsidian_headers",
        "//platform/apple/macos:apple_ffi",
    ],
)

# Filesystem APIs (placeholder for future implementation)
cc_library(
    name = "filesystem",
    srcs = glob(
        ["filesystem/*.cpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    deps = [
        "//include:obsidian_headers",
    ],
)

# Network APIs (placeholder for future implementation)
cc_library(
    name = "network",
    srcs = glob(
        ["network/*.cpp"],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    deps = [
        "//include:obsidian_headers",
    ],
)

# Combined system library
cc_library(
    name = "system",
    deps = [
        ":filesystem",
        ":network",
        ":process",
    ],
)
