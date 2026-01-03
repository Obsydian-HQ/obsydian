/**
 * Integration test: FFI → Platform bindings
 * Tests that C++ can actually call native platform APIs
 * 
 * These tests do NOT require a GUI environment and can run in headless CI.
 * For GUI-specific tests (NSWindow creation, etc.), see tests/gui/
 */

#include <gtest/gtest.h>
#include "core/ffi/platform.h"

using namespace obsidian::ffi;

TEST(FFIPlatformIntegration, PlatformDetectionWorks) {
    // Basic integration test: platform detection should work
    Platform platform = getCurrentPlatform();
    EXPECT_NE(platform, Platform::Unknown);
}

TEST(FFIPlatformIntegration, PlatformInitializationWorks) {
    // Test that platform initialization integrates properly
    // Note: On macOS, this initializes NSApplication but doesn't require GUI for basic init
    EXPECT_TRUE(initializePlatform());
    cleanupPlatform();
}

TEST(FFIPlatformIntegration, PlatformInitializationIsIdempotent) {
    // Test that we can initialize multiple times safely
    EXPECT_TRUE(initializePlatform());
    EXPECT_TRUE(initializePlatform());
    cleanupPlatform();
    cleanupPlatform();
}

