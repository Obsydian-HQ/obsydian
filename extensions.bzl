# Obsidian Module Extensions
# Extensions for dependencies not available in Bazel Central Registry

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def _google_benchmark_extension_impl(module_ctx):
    """Module extension to fetch Google Benchmark via http_archive.

    Google Benchmark doesn't have native Bazel support, so we fetch the source
    and create BUILD files for it.
    """
    http_archive(
        name = "com_github_google_benchmark",
        urls = [
            "https://github.com/google/benchmark/archive/v1.9.4.tar.gz",
        ],
        sha256 = "b334658edd35efcf06a99d9be21e4e93e092bd5f95074c1673d5c8705d95c104",
        strip_prefix = "benchmark-1.9.4",
        build_file_content = """
# Google Benchmark BUILD file
# Generated for Obsidian project - C++ library only (no Python bindings)

package(default_visibility = ["//visibility:public"])

# Core benchmark library
cc_library(
    name = "benchmark",
    srcs = glob([
        "src/*.cc",
    ], exclude = [
        "src/benchmark_main.cc",
    ]),
    hdrs = glob([
        "include/benchmark/**/*.h",
        "src/*.h",  # Include internal headers from src/
    ]),
    includes = [
        "include",
        "src",  # Add src/ to include path for internal headers
    ],
    copts = [
        "-std=c++14",  # Google Benchmark 1.9.4 requires C++14
        "-Wall",
        "-Wextra",
        "-Wshadow",
    ],
    linkopts = ["-pthread"],
)

# Benchmark main (provides main() function)
cc_library(
    name = "benchmark_main",
    srcs = ["src/benchmark_main.cc"],
    hdrs = glob([
        "include/benchmark/**/*.h",
    ]),
    includes = ["include"],
    deps = [":benchmark"],
    linkopts = ["-pthread"],
)
""",
        patches = [
            # Patch to remove Python bindings dependency if BUILD.bazel exists
        ],
        patch_cmds = [
            # Remove Python bindings directory to avoid dependency issues
            "rm -rf bindings/python 2>/dev/null || true",
        ],
    )

google_benchmark_extension = module_extension(
    implementation = _google_benchmark_extension_impl,
)
