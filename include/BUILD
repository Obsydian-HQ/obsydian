# Public API Headers
# This target exposes only the public API headers to users

load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Public API header-only library
# Users can include <obsidian/obsidian.h> or <obsidian/window.h>, etc.
cc_library(
    name = "obsidian_headers",
    hdrs = glob(["obsidian/**/*.h"]),
    includes = ["."],  # This allows #include <obsidian/obsidian.h>
    deps = [
        # Implementation is in //src:obsidian_impl
    ],
)
