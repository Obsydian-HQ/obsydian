/**
 * Layout Engine Tests
 */

#include <gtest/gtest.h>
#include "engine.h"
#include "node.h"
#include "style.h"

using namespace obsidian::layout;

class LayoutEngineTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test basic node creation
TEST_F(LayoutEngineTest, NodeCreation) {
    LayoutNode node;
    EXPECT_EQ(node.getChildCount(), 0);
    EXPECT_EQ(node.getParent(), nullptr);
}

// Test adding children
TEST_F(LayoutEngineTest, AddChildren) {
    LayoutNode parent;
    LayoutNode child1;
    LayoutNode child2;
    
    parent.addChild(&child1);
    parent.addChild(&child2);
    
    EXPECT_EQ(parent.getChildCount(), 2);
    EXPECT_EQ(child1.getParent(), &parent);
    EXPECT_EQ(child2.getParent(), &parent);
}

// Test simple column layout (VStack)
TEST_F(LayoutEngineTest, SimpleColumnLayout) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(200);
    
    LayoutNode child1;
    child1.getStyle().height = LayoutValue::points(50);
    
    LayoutNode child2;
    child2.getStyle().height = LayoutValue::points(50);
    
    root.addChild(&child1);
    root.addChild(&child2);
    
    root.calculateLayout(100, 200);
    
    // Root should be 100x200
    EXPECT_FLOAT_EQ(root.getLayout().width, 100);
    EXPECT_FLOAT_EQ(root.getLayout().height, 200);
    
    // Child1 should be at top, full width
    EXPECT_FLOAT_EQ(child1.getLayout().left, 0);
    EXPECT_FLOAT_EQ(child1.getLayout().top, 0);
    EXPECT_FLOAT_EQ(child1.getLayout().width, 100);
    EXPECT_FLOAT_EQ(child1.getLayout().height, 50);
    
    // Child2 should be below child1
    EXPECT_FLOAT_EQ(child2.getLayout().left, 0);
    EXPECT_FLOAT_EQ(child2.getLayout().top, 50);
    EXPECT_FLOAT_EQ(child2.getLayout().width, 100);
    EXPECT_FLOAT_EQ(child2.getLayout().height, 50);
}

// Test simple row layout (HStack)
TEST_F(LayoutEngineTest, SimpleRowLayout) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Row;
    root.getStyle().width = LayoutValue::points(200);
    root.getStyle().height = LayoutValue::points(100);
    
    LayoutNode child1;
    child1.getStyle().width = LayoutValue::points(50);
    
    LayoutNode child2;
    child2.getStyle().width = LayoutValue::points(50);
    
    root.addChild(&child1);
    root.addChild(&child2);
    
    root.calculateLayout(200, 100);
    
    // Root should be 200x100
    EXPECT_FLOAT_EQ(root.getLayout().width, 200);
    EXPECT_FLOAT_EQ(root.getLayout().height, 100);
    
    // Child1 should be at left, full height
    EXPECT_FLOAT_EQ(child1.getLayout().left, 0);
    EXPECT_FLOAT_EQ(child1.getLayout().top, 0);
    EXPECT_FLOAT_EQ(child1.getLayout().width, 50);
    EXPECT_FLOAT_EQ(child1.getLayout().height, 100);
    
    // Child2 should be right of child1
    EXPECT_FLOAT_EQ(child2.getLayout().left, 50);
    EXPECT_FLOAT_EQ(child2.getLayout().top, 0);
    EXPECT_FLOAT_EQ(child2.getLayout().width, 50);
    EXPECT_FLOAT_EQ(child2.getLayout().height, 100);
}

// Test flexGrow (Spacer behavior)
TEST_F(LayoutEngineTest, FlexGrow) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(200);
    
    LayoutNode child1;
    child1.getStyle().height = LayoutValue::points(50);
    
    LayoutNode spacer;
    spacer.getStyle().flexGrow = 1.0f;  // Should take remaining space
    
    LayoutNode child2;
    child2.getStyle().height = LayoutValue::points(50);
    
    root.addChild(&child1);
    root.addChild(&spacer);
    root.addChild(&child2);
    
    root.calculateLayout(100, 200);
    
    // Child1: 50px
    EXPECT_FLOAT_EQ(child1.getLayout().height, 50);
    EXPECT_FLOAT_EQ(child1.getLayout().top, 0);
    
    // Spacer: should fill remaining space (200 - 50 - 50 = 100)
    EXPECT_FLOAT_EQ(spacer.getLayout().height, 100);
    EXPECT_FLOAT_EQ(spacer.getLayout().top, 50);
    
    // Child2: 50px at bottom
    EXPECT_FLOAT_EQ(child2.getLayout().height, 50);
    EXPECT_FLOAT_EQ(child2.getLayout().top, 150);
}

// Test padding
TEST_F(LayoutEngineTest, Padding) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(100);
    root.getStyle().setPadding(Edge::All, LayoutValue::points(10));
    
    LayoutNode child;
    child.getStyle().flexGrow = 1.0f;  // Fill remaining
    
    root.addChild(&child);
    
    root.calculateLayout(100, 100);
    
    // Child should be inset by padding
    EXPECT_FLOAT_EQ(child.getLayout().left, 10);
    EXPECT_FLOAT_EQ(child.getLayout().top, 10);
    EXPECT_FLOAT_EQ(child.getLayout().width, 80);   // 100 - 10 - 10
    EXPECT_FLOAT_EQ(child.getLayout().height, 80);  // 100 - 10 - 10
}

// Test gap
TEST_F(LayoutEngineTest, Gap) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(100);
    root.getStyle().gap = 10.0f;
    
    LayoutNode child1;
    child1.getStyle().height = LayoutValue::points(30);
    
    LayoutNode child2;
    child2.getStyle().height = LayoutValue::points(30);
    
    root.addChild(&child1);
    root.addChild(&child2);
    
    root.calculateLayout(100, 100);
    
    EXPECT_FLOAT_EQ(child1.getLayout().top, 0);
    EXPECT_FLOAT_EQ(child2.getLayout().top, 40);  // 30 + 10 gap
}

// Test JustifyContent::Center
TEST_F(LayoutEngineTest, JustifyContentCenter) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().justifyContent = JustifyContent::Center;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(200);
    
    LayoutNode child;
    child.getStyle().height = LayoutValue::points(50);
    
    root.addChild(&child);
    
    root.calculateLayout(100, 200);
    
    // Child should be centered vertically
    EXPECT_FLOAT_EQ(child.getLayout().top, 75);  // (200 - 50) / 2
}

// Test AlignItems::Center
TEST_F(LayoutEngineTest, AlignItemsCenter) {
    LayoutNode root;
    root.getStyle().flexDirection = FlexDirection::Column;
    root.getStyle().alignItems = AlignItems::Center;
    root.getStyle().width = LayoutValue::points(100);
    root.getStyle().height = LayoutValue::points(100);
    
    LayoutNode child;
    child.getStyle().width = LayoutValue::points(50);
    child.getStyle().height = LayoutValue::points(50);
    
    root.addChild(&child);
    
    root.calculateLayout(100, 100);
    
    // Child should be centered horizontally
    EXPECT_FLOAT_EQ(child.getLayout().left, 25);  // (100 - 50) / 2
    EXPECT_FLOAT_EQ(child.getLayout().width, 50);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
