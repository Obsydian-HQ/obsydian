# Public API Implementation
# This contains the implementation of the public API, which wraps internal code

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Public API implementation library
# This wraps internal implementation and is linked by the public API
cc_library(
    name = "obsidian_impl",
    srcs = glob(["obsidian/**/*.cpp"]),
    hdrs = [],  # Headers are in //include:obsidian_headers
    copts = ["-std=c++20"],
    deps = [
        "//core:ffi_base",
        "//core:runtime",
        "//core/layout",  # Layout engine
        "//core/routing",  # Routing system
        "//include:obsidian_headers",  # Need headers for implementation
        "//packages/apple:apple_ffi",
        "//packages/apple/renderer:fabric",  # Renderer/mounting system
    ],
)

# Combined public API library (headers + implementation)
# This is what users should depend on
cc_library(
    name = "obsidian",
    hdrs = [],  # Headers come from //include:obsidian_headers
    deps = [
        ":obsidian_impl",
        "//include:obsidian_headers",
    ],
)
