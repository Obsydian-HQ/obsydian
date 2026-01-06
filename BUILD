# Root BUILD file for Obsidian Framework
# Cross-platform native app framework

package(default_visibility = ["//visibility:public"])

# Export workspace configuration files
exports_files([
    ".bazelrc",
    ".bazelversion",
    "MODULE.bazel",
])

# Main Obsidian library - complete public API
# This is what applications should depend on
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "obsidian",
    deps = [
        "//include:obsidian_headers",
        "//system:system",
        "//ui:ui",
    ],
)
