load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "shadow",
    srcs = [
        "shadow_node.cpp",
        "shadow_tree.cpp",
        "mounting_manager.cpp",
    ],
    hdrs = [
        "shadow_node.h",
        "shadow_tree.h",
        "mounting_manager.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/layout:layout",
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
