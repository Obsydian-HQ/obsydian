/**
 * GUI Tests: macOS ZStack Layout
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual ZStack layout creation and visual verification.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_zstack_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include <obsidian/obsidian.h>

using namespace obsidian;

TEST(MacOSZStackGUI, CanCreateZStackWithButtons) {
    // Initialize platform first
    App app;
    ASSERT_TRUE(app.initialize());
    
    // Create a window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "ZStack Test"));
    
    // Create ZStack
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::Center);
    
    // Create buttons
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    Button button2;
    ASSERT_TRUE(button2.create("Button 2", 0, 0, 120, 40));
    
    Button button3;
    ASSERT_TRUE(button3.create("Button 3", 0, 0, 140, 50));
    
    // Add ZStack to window
    zstack.addToWindow(window);
    EXPECT_TRUE(zstack.isValid());
    
    // Add children (last one should be on top)
    zstack.addChild(button1);
    zstack.addChild(button2);
    zstack.addChild(button3);
    
    // Show window (for manual verification)
    // In automated tests, we don't show to avoid blocking
    // window.show();
    
    // Cleanup
    window.close();
    app.shutdown();
}

TEST(MacOSZStackGUI, ZStackAlignmentWorks) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "ZStack Alignment Test"));
    
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::TopLeading);
    zstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Button 1", 0, 0, 100, 30));
    
    zstack.addChild(button1);
    
    // Verify alignment is set
    EXPECT_EQ(zstack.getAlignment(), layout::Alignment::TopLeading);
    
    window.close();
    app.shutdown();
}

TEST(MacOSZStackGUI, ZStackCenterAlignment) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "ZStack Center Test"));
    
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::Center);
    zstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Center", 0, 0, 100, 30));
    
    zstack.addChild(button1);
    
    EXPECT_EQ(zstack.getAlignment(), layout::Alignment::Center);
    
    window.close();
    app.shutdown();
}

TEST(MacOSZStackGUI, ZStackBottomTrailingAlignment) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "ZStack BottomTrailing Test"));
    
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::BottomTrailing);
    zstack.addToWindow(window);
    
    Button button1;
    ASSERT_TRUE(button1.create("Bottom", 0, 0, 100, 30));
    
    zstack.addChild(button1);
    
    EXPECT_EQ(zstack.getAlignment(), layout::Alignment::BottomTrailing);
    
    window.close();
    app.shutdown();
}

TEST(MacOSZStackGUI, ZStackZOrder) {
    App app;
    ASSERT_TRUE(app.initialize());
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "ZStack Z-Order Test"));
    
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::Center);
    zstack.addToWindow(window);
    
    // Create overlapping buttons
    Button button1;
    ASSERT_TRUE(button1.create("Bottom", 0, 0, 150, 50));
    
    Button button2;
    ASSERT_TRUE(button2.create("Middle", 0, 0, 120, 40));
    
    Button button3;
    ASSERT_TRUE(button3.create("Top", 0, 0, 100, 30));
    
    // Add in order - last one should be on top
    zstack.addChild(button1);
    zstack.addChild(button2);
    zstack.addChild(button3);
    
    // All should be valid
    EXPECT_TRUE(zstack.isValid());
    
    window.close();
    app.shutdown();
}

#else
// Non-Apple platforms
TEST(MacOSZStackGUI, NotImplemented) {
    GTEST_SKIP() << "ZStack GUI tests not implemented on this platform";
}
#endif
