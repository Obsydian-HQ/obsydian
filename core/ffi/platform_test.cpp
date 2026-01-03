/**
 * Unit tests for platform detection and FFI layer
 */

#include <gtest/gtest.h>
#include "platform.h"

using namespace obsidian::ffi;

TEST(PlatformFFI, CanDetectPlatform) {
    // Test that we can detect the current platform
    Platform platform = getCurrentPlatform();
    
    // Should not be Unknown
    EXPECT_NE(platform, Platform::Unknown);
    
    // On macOS, should detect macOS
    #if defined(__APPLE__) && !TARGET_OS_IPHONE
        EXPECT_EQ(platform, Platform::macOS);
    #elif defined(__APPLE__) && TARGET_OS_IPHONE
        EXPECT_EQ(platform, Platform::iOS);
    #elif defined(__ANDROID__)
        EXPECT_EQ(platform, Platform::Android);
    #elif defined(_WIN32) || defined(_WIN64)
        EXPECT_EQ(platform, Platform::Windows);
    #elif defined(__linux__)
        EXPECT_EQ(platform, Platform::Linux);
    #endif
}

TEST(PlatformFFI, CanInitializePlatform) {
    // Test that platform initialization succeeds
    bool initialized = initializePlatform();
    EXPECT_TRUE(initialized);
    
    // Cleanup
    cleanupPlatform();
}

TEST(PlatformFFI, PlatformInitializationIsIdempotent) {
    // Test that we can initialize multiple times
    EXPECT_TRUE(initializePlatform());
    EXPECT_TRUE(initializePlatform());
    
    cleanupPlatform();
    cleanupPlatform();
}

