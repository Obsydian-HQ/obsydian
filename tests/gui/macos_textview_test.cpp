/**
 * GUI Tests: macOS TextView Creation
 * 
 * These tests require a GUI environment (display server) to run.
 * They test actual NSTextView creation and management.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_textview_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include "packages/apple/macos_ffi.h"

using namespace obsidian::ffi::macos;

TEST(MacOSTextViewGUI, CanCreateAndDestroyTextView) {
    // Initialize platform first
    ASSERT_TRUE(initializePlatform());
    
    // Create a text view
    TextView textView;
    EXPECT_TRUE(textView.create(10, 10, 200, 100));
    EXPECT_TRUE(textView.isValid());
    
    // Test basic properties
    EXPECT_FALSE(textView.isEditable());  // Default is not editable
    EXPECT_TRUE(textView.isSelectable());  // Default is selectable
    EXPECT_TRUE(textView.isVisible());
    
    // Cleanup
    textView.removeFromParent();
    // TextView remains valid after removal - it's just not attached to a parent
    EXPECT_TRUE(textView.isValid());
    
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetTextViewString) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    
    // Initially empty
    EXPECT_EQ(textView.getString(), "");
    
    // Set text
    textView.setString("Hello, World!");
    EXPECT_EQ(textView.getString(), "Hello, World!");
    
    // Update text
    textView.setString("Updated text");
    EXPECT_EQ(textView.getString(), "Updated text");
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetTextViewEditable) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    
    // Default is not editable
    EXPECT_FALSE(textView.isEditable());
    
    // Make it editable
    textView.setEditable(true);
    EXPECT_TRUE(textView.isEditable());
    
    // Make it read-only again
    textView.setEditable(false);
    EXPECT_FALSE(textView.isEditable());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetTextViewSelectable) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    textView.setString("Selectable text");
    
    // Default is selectable
    EXPECT_TRUE(textView.isSelectable());
    
    // Make it not selectable
    textView.setSelectable(false);
    EXPECT_FALSE(textView.isSelectable());
    
    // Make it selectable again
    textView.setSelectable(true);
    EXPECT_TRUE(textView.isSelectable());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetTextViewVisibility) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    
    EXPECT_TRUE(textView.isVisible());
    
    textView.setVisible(false);
    EXPECT_FALSE(textView.isVisible());
    
    textView.setVisible(true);
    EXPECT_TRUE(textView.isVisible());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetTextViewEnabled) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    
    // Enabled state affects editability
    EXPECT_TRUE(textView.isEnabled());
    
    textView.setEnabled(false);
    EXPECT_FALSE(textView.isEnabled());
    EXPECT_FALSE(textView.isEditable());  // Disabled means not editable
    
    textView.setEnabled(true);
    EXPECT_TRUE(textView.isEnabled());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetFontSize) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    textView.setString("Test text");
    
    // Set font size
    textView.setFontSize(12.0);
    EXPECT_DOUBLE_EQ(textView.getFontSize(), 12.0);
    
    // Change font size
    textView.setFontSize(18.0);
    EXPECT_DOUBLE_EQ(textView.getFontSize(), 18.0);
    
    // Set very small font
    textView.setFontSize(8.0);
    EXPECT_DOUBLE_EQ(textView.getFontSize(), 8.0);
    
    // Set large font
    textView.setFontSize(24.0);
    EXPECT_DOUBLE_EQ(textView.getFontSize(), 24.0);
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanSetFontWeight) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    textView.setString("Test text");
    
    // Set different font weights
    textView.setFontWeight(0);  // Regular
    EXPECT_EQ(textView.getFontWeight(), 0);
    
    textView.setFontWeight(1);  // Bold
    EXPECT_EQ(textView.getFontWeight(), 1);
    
    textView.setFontWeight(2);  // Semibold
    EXPECT_EQ(textView.getFontWeight(), 2);
    
    textView.setFontWeight(0);  // Back to Regular
    EXPECT_EQ(textView.getFontWeight(), 0);
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanAddTextViewToWindow) {
    ASSERT_TRUE(initializePlatform());
    
    {
        // Create a window
        Window window;
        ASSERT_TRUE(window.create(400, 300, "Test Window"));
        
        // Create a text view
        TextView textView;
        ASSERT_TRUE(textView.create(50, 50, 300, 200));
        textView.setString("This is a TextView in a window");
        
        // Add to window
        textView.addToWindow(window);
        EXPECT_TRUE(textView.isValid());
        
        // Show window (for manual verification)
        window.show();
        
        // TextView should still be valid
        EXPECT_TRUE(textView.isValid());
        
        // Cleanup happens automatically when objects go out of scope
    }
    
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, CanCreateTextViewWithInitialText) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    // Note: The create method with text parameter is available in the public API
    // but the FFI layer uses the version without text, then setString
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    textView.setString("Initial text");
    
    EXPECT_EQ(textView.getString(), "Initial text");
    EXPECT_TRUE(textView.isValid());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, MultiLineTextView) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 300, 200));
    
    std::string multiLineText = 
        "Line 1: This is the first line\n"
        "Line 2: This is the second line\n"
        "Line 3: This is the third line\n"
        "Line 4: This is the fourth line";
    
    textView.setString(multiLineText);
    EXPECT_EQ(textView.getString(), multiLineText);
    
    // Verify it's still valid and can be edited
    textView.setEditable(true);
    EXPECT_TRUE(textView.isEditable());
    
    textView.removeFromParent();
    cleanupPlatform();
}

TEST(MacOSTextViewGUI, TextViewEditableAndSelectableCombinations) {
    ASSERT_TRUE(initializePlatform());
    
    TextView textView;
    ASSERT_TRUE(textView.create(10, 10, 200, 100));
    textView.setString("Test text");
    
    // Test 1: Not editable, not selectable
    textView.setEditable(false);
    textView.setSelectable(false);
    EXPECT_FALSE(textView.isEditable());
    EXPECT_FALSE(textView.isSelectable());
    
    // Test 2: Not editable, selectable (default for labels)
    textView.setEditable(false);
    textView.setSelectable(true);
    EXPECT_FALSE(textView.isEditable());
    EXPECT_TRUE(textView.isSelectable());
    
    // Test 3: Editable, selectable (default for text editors)
    textView.setEditable(true);
    textView.setSelectable(true);
    EXPECT_TRUE(textView.isEditable());
    EXPECT_TRUE(textView.isSelectable());
    
    // Test 4: Editable, not selectable (unusual but possible)
    textView.setEditable(true);
    textView.setSelectable(false);
    EXPECT_TRUE(textView.isEditable());
    EXPECT_FALSE(textView.isSelectable());
    
    textView.removeFromParent();
    cleanupPlatform();
}

#endif // __APPLE__
