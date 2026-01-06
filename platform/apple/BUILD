# Apple Platform Bindings
# Shared Apple platform code and platform-specific subdirectories

package(default_visibility = ["//visibility:public"])

# Apple platform bindings are organized as:
# - macos/     - macOS-specific bindings (AppKit)
# - ios/       - iOS-specific bindings (UIKit) - planned
# - renderer/  - Shared renderer/mounting system
# - styling/   - Platform-specific styling - planned

# Re-export macOS bindings for backward compatibility during transition
alias(
    name = "apple_ffi",
    actual = "//platform/apple/macos:apple_ffi",
)

alias(
    name = "apple_objc_bridge",
    actual = "//platform/apple/macos:apple_objc_bridge",
)

alias(
    name = "apple_c_headers",
    actual = "//platform/apple/macos:apple_c_headers",
)

# Swift bridge currently disabled
# alias(
#     name = "apple_swift",
#     actual = "//platform/apple/macos:apple_swift",
# )
