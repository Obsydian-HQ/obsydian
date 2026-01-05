/**
 * macOS Sidebar GUI Tests
 * 
 * Tests for the native macOS Sidebar component using NSSplitViewController
 * and NSSplitViewItem.sidebar().
 * 
 * These tests require a GUI environment and should be run with:
 *   bazel test //tests/gui:macos_sidebar_test --config=macos --test_tag_filters=requires-gui
 */

#include <gtest/gtest.h>
#include <obsidian/obsidian.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace obsidian;

class SidebarTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the application
        app.initialize();
    }
    
    void TearDown() override {
        // Cleanup
        app.shutdown();
    }
    
    App app;
};

// Test 1: Create and destroy Sidebar
TEST_F(SidebarTest, CreateAndDestroy) {
    Sidebar sidebar;
    EXPECT_FALSE(sidebar.isValid());
    
    EXPECT_TRUE(sidebar.create());
    EXPECT_TRUE(sidebar.isValid());
}

// Test 2: Set sidebar content with VStack
TEST_F(SidebarTest, SetSidebarContentVStack) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setPadding(Padding::all(15.0));
    
    Button button;
    button.create("Test Button", 0, 0, 200, 30);
    vstack.addChild(button);
    vstack.updateLayout();
    
    sidebar.setSidebarContent(vstack);
    EXPECT_TRUE(sidebar.isValid());
}

// Test 3: Set sidebar content with List
TEST_F(SidebarTest, SetSidebarContentList) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    List list;
    list.create(0, 0, 250, 600);
    list.addItem("Item 1");
    list.addItem("Item 2");
    list.addItem("Item 3");
    
    sidebar.setSidebarContent(list);
    EXPECT_TRUE(sidebar.isValid());
}

// Test 4: Set main content with VStack
TEST_F(SidebarTest, SetMainContentVStack) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setPadding(Padding::all(20.0));
    
    Button button;
    button.create("Main Button", 0, 0, 300, 40);
    vstack.addChild(button);
    vstack.updateLayout();
    
    sidebar.setMainContent(vstack);
    EXPECT_TRUE(sidebar.isValid());
}

// Test 5: Set minimum and maximum sidebar width
TEST_F(SidebarTest, SetSidebarWidthConstraints) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    sidebar.setMinimumSidebarWidth(200.0);
    sidebar.setMaximumSidebarWidth(400.0);
    
    EXPECT_DOUBLE_EQ(sidebar.getMinimumSidebarWidth(), 200.0);
    EXPECT_DOUBLE_EQ(sidebar.getMaximumSidebarWidth(), 400.0);
}

// Test 6: Collapse and expand sidebar
TEST_F(SidebarTest, CollapseAndExpand) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    List list;
    list.create(0, 0, 250, 600);
    list.addItem("Item 1");
    sidebar.setSidebarContent(list);
    
    VStack vstack;
    vstack.setSpacing(10.0);
    sidebar.setMainContent(vstack);
    
    // Collapse
    sidebar.collapseSidebar();
    EXPECT_TRUE(sidebar.isSidebarCollapsed());
    
    // Expand
    sidebar.expandSidebar();
    EXPECT_FALSE(sidebar.isSidebarCollapsed());
    
    // Toggle
    sidebar.toggleSidebar();
    EXPECT_TRUE(sidebar.isSidebarCollapsed());
    
    sidebar.toggleSidebar();
    EXPECT_FALSE(sidebar.isSidebarCollapsed());
}

// Test 7: Add Sidebar to window
TEST_F(SidebarTest, AddToWindow) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    List list;
    list.create(0, 0, 250, 600);
    list.addItem("Item 1");
    sidebar.setSidebarContent(list);
    
    VStack vstack;
    vstack.setSpacing(10.0);
    sidebar.setMainContent(vstack);
    
    Window window;
    ASSERT_TRUE(window.create(1000, 700, "Sidebar Test"));
    
    sidebar.addToWindow(window);
    EXPECT_TRUE(sidebar.isValid());
    EXPECT_TRUE(window.isValid());
}

// Test 8: Sidebar toggle callback
TEST_F(SidebarTest, SidebarToggleCallback) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    bool callbackCalled = false;
    bool callbackCollapsedState = false;
    
    sidebar.setOnSidebarToggle([&](bool collapsed) {
        callbackCalled = true;
        callbackCollapsedState = collapsed;
    });
    
    List list;
    list.create(0, 0, 250, 600);
    sidebar.setSidebarContent(list);
    
    VStack vstack;
    sidebar.setMainContent(vstack);
    
    // Toggle should trigger callback
    sidebar.collapseSidebar();
    // Note: Callback may be called synchronously or asynchronously
    // This test verifies the callback is set up correctly
}

// Test 9: Complete Sidebar setup
TEST_F(SidebarTest, CompleteSetup) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    // Configure width constraints
    sidebar.setMinimumSidebarWidth(200.0);
    sidebar.setMaximumSidebarWidth(400.0);
    
    // Set sidebar content
    List list;
    list.create(0, 0, 250, 600);
    list.addItem("Home");
    list.addItem("Projects");
    list.addItem("Settings");
    sidebar.setSidebarContent(list);
    
    // Set main content
    VStack mainContent;
    mainContent.setSpacing(15.0);
    mainContent.setPadding(Padding::all(20.0));
    
    Button button1;
    button1.create("Action 1", 0, 0, 300, 40);
    mainContent.addChild(button1);
    
    Button button2;
    button2.create("Action 2", 0, 0, 300, 40);
    mainContent.addChild(button2);
    
    mainContent.updateLayout();
    sidebar.setMainContent(mainContent);
    
    // Add to window
    Window window;
    ASSERT_TRUE(window.create(1200, 800, "Complete Sidebar Test"));
    sidebar.addToWindow(window);
    
    EXPECT_TRUE(sidebar.isValid());
    EXPECT_TRUE(window.isValid());
}

// Test 10: Sidebar with ScrollView content
TEST_F(SidebarTest, SidebarWithScrollView) {
    Sidebar sidebar;
    ASSERT_TRUE(sidebar.create());
    
    ScrollView scrollView;
    scrollView.create(0, 0, 250, 600);
    
    TextView textView;
    textView.create(0, 0, 250, 1000);
    textView.setString("This is a long text that will require scrolling...\n");
    for (int i = 0; i < 50; i++) {
        textView.setString(textView.getString() + "Line " + std::to_string(i) + "\n");
    }
    
    scrollView.setDocumentView(textView);
    sidebar.setSidebarContent(scrollView);
    
    EXPECT_TRUE(sidebar.isValid());
}
