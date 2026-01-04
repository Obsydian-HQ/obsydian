/**
 * GUI Tests: macOS HStack Layout
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual HStack layout creation and visual verification.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_hstack_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include <obsidian/obsidian.h>

using namespace obsidian;

TEST(MacOSHStackGUI, CanCreateHStackWithButtons) {
    // Initialize platform first
    App app;
    ASSERT_TRUE(app.initialize());
    
    // Create a window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "HStack Test"));
    
    // Create HStack
    HStack hstack;
    hstack.setSpacing(10.0);
    hstack.setAlignment(layout::Alignment::Middle);
    hstack.setPadding(Padding::all(20.0));
    
    // Create buttons
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    Button button3;
    ASSERT_TRUE(button3.create("Button 3", 0, 0, 100, 30));
    
    // Add HStack to window
    hstack.addToWindow(window);
    EXPECT_TRUE(hstack.isValid());
    
    // Add children
    hstack.addChild(button1);
    hstack.addChild(button2);
    hstack.addChild(button3);
    
    // Show window (for manual verification)
    // In automated tests, we don't show to avoid blocking
    // window.show();
    
    // Cleanup
    window.close();
    app.shutdown();
}

TEST(MacOSHStackGUI, HStackSpacingWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "HStack Spacing Test"));
    
    HStack hstack;
    hstack.setSpacing(20.0);
    hstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 100, 30));
    
    hstack.addChild(button1);
    hstack.addChild(button2);
    
    // Verify spacing is set
    EXPECT_DOUBLE_EQ(hstack.getSpacing(), 20.0);
    
    window.close();
    app.shutdown();
}

TEST(MacOSHStackGUI, HStackAlignmentWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "HStack Alignment Test"));
    
    HStack hstack;
    hstack.setAlignment(layout::Alignment::Bottom);
    hstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    hstack.addChild(button1);
    
    // Verify alignment is set
    EXPECT_EQ(hstack.getAlignment(), layout::Alignment::Bottom);
    
    window.close();
    app.shutdown();
}

TEST(MacOSHStackGUI, HStackPaddingWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "HStack Padding Test"));
    
    HStack hstack;
    Padding padding = Padding::all(30.0);
    hstack.setPadding(padding);
    hstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    hstack.addChild(button1);
    
    // Verify padding is set
    Padding retrieved = hstack.getPadding();
    EXPECT_DOUBLE_EQ(retrieved.top, 30.0);
    
    window.close();
    app.shutdown();
}

#else
// Non-Apple platforms
TEST(MacOSHStackGUI, NotImplemented) {
    GTEST_SKIP() << "HStack GUI tests not implemented on this platform";
}
#endif
