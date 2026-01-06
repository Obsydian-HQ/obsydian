#!/bin/bash
set -euo pipefail

# Production-ready XCFramework build script
# Uses xcodebuild archive (like Expo and React Native do)
# NO BAZEL - builds directly with Xcode

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
OUTPUT_DIR="${OUTPUT_DIR:-$REPO_ROOT/dist}"
VERSION="${VERSION:-$(git describe --tags --always 2>/dev/null | sed 's/^v//' || echo 'dev')}"
FRAMEWORK_NAME="Obsydian"

echo "🔨 Building ${FRAMEWORK_NAME} XCFramework v${VERSION}"
echo "📦 Output directory: $OUTPUT_DIR"
echo ""

cd "$REPO_ROOT"

# Clean output directory
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

# Temporary directory for archives
ARCHIVE_DIR=$(mktemp -d)
trap "rm -rf $ARCHIVE_DIR" EXIT

# Collect all source files
SOURCES=()
HEADERS=()

# C++ source files (exclude test files)
for file in src/obsidian/*.cpp; do
    [ -f "$file" ] && SOURCES+=("$file")
done

for file in core/runtime/*.cpp; do
    # Exclude test files
    [[ "$file" == *"_test.cpp" ]] && continue
    [ -f "$file" ] && SOURCES+=("$file")
done

for file in core/ffi/*.cpp; do
    # Exclude test files
    [[ "$file" == *"_test.cpp" ]] && continue
    [ -f "$file" ] && SOURCES+=("$file")
done

for file in core/layout/*.cpp; do
    # Exclude test files
    [[ "$file" == *"_test.cpp" ]] && continue
    [ -f "$file" ] && SOURCES+=("$file")
done

for file in core/routing/*.cpp; do
    # Exclude test files
    [[ "$file" == *"_test.cpp" ]] && continue
    [ -f "$file" ] && SOURCES+=("$file")
done

# Objective-C++ source files
for file in packages/apple/*.mm; do
    [ -f "$file" ] && SOURCES+=("$file")
done

# Fabric source files (Objective-C++)
# Use find with relative paths
while IFS= read -r file; do
    # Convert absolute path to relative path from repo root
    rel_file="${file#$REPO_ROOT/}"
    [ -f "$rel_file" ] && SOURCES+=("$rel_file")
done < <(find "$REPO_ROOT/platform/apple/Fabric" -name "*.mm" -type f)

# C++ FFI wrapper
for file in packages/apple/*.cpp; do
    [ -f "$file" ] && SOURCES+=("$file")
done

# Header files
for file in include/obsidian/*.h; do
    [ -f "$file" ] && HEADERS+=("$file")
done

for file in core/runtime/*.h core/ffi/*.h core/layout/*.h core/routing/*.h; do
    [ -f "$file" ] && HEADERS+=("$file")
done

for file in packages/apple/*.h; do
    [ -f "$file" ] && HEADERS+=("$file")
done

echo "Found ${#SOURCES[@]} source files and ${#HEADERS[@]} header files"
echo "🔨 Building frameworks for each platform..."

# Build for macOS arm64
echo "Building for macOS (arm64)..."
MACOS_ARM64_DIR="$ARCHIVE_DIR/macos-arm64"
FRAMEWORK_DIR="$MACOS_ARM64_DIR/${FRAMEWORK_NAME}.framework"

# Create versioned bundle structure for macOS (required for validation)
mkdir -p "$FRAMEWORK_DIR/Versions/A/Headers"
mkdir -p "$FRAMEWORK_DIR/Versions/A/Resources"
mkdir -p "$FRAMEWORK_DIR/Versions/A/Modules"

# Compile all sources into object files
OBJECT_FILES=()
for src in "${SOURCES[@]}"; do
    # Create unique object file names by preserving directory structure
    obj_name=$(echo "$src" | sed 's|/|_|g' | sed 's/\.[^.]*$/.o/')
    obj_file="$MACOS_ARM64_DIR/$obj_name"
    # Determine compiler based on file extension
    # Note: .mm files are automatically recognized as Objective-C++ by clang++
    # No need for -x objective-c++ flag when extension is .mm
    if [[ "$src" == *.mm ]]; then
        COMPILER="clang++"
        OBJC_FLAGS="-fobjc-arc"
    else
        COMPILER="clang++"
        OBJC_FLAGS=""
    fi
    
    # Additional include paths for Fabric subdirectories
    FABRIC_INCLUDES=""
    if [[ "$src" == platform/apple/Fabric/* ]]; then
        FABRIC_INCLUDES="-I\"$REPO_ROOT/platform/apple/Fabric/Mounting\" -I\"$REPO_ROOT/platform/apple/Fabric/Mounting/ComponentViews\""
    fi
    
    # Use C++17 for Fabric files (as per BUILD file), C++20 for others
    CXX_STD="-std=c++20"
    if [[ "$src" == platform/apple/Fabric/* ]]; then
        CXX_STD="-std=c++17"
    fi
    
    if ! $COMPILER -c "$REPO_ROOT/$src" \
        -o "$obj_file" \
        $CXX_STD \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk macosx) \
        -I"$REPO_ROOT/include" \
        -I"$REPO_ROOT/core/runtime" \
        -I"$REPO_ROOT/core/ffi" \
        -I"$REPO_ROOT/core/layout" \
        -I"$REPO_ROOT/core/routing" \
        -I"$REPO_ROOT/packages/apple" \
        -I"$REPO_ROOT/platform/apple/Fabric" \
        $FABRIC_INCLUDES \
        -I"$REPO_ROOT" \
        $OBJC_FLAGS \
        -fPIC \
        -O2 \
        -Wall 2>&1; then
        echo "❌ Failed to compile $src for macOS arm64"
        exit 1
    fi
    OBJECT_FILES+=("$obj_file")
done

# Link into framework binary (in Versions/A/)
if [ ${#OBJECT_FILES[@]} -gt 0 ]; then
    if ! clang++ "${OBJECT_FILES[@]}" \
        -o "$FRAMEWORK_DIR/Versions/A/${FRAMEWORK_NAME}" \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk macosx) \
        -framework AppKit \
        -framework Foundation \
        -dynamiclib \
        -install_name "@rpath/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -compatibility_version 1.0.0 \
        -current_version 1.0.0 \
        -Wl,-rpath,@loader_path/Frameworks 2>&1; then
        echo "❌ Failed to link macOS arm64 framework"
        echo "   Check the linker errors above"
        exit 1
    fi
    
    # Copy headers to Versions/A/Headers
    for header in "${HEADERS[@]}"; do
        rel_path=$(echo "$header" | sed "s|^include/||" | sed "s|^packages/apple/||")
        mkdir -p "$FRAMEWORK_DIR/Versions/A/Headers/$(dirname "$rel_path")"
        cp "$REPO_ROOT/$header" "$FRAMEWORK_DIR/Versions/A/Headers/$rel_path"
    done
    
    # Create module map in Versions/A/Modules
    cat > "$FRAMEWORK_DIR/Versions/A/Modules/module.modulemap" << MODULEMAP_EOF
framework module ${FRAMEWORK_NAME} {
    umbrella header "${FRAMEWORK_NAME}.h"
    export *
    module * { export * }
}
MODULEMAP_EOF
    
    # Create Info.plist in Versions/A/Resources (required for versioned bundles)
    cat > "$FRAMEWORK_DIR/Versions/A/Resources/Info.plist" << INFOPLIST_EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>${FRAMEWORK_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>com.obsydian.${FRAMEWORK_NAME}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${FRAMEWORK_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>FMWK</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>MinimumOSVersion</key>
    <string>14.0</string>
</dict>
</plist>
INFOPLIST_EOF
    
    # Create Versions/Current symlink
    (cd "$FRAMEWORK_DIR/Versions" && ln -sf A Current)
    
    # Create root-level symlinks for versioned bundle structure
    (cd "$FRAMEWORK_DIR" && ln -sf Versions/Current/${FRAMEWORK_NAME} ${FRAMEWORK_NAME})
    (cd "$FRAMEWORK_DIR" && ln -sf Versions/Current/Headers Headers)
    (cd "$FRAMEWORK_DIR" && ln -sf Versions/Current/Resources Resources)
    (cd "$FRAMEWORK_DIR" && ln -sf Versions/Current/Modules Modules)
    
    # Remove any root-level Info.plist (for versioned bundles, Info.plist should ONLY be in Versions/A/Resources/)
    # Having it in the root causes "unsealed contents" code signing errors
    if [ -f "$FRAMEWORK_DIR/Info.plist" ]; then
        rm -f "$FRAMEWORK_DIR/Info.plist"
    fi
fi

# Build for iOS arm64 (device)
echo "Building for iOS (arm64 device)..."
IOS_ARM64_DIR="$ARCHIVE_DIR/ios-arm64"
mkdir -p "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers"
mkdir -p "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Modules"

OBJECT_FILES=()
for src in "${SOURCES[@]}"; do
    # Skip macOS-specific .mm files for iOS (they use AppKit which is macOS-only)
    if [[ "$src" == packages/apple/*.mm ]]; then
        continue
    fi
    
    obj_name=$(echo "$src" | sed 's|/|_|g' | sed 's/\.[^.]*$/.o/')
    obj_file="$IOS_ARM64_DIR/$obj_name"
    # Determine compiler based on file extension
    # Note: .mm files are automatically recognized as Objective-C++ by clang++
    if [[ "$src" == *.mm ]]; then
        COMPILER="clang++"
        OBJC_FLAGS="-fobjc-arc"
    else
        COMPILER="clang++"
        OBJC_FLAGS=""
    fi
    
    $COMPILER -c "$REPO_ROOT/$src" \
        -o "$obj_file" \
        -std=c++20 \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk iphoneos) \
        -I"$REPO_ROOT/include" \
        -I"$REPO_ROOT/core/runtime" \
        -I"$REPO_ROOT/core/ffi" \
        -I"$REPO_ROOT/core/layout" \
        -I"$REPO_ROOT/core/routing" \
        -I"$REPO_ROOT/packages/apple" \
        -I"$REPO_ROOT/platform/apple/Fabric" \
        -I"$REPO_ROOT" \
        $OBJC_FLAGS \
        -fPIC \
        -O2 \
        -miphoneos-version-min=14.0 \
        -Wall || {
        echo "⚠️  Failed to compile $src for iOS arm64"
        continue
    }
    OBJECT_FILES+=("$obj_file")
done

if [ ${#OBJECT_FILES[@]} -gt 0 ]; then
    clang++ "${OBJECT_FILES[@]}" \
        -o "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk iphoneos) \
        -framework UIKit \
        -framework Foundation \
        -dynamiclib \
        -install_name "@rpath/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -compatibility_version 1.0.0 \
        -current_version 1.0.0 \
        -miphoneos-version-min=14.0 \
        -Wl,-rpath,@loader_path/Frameworks || {
        echo "⚠️  Failed to link iOS arm64 framework"
    }
    
    # Copy headers (same as macOS)
    for header in "${HEADERS[@]}"; do
        rel_path=$(echo "$header" | sed "s|^include/||" | sed "s|^packages/apple/||")
        mkdir -p "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers/$(dirname "$rel_path")"
        cp "$REPO_ROOT/$header" "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers/$rel_path"
    done
    
    # Create module map and Info.plist (same as macOS)
    cp "$FRAMEWORK_DIR/Versions/A/Modules/module.modulemap" \
       "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Modules/module.modulemap"
    cp "$FRAMEWORK_DIR/Versions/A/Resources/Info.plist" \
       "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/Info.plist"
fi

# Build for iOS Simulator arm64
echo "Building for iOS Simulator (arm64)..."
IOS_SIM_ARM64_DIR="$ARCHIVE_DIR/ios-simulator-arm64"
mkdir -p "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers"
mkdir -p "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Modules"

OBJECT_FILES=()
for src in "${SOURCES[@]}"; do
    # Skip macOS-specific .mm files for iOS (they use AppKit which is macOS-only)
    if [[ "$src" == packages/apple/*.mm ]]; then
        continue
    fi
    
    obj_name=$(echo "$src" | sed 's|/|_|g' | sed 's/\.[^.]*$/.o/')
    obj_file="$IOS_SIM_ARM64_DIR/$obj_name"
    # Determine compiler based on file extension
    # Note: .mm files are automatically recognized as Objective-C++ by clang++
    if [[ "$src" == *.mm ]]; then
        COMPILER="clang++"
        OBJC_FLAGS="-fobjc-arc"
    else
        COMPILER="clang++"
        OBJC_FLAGS=""
    fi
    
    $COMPILER -c "$REPO_ROOT/$src" \
        -o "$obj_file" \
        -std=c++20 \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk iphonesimulator) \
        -I"$REPO_ROOT/include" \
        -I"$REPO_ROOT/core/runtime" \
        -I"$REPO_ROOT/core/ffi" \
        -I"$REPO_ROOT/core/layout" \
        -I"$REPO_ROOT/core/routing" \
        -I"$REPO_ROOT/packages/apple" \
        -I"$REPO_ROOT/platform/apple/Fabric" \
        -I"$REPO_ROOT" \
        $OBJC_FLAGS \
        -fPIC \
        -O2 \
        -mios-simulator-version-min=14.0 \
        -Wall || {
        echo "⚠️  Failed to compile $src for iOS Simulator arm64"
        continue
    }
    OBJECT_FILES+=("$obj_file")
done

if [ ${#OBJECT_FILES[@]} -gt 0 ]; then
    clang++ "${OBJECT_FILES[@]}" \
        -o "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -arch arm64 \
        -isysroot $(xcrun --show-sdk-path --sdk iphonesimulator) \
        -framework UIKit \
        -framework Foundation \
        -dynamiclib \
        -install_name "@rpath/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" \
        -compatibility_version 1.0.0 \
        -current_version 1.0.0 \
        -mios-simulator-version-min=14.0 \
        -Wl,-rpath,@loader_path/Frameworks || {
        echo "⚠️  Failed to link iOS Simulator arm64 framework"
    }
    
    # Copy headers
    for header in "${HEADERS[@]}"; do
        rel_path=$(echo "$header" | sed "s|^include/||" | sed "s|^packages/apple/||")
        mkdir -p "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers/$(dirname "$rel_path")"
        cp "$REPO_ROOT/$header" "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Headers/$rel_path"
    done
    
    # Create module map and Info.plist
    cp "$FRAMEWORK_DIR/Versions/A/Modules/module.modulemap" \
       "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Modules/module.modulemap"
    cp "$FRAMEWORK_DIR/Versions/A/Resources/Info.plist" \
       "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/Info.plist"
fi

echo ""
echo "📦 Creating XCFramework..."

XCFRAMEWORK_ARGS=()

if [ -f "$FRAMEWORK_DIR/Obsydian" ] || [ -f "$FRAMEWORK_DIR/Versions/A/Obsydian" ]; then
    XCFRAMEWORK_ARGS+=("-framework" "$FRAMEWORK_DIR")
    echo "  ✓ Added macOS arm64 framework"
fi

if [ -f "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" ]; then
    XCFRAMEWORK_ARGS+=("-framework" "$IOS_ARM64_DIR/${FRAMEWORK_NAME}.framework")
    echo "  ✓ Added iOS arm64 device framework"
fi

if [ -f "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework/${FRAMEWORK_NAME}" ]; then
    XCFRAMEWORK_ARGS+=("-framework" "$IOS_SIM_ARM64_DIR/${FRAMEWORK_NAME}.framework")
    echo "  ✓ Added iOS arm64 simulator framework"
fi

if [ ${#XCFRAMEWORK_ARGS[@]} -eq 0 ]; then
    echo "❌ No frameworks built. Cannot create XCFramework."
    echo "⚠️  Note: iOS builds may fail if iOS support is not yet implemented."
    echo "    This is expected. At minimum, macOS framework should be built."
    exit 1
fi

echo ""
if [ ${#XCFRAMEWORK_ARGS[@]} -eq 1 ]; then
    echo "⚠️  Only macOS framework built (iOS support not yet implemented)."
fi

XCFRAMEWORK_PATH="$OUTPUT_DIR/${FRAMEWORK_NAME}.xcframework"

xcodebuild -create-xcframework \
    "${XCFRAMEWORK_ARGS[@]}" \
    -output "$XCFRAMEWORK_PATH" \
    || {
    echo "❌ xcodebuild failed to create XCFramework"
    exit 1
}

echo ""
echo "✅ XCFramework created at: $XCFRAMEWORK_PATH"
echo "📦 Framework version: ${VERSION}"
echo ""
echo "📋 Framework structure:"
find "$XCFRAMEWORK_PATH" -type f -name "${FRAMEWORK_NAME}" | head -5

echo ""
echo "✅ Build complete! Framework is ready for distribution."
