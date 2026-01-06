/**
 * Integration test: VStack API
 * Tests that VStack can be created, configured, and manages children
 */

#include <gtest/gtest.h>
#include <memory>

#ifdef __APPLE__

#include "macos_ffi.h"
#include "obsidian/vstack.h"
#include "obsidian/button.h"
#include "obsidian/window.h"

using namespace obsidian;

class VStackTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize platform (required for NSApplication)
        ASSERT_TRUE(obsidian::ffi::macos::initializePlatform());
    }
    
    void TearDown() override {
        obsidian::ffi::macos::cleanupPlatform();
    }
};

TEST_F(VStackTest, VStackCanBeCreated) {
    VStack vstack;
    
    // VStack is now eagerly initialized (frame-based layout approach)
    // The native container view is created immediately in the constructor
    // This makes the API more predictable and avoids lazy initialization issues
    EXPECT_TRUE(vstack.isValid());
}

TEST_F(VStackTest, VStackConfiguration) {
    VStack vstack;
    
    // Test spacing
    vstack.setSpacing(15.0);
    EXPECT_DOUBLE_EQ(vstack.getSpacing(), 15.0);
    
    // Test alignment
    vstack.setAlignment(layout::Alignment::Center);
    EXPECT_EQ(vstack.getAlignment(), layout::Alignment::Center);
    
    // Test padding
    Padding padding = Padding::all(10.0);
    vstack.setPadding(padding);
    Padding retrieved = vstack.getPadding();
    EXPECT_DOUBLE_EQ(retrieved.top, 10.0);
    EXPECT_DOUBLE_EQ(retrieved.bottom, 10.0);
    EXPECT_DOUBLE_EQ(retrieved.leading, 10.0);
    EXPECT_DOUBLE_EQ(retrieved.trailing, 10.0);
}

TEST_F(VStackTest, VStackCanAddChildren) {
    VStack vstack;
    
    // Create a window (required for button creation)
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Test Window"));
    
    // Create buttons
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    // Add VStack to window first
    vstack.addToWindow(window);
    EXPECT_TRUE(vstack.isValid());
    
    // Add children
    vstack.addChild(button1);
    vstack.addChild(button2);
    
    // VStack should still be valid
    EXPECT_TRUE(vstack.isValid());
    
    window.close();
}

TEST_F(VStackTest, VStackCanRemoveChildren) {
    VStack vstack;
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Test Window"));
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    vstack.addToWindow(window);
    vstack.addChild(button1);
    EXPECT_TRUE(vstack.isValid());
    
    // Remove child
    vstack.removeChild(button1);
    EXPECT_TRUE(vstack.isValid());
    
    window.close();
}

TEST_F(VStackTest, VStackCanClearChildren) {
    VStack vstack;
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Test Window"));
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    vstack.addToWindow(window);
    vstack.addChild(button1);
    vstack.addChild(button2);
    
    // Clear all children
    vstack.clearChildren();
    EXPECT_TRUE(vstack.isValid());
    
    window.close();
}

#else
// Non-Apple platforms
TEST(VStackTest, NotImplemented) {
    GTEST_SKIP() << "VStack API not implemented on this platform";
}
#endif
