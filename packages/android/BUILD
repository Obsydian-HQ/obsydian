# Android Platform Bindings
# Provides JNI bindings for Java/Kotlin APIs

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_kotlin//kotlin:jvm.bzl", "kt_jvm_library")

package(default_visibility = ["//visibility:public"])

# Android JNI bindings (C++)
cc_library(
    name = "android_jni",
    srcs = glob(
        ["**/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(
        [
            "**/*.h",
            "**/*.hpp",
        ],
        allow_empty = True,
    ),
    copts = ["-std=c++20"],
    includes = ["."],
    deps = [
        "//core:ffi_base",
    ],
)

# Kotlin/Java interface
# Note: kt_jvm_library cannot directly depend on cc_library (android_jni) because
# they provide different providers (JavaInfo vs CcInfo). The native library (android_jni)
# is linked at the Android application level, and loaded at runtime via System.loadLibrary().
# See Placeholder.kt for example usage.
kt_jvm_library(
    name = "android_kt",
    srcs = glob(["**/*.kt"]),
    deps = [
        # JNI bindings are linked at app level, not library level
    ],
)
