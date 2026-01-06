load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "mounting",
    srcs = [
        "component_view_registry.cpp",
        "mounting_coordinator.cpp",
    ],
    hdrs = [
        "component_view_registry.h",
        "mounting_coordinator.h",
        "view_mutation.h",
    ],
    copts = ["-std=c++20"],
    visibility = ["//visibility:public"],
    # Note: No deps on layout to avoid circular dependency
    # Layout types are forward-declared where needed
)

cc_test(
    name = "mounting_test",
    srcs = ["mounting_test.cpp"],
    deps = [
        ":mounting",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
