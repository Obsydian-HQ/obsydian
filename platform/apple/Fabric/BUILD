# Platform-specific Fabric implementation for macOS

load("@rules_cc//cc:defs.bzl", "objc_library")

objc_library(
    name = "fabric_mounting",
    srcs = [
        "Mounting/NSView+OBSComponentViewProtocol.mm",
        "Mounting/OBSComponentViewRegistry.mm",
        "Mounting/OBSMountingManager.mm",
    ],
    hdrs = [
        "Mounting/NSView+OBSComponentViewProtocol.h",
        "Mounting/OBSComponentViewDescriptor.h",
        "Mounting/OBSComponentViewProtocol.h",
        "Mounting/OBSComponentViewRegistry.h",
        "Mounting/OBSMountingManager.h",
    ],
    copts = [
        "-std=c++17",
        "-fobjc-arc",
    ],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
    visibility = ["//visibility:public"],
)

objc_library(
    name = "component_views",
    srcs = [
        "Mounting/ComponentViews/OBSButtonComponentView.mm",
        "Mounting/ComponentViews/OBSTextComponentView.mm",
        "Mounting/ComponentViews/OBSViewComponentView.mm",
    ],
    hdrs = [
        "Mounting/ComponentViews/OBSButtonComponentView.h",
        "Mounting/ComponentViews/OBSTextComponentView.h",
        "Mounting/ComponentViews/OBSViewComponentView.h",
    ],
    copts = [
        "-std=c++17",
        "-fobjc-arc",
    ],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
    visibility = ["//visibility:public"],
    deps = [":fabric_mounting"],
)

objc_library(
    name = "component_factory",
    srcs = ["OBSComponentViewFactory.mm"],
    hdrs = ["OBSComponentViewFactory.h"],
    copts = [
        "-std=c++17",
        "-fobjc-arc",
    ],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":component_views",
        ":fabric_mounting",
    ],
)

objc_library(
    name = "fabric_bridge",
    srcs = ["OBSFabricBridge.mm"],
    hdrs = ["OBSFabricBridge.h"],
    copts = [
        "-std=c++17",
        "-fobjc-arc",
    ],
    sdk_frameworks = [
        "AppKit",
        "Foundation",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":component_factory",
        ":component_views",
        ":fabric_mounting",
    ],
)

# Combined target for all Fabric functionality
objc_library(
    name = "fabric",
    visibility = ["//visibility:public"],
    deps = [
        ":component_factory",
        ":component_views",
        ":fabric_bridge",
        ":fabric_mounting",
    ],
)
