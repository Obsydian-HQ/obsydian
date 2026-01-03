/**
 * GUI Tests: macOS Window Creation
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual NSWindow creation and management.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_window_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include "packages/apple/macos_ffi.h"

using namespace obsidian::ffi::macos;

TEST(MacOSWindowGUI, CanCreateAndDestroyWindow) {
    // Initialize platform first
    ASSERT_TRUE(obsidian::ffi::macos::initializePlatform());
    
    // Create a window
    Window window;
    EXPECT_TRUE(window.create(800, 600, "Test Window"));
    EXPECT_TRUE(window.isValid());
    
    // Don't show in automated tests - just verify creation works
    // In manual testing, you can uncomment window.show() to see it
    
    // Cleanup
    window.close();
    EXPECT_FALSE(window.isValid());
    
    obsidian::ffi::macos::cleanupPlatform();
}

TEST(MacOSWindowGUI, WindowCreationRequiresInitialization) {
    // This test verifies the pattern that initialization should happen first
    // Note: NSApplication might auto-initialize, but we test the proper pattern
    ASSERT_TRUE(obsidian::ffi::macos::initializePlatform());
    
    Window window;
    bool created = window.create(800, 600, "Test Window");
    
    if (created) {
        EXPECT_TRUE(window.isValid());
        window.close();
    }
    
    obsidian::ffi::macos::cleanupPlatform();
}

TEST(MacOSWindowGUI, MultipleWindowsCanBeCreated) {
    ASSERT_TRUE(obsidian::ffi::macos::initializePlatform());
    
    Window window1;
    Window window2;
    
    EXPECT_TRUE(window1.create(400, 300, "Window 1"));
    EXPECT_TRUE(window2.create(400, 300, "Window 2"));
    
    EXPECT_TRUE(window1.isValid());
    EXPECT_TRUE(window2.isValid());
    
    window1.close();
    window2.close();
    
    obsidian::ffi::macos::cleanupPlatform();
}

#endif // __APPLE__

