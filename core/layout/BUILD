# Obsidian Layout Engine
# Frame-based layout system inspired by Yoga
# Calculates positions/sizes in a shadow tree, then applies to native views

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])

# Layout engine library
cc_library(
    name = "layout",
    srcs = [
        "engine.cpp",
        "manager.cpp",
        "node.cpp",
        "style.cpp",
        "view_node.cpp",
    ],
    hdrs = [
        "alignment.h",
        "engine.h",
        "manager.h",
        "node.h",
        "style.h",
        "view_node.h",
    ],
    copts = ["-std=c++20"],
    includes = ["."],
    deps = [],
)

# Unit tests for layout engine
cc_test(
    name = "layout_test",
    size = "small",
    srcs = ["layout_test.cpp"],
    copts = ["-std=c++20"],
    deps = [
        ":layout",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
