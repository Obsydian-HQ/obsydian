load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

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

cc_test(
    name = "shadow_test",
    srcs = ["shadow_test.cpp"],
    deps = [
        ":shadow",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
