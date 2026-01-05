# Obsidian Core - Routing Module
# Route scanner and registry for file-based routing

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

# Route scanner and registry library
cc_library(
    name = "routing",
    srcs = glob(
        ["*.cpp"],
        exclude = ["*_test.cpp"],
    ),
    hdrs = glob(["*.h"]),
    copts = ["-std=c++20"],
    includes = ["."],
    deps = [
        "//include:obsidian_headers",  # For RouteContext, Router, Window forward declarations/headers
    ],
)
