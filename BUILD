load("@npm//:defs.bzl", "npm_link_all_packages")

# Root BUILD file for Obsidian workspace
package(default_visibility = ["//visibility:public"])

# Link all npm packages
npm_link_all_packages(name = "node_modules")

# Export workspace configuration files
exports_files([
    ".bazelrc",
    ".bazelversion",
    "MODULE.bazel",
])
