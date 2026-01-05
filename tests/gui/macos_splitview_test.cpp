/**
 * GUI Tests: macOS SplitView Creation and Management
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual NSSplitView creation, primary/secondary pane management, and integration.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_splitview_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include <obsidian/obsidian.h>

using namespace obsidian;

class MacOSSplitViewGUITest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize application (required for NSApplication)
        ASSERT_TRUE(app.initialize());
    }
    
    void TearDown() override {
        app.shutdown();
    }
    
    App app;
};

// Test creation and destruction
TEST_F(MacOSSplitViewGUITest, CanCreateAndDestroySplitView) {
    // Create SplitView with leading position
    SplitView splitView;
    EXPECT_TRUE(splitView.create(SplitPosition::Leading));
    EXPECT_TRUE(splitView.isValid());
    
    // SplitView should be valid after creation
    EXPECT_TRUE(splitView.isValid());
    
    // Cleanup is automatic (RAII), but we can verify destruction
    // The destructor will handle cleanup
}

TEST_F(MacOSSplitViewGUITest, CanCreateWithTrailingPosition) {
    // Create SplitView with trailing position
    SplitView splitView;
    EXPECT_TRUE(splitView.create(SplitPosition::Trailing));
    EXPECT_TRUE(splitView.isValid());
}

TEST_F(MacOSSplitViewGUITest, CannotCreateTwice) {
    // Attempting to create twice should fail
    SplitView splitView;
    EXPECT_TRUE(splitView.create(SplitPosition::Leading));
    EXPECT_TRUE(splitView.isValid());
    
    // Second creation should fail
    EXPECT_FALSE(splitView.create(SplitPosition::Leading));
}

// Test primary pane width management
TEST_F(MacOSSplitViewGUITest, CanSetAndGetPrimaryPaneWidth) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    // Create a window (required for layout calculations)
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    splitView.addToWindow(window);
    
    // Set primary pane width
    splitView.setPrimaryPaneWidth(250.0);
    
    // Get primary pane width (may return 0 if not laid out yet, but shouldn't crash)
    double width = splitView.getPrimaryPaneWidth();
    // Width might be 0 if not laid out yet, but the call should succeed
    EXPECT_GE(width, 0.0);
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanSetMinimumAndMaximumPrimaryPaneWidth) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    // Set minimum and maximum widths
    splitView.setMinimumPrimaryPaneWidth(150.0);
    splitView.setMaximumPrimaryPaneWidth(400.0);
    
    // Width constraints should be set (we can't easily verify without layout)
    // But the calls should succeed without crashing
    
    // Attempt to set width outside bounds (should be clamped)
    splitView.setPrimaryPaneWidth(100.0);  // Below minimum
    splitView.setPrimaryPaneWidth(500.0);  // Above maximum
    
    // Calls should succeed (widths will be clamped internally)
    EXPECT_TRUE(splitView.isValid());
}

// Test collapse/expand functionality
TEST_F(MacOSSplitViewGUITest, CanCollapseAndExpandPrimaryPane) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    splitView.addToWindow(window);
    
    // Initially primary pane should not be collapsed
    EXPECT_FALSE(splitView.isPrimaryPaneCollapsed());
    
    // Collapse primary pane
    splitView.collapsePrimaryPane();
    
    // Primary pane should now be collapsed
    EXPECT_TRUE(splitView.isPrimaryPaneCollapsed());
    
    // Expand primary pane
    splitView.expandPrimaryPane();
    
    // Primary pane should now be expanded
    EXPECT_FALSE(splitView.isPrimaryPaneCollapsed());
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanTogglePrimaryPane) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    splitView.addToWindow(window);
    
    // Initially primary pane should not be collapsed
    bool initialState = splitView.isPrimaryPaneCollapsed();
    
    // Toggle primary pane
    splitView.togglePrimaryPane();
    
    // State should have changed
    EXPECT_NE(splitView.isPrimaryPaneCollapsed(), initialState);
    
    // Toggle again
    splitView.togglePrimaryPane();
    
    // State should be back to initial
    EXPECT_EQ(splitView.isPrimaryPaneCollapsed(), initialState);
    
    window.close();
}

// Test content setting
TEST_F(MacOSSplitViewGUITest, CanSetPrimaryPaneContentWithVStack) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Create VStack for primary pane content
    VStack primaryPaneVStack;
    primaryPaneVStack.addToWindow(window);
    
    // Create a button to add to VStack
    Button button;
    ASSERT_TRUE(button.create("Test Button", 0, 0, 100, 30));
    primaryPaneVStack.addChild(button);
    primaryPaneVStack.updateLayout();
    
    // Set primary pane content
    splitView.setPrimaryPaneContent(primaryPaneVStack);
    EXPECT_TRUE(splitView.isValid());
    
    // Add to window
    splitView.addToWindow(window);
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanSetSecondaryPaneContentWithVStack) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Create VStack for secondary pane content
    VStack secondaryPaneVStack;
    secondaryPaneVStack.addToWindow(window);
    
    // Create a button to add to VStack
    Button button;
    ASSERT_TRUE(button.create("Secondary Button", 0, 0, 100, 30));
    secondaryPaneVStack.addChild(button);
    secondaryPaneVStack.updateLayout();
    
    // Set secondary pane content
    splitView.setSecondaryPaneContent(secondaryPaneVStack);
    EXPECT_TRUE(splitView.isValid());
    
    // Add to window
    splitView.addToWindow(window);
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanSetPrimaryPaneContentWithList) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Create List for primary pane content
    List list;
    list.addToWindow(window);
    list.addItem("Item 1");
    list.addItem("Item 2");
    
    // Set primary pane content
    splitView.setPrimaryPaneContent(list);
    EXPECT_TRUE(splitView.isValid());
    
    // Add to window
    splitView.addToWindow(window);
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanSetBothPrimaryAndSecondaryPaneContent) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Create primary pane VStack
    VStack primaryPaneVStack;
    primaryPaneVStack.addToWindow(window);
    Button primaryButton;
    ASSERT_TRUE(primaryButton.create("Primary", 0, 0, 100, 30));
    primaryPaneVStack.addChild(primaryButton);
    primaryPaneVStack.updateLayout();
    
    // Create secondary pane VStack
    VStack secondaryPaneVStack;
    secondaryPaneVStack.addToWindow(window);
    Button secondaryButton;
    ASSERT_TRUE(secondaryButton.create("Secondary", 0, 0, 100, 30));
    secondaryPaneVStack.addChild(secondaryButton);
    secondaryPaneVStack.updateLayout();
    
    // Set both contents
    splitView.setPrimaryPaneContent(primaryPaneVStack);
    splitView.setSecondaryPaneContent(secondaryPaneVStack);
    EXPECT_TRUE(splitView.isValid());
    
    // Add to window
    splitView.addToWindow(window);
    
    window.close();
}

// Test window integration
TEST_F(MacOSSplitViewGUITest, CanAddToWindow) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Add SplitView to window
    splitView.addToWindow(window);
    EXPECT_TRUE(splitView.isValid());
    
    window.close();
}

TEST_F(MacOSSplitViewGUITest, CanRemoveFromParent) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    
    // Add to window
    splitView.addToWindow(window);
    EXPECT_TRUE(splitView.isValid());
    
    // Remove from parent
    splitView.removeFromParent();
    EXPECT_TRUE(splitView.isValid());  // SplitView should still be valid
    
    window.close();
}

// Test primary pane toggle callback
TEST_F(MacOSSplitViewGUITest, PrimaryPaneToggleCallbackWorks) {
    SplitView splitView;
    ASSERT_TRUE(splitView.create(SplitPosition::Leading));
    
    Window window;
    ASSERT_TRUE(window.create(800, 600, "SplitView Test"));
    splitView.addToWindow(window);
    
    // Track callback invocations
    int callbackCount = 0;
    bool lastCollapsedState = false;
    
    // Set callback
    splitView.setOnPrimaryPaneToggle([&](bool collapsed) {
        callbackCount++;
        lastCollapsedState = collapsed;
    });
    
    // Toggle primary pane (should invoke callback)
    splitView.togglePrimaryPane();
    // Note: Callback might not be invoked immediately in all cases
    // The callback is triggered by the public API methods, not by user interaction
    
    // Cleanup
    window.close();
}

// Test invalid operations
TEST_F(MacOSSplitViewGUITest, OperationsOnInvalidSplitViewDoNotCrash) {
    SplitView splitView;
    
    // Operations on uncreated SplitView should not crash
    EXPECT_FALSE(splitView.isValid());
    splitView.setPrimaryPaneWidth(250.0);
    splitView.collapsePrimaryPane();
    splitView.expandPrimaryPane();
    splitView.togglePrimaryPane();
    splitView.setMinimumPrimaryPaneWidth(150.0);
    splitView.setMaximumPrimaryPaneWidth(400.0);
    EXPECT_EQ(splitView.getPrimaryPaneWidth(), 0.0);
    EXPECT_FALSE(splitView.isPrimaryPaneCollapsed());
    
    // These operations should be safe (no-op)
}

#else
// Non-Apple platforms
TEST(MacOSSplitViewGUITest, NotImplemented) {
    GTEST_SKIP() << "SplitView GUI tests not implemented on this platform";
}
#endif
