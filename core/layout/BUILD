# Layout Engine Core
# Cross-platform layout logic

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Layout core library
cc_library(
    name = "layout",
    srcs = glob(
        ["*.cpp"],
        exclude = ["*_test.cpp"],
    ),
    hdrs = glob(["*.h"]),
    copts = ["-std=c++20"],
    includes = ["."],
    deps = [],
)
