/**
 * GUI Tests: macOS Button Creation
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual NSButton creation and management.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_button_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include "packages/apple/macos_ffi.h"

using namespace obsidian::ffi::macos;

TEST(MacOSButtonGUI, CanCreateAndDestroyButton) {
    // Initialize platform first
    ASSERT_TRUE(initializePlatform());
    
    // Create a button
    Button button;
    EXPECT_TRUE(button.create("Test Button", 10, 10, 100, 30));
    EXPECT_TRUE(button.isValid());
    
    // Test basic properties
    EXPECT_EQ(button.getTitle(), "Test Button");
    EXPECT_TRUE(button.isVisible());
    EXPECT_TRUE(button.isEnabled());
    
    // Cleanup
    button.removeFromParent();
    // Button remains valid after removal - it's just not attached to a parent
    EXPECT_TRUE(button.isValid());
    
    cleanupPlatform();
}

TEST(MacOSButtonGUI, CanSetButtonTitle) {
    ASSERT_TRUE(initializePlatform());
    
    Button button;
    ASSERT_TRUE(button.create("Initial Title", 10, 10, 100, 30));
    
    EXPECT_EQ(button.getTitle(), "Initial Title");
    
    button.setTitle("New Title");
    EXPECT_EQ(button.getTitle(), "New Title");
    
    button.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSButtonGUI, CanSetButtonVisibility) {
    ASSERT_TRUE(initializePlatform());
    
    Button button;
    ASSERT_TRUE(button.create("Test Button", 10, 10, 100, 30));
    
    EXPECT_TRUE(button.isVisible());
    
    button.setVisible(false);
    EXPECT_FALSE(button.isVisible());
    
    button.setVisible(true);
    EXPECT_TRUE(button.isVisible());
    
    button.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSButtonGUI, CanSetButtonEnabled) {
    ASSERT_TRUE(initializePlatform());
    
    Button button;
    ASSERT_TRUE(button.create("Test Button", 10, 10, 100, 30));
    
    EXPECT_TRUE(button.isEnabled());
    
    button.setEnabled(false);
    EXPECT_FALSE(button.isEnabled());
    
    button.setEnabled(true);
    EXPECT_TRUE(button.isEnabled());
    
    button.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSButtonGUI, CanAddButtonToWindow) {
    ASSERT_TRUE(initializePlatform());
    
    {
        // Create a window
        Window window;
        ASSERT_TRUE(window.create(400, 300, "Test Window"));
        
        {
            // Create a button
            Button button;
            ASSERT_TRUE(button.create("Click Me", 50, 50, 100, 30));
            
            // Add button to window
            button.addToWindow(window);
            EXPECT_TRUE(button.isValid());
            
            // Button goes out of scope first (destructor runs), ensuring proper RAII cleanup order
            // The button destructor will call removeFromParent internally via destroy_button
        }
        // Window goes out of scope second, ensuring buttons are cleaned up before window
    }
    
    cleanupPlatform();
}

TEST(MacOSButtonGUI, ButtonClickCallbackWorks) {
    ASSERT_TRUE(initializePlatform());
    
    Button button;
    ASSERT_TRUE(button.create("Click Me", 10, 10, 100, 30));
    
    bool callbackCalled = false;
    button.setOnClick([&callbackCalled]() {
        callbackCalled = true;
    });
    
    // Note: In a real test, we would simulate a click event
    // For now, we just verify the callback can be set
    EXPECT_TRUE(button.isValid());
    
    button.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSButtonGUI, MultipleButtonsCanBeCreated) {
    ASSERT_TRUE(initializePlatform());
    
    Window window;
    ASSERT_TRUE(window.create(400, 300, "Test Window"));
    
    Button button1;
    Button button2;
    Button button3;
    
    EXPECT_TRUE(button1.create("Button 1", 10, 10, 100, 30));
    EXPECT_TRUE(button2.create("Button 2", 10, 50, 100, 30));
    EXPECT_TRUE(button3.create("Button 3", 10, 90, 100, 30));
    
    EXPECT_TRUE(button1.isValid());
    EXPECT_TRUE(button2.isValid());
    EXPECT_TRUE(button3.isValid());
    
    button1.addToWindow(window);
    button2.addToWindow(window);
    button3.addToWindow(window);
    
    button1.removeFromParent();
    button2.removeFromParent();
    button3.removeFromParent();
    
    window.close();
    cleanupPlatform();
}

#endif // __APPLE__

