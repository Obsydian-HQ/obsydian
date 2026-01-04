/**
 * GUI Tests: macOS VStack Layout
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual VStack layout creation and visual verification.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_vstack_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include <obsidian/obsidian.h>

using namespace obsidian;

TEST(MacOSVStackGUI, CanCreateVStackWithButtons) {
    // Initialize platform first
    App app;
    ASSERT_TRUE(app.initialize());
    
    // Create a window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "VStack Test"));
    
    // Create VStack
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::Center);
    vstack.setPadding(Padding::all(20.0));
    
    // Create buttons
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    Button button3;
    ASSERT_TRUE(button3.create("Button 3", 0, 0, 100, 30));
    
    // Add VStack to window
    vstack.addToWindow(window);
    EXPECT_TRUE(vstack.isValid());
    
    // Add children
    vstack.addChild(button1);
    vstack.addChild(button2);
    vstack.addChild(button3);
    
    // Show window (for manual verification)
    // In automated tests, we don't show to avoid blocking
    // window.show();
    
    // Cleanup
    window.close();
    app.shutdown();
}

TEST(MacOSVStackGUI, VStackSpacingWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "VStack Spacing Test"));
    
    VStack vstack;
    vstack.setSpacing(20.0);
    vstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    vstack.addChild(button1);
    vstack.addChild(button2);
    
    // Verify spacing is set
    EXPECT_DOUBLE_EQ(vstack.getSpacing(), 20.0);
    
    window.close();
    app.shutdown();
}

TEST(MacOSVStackGUI, VStackAlignmentWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "VStack Alignment Test"));
    
    VStack vstack;
    vstack.setAlignment(layout::Alignment::Trailing);
    vstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    vstack.addChild(button1);
    
    // Verify alignment is set
    EXPECT_EQ(vstack.getAlignment(), layout::Alignment::Trailing);
    
    window.close();
    app.shutdown();
}

TEST(MacOSVStackGUI, VStackPaddingWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "VStack Padding Test"));
    
    VStack vstack;
    Padding padding = Padding::all(30.0);
    vstack.setPadding(padding);
    vstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    vstack.addChild(button1);
    
    // Verify padding is set
    Padding retrieved = vstack.getPadding();
    EXPECT_DOUBLE_EQ(retrieved.top, 30.0);
    
    window.close();
    app.shutdown();
}

#else
// Non-Apple platforms
TEST(MacOSVStackGUI, NotImplemented) {
    GTEST_SKIP() << "VStack GUI tests not implemented on this platform";
}
#endif
