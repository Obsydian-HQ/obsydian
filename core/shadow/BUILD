# Obsidian Shadow Tree
# Virtual representation of the UI tree for layout computation and diff generation

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "shadow",
    srcs = [
        "mounting_manager.cpp",
        "shadow_node.cpp",
        "shadow_tree.cpp",
    ],
    hdrs = [
        "mounting_manager.h",
        "shadow_node.h",
        "shadow_tree.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/layout",
    ],
)
