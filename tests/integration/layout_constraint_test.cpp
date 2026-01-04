/**
 * Integration test: Layout Constraint API
 * Tests that the layout constraint system can create, activate, and destroy constraints
 */

#include <gtest/gtest.h>
#include <memory>

#ifdef __APPLE__

#include "macos_layout_ffi.h"
#include "macos_ffi.h"
#include "macos_button.h"

using namespace obsidian::ffi::macos;

class LayoutConstraintTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize platform (required for NSApplication)
        ASSERT_TRUE(initializePlatform());
        
        // Create buttons to get view handles for constraints
        button1_ = std::make_unique<Button>();
        ASSERT_TRUE(button1_->create("Button 1", 10, 10, 100, 30));
        ASSERT_TRUE(button1_->isValid());
        
        button2_ = std::make_unique<Button>();
        ASSERT_TRUE(button2_->create("Button 2", 120, 10, 100, 30));
        ASSERT_TRUE(button2_->isValid());
    }
    
    void TearDown() override {
        button1_.reset();
        button2_.reset();
        cleanupPlatform();
    }
    
    std::unique_ptr<Button> button1_;
    std::unique_ptr<Button> button2_;
};

TEST_F(LayoutConstraintTest, ConstraintCreationWorks) {
    LayoutConstraint constraint;
    
    void* view1 = obsidian_macos_button_get_view(button1_->getHandle());
    ASSERT_NE(view1, nullptr);
    
    ObsidianLayoutConstraintParams params;
    params.firstView = view1;
    params.firstAttribute = ObsidianLayoutAttributeWidth;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = nullptr;
    params.secondAttribute = ObsidianLayoutAttributeWidth;  // Not used when secondView is null
    params.multiplier = 1.0;
    params.constant = 200.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    EXPECT_TRUE(created);
    EXPECT_TRUE(constraint.isValid());
}

TEST_F(LayoutConstraintTest, ConstraintActivationWorks) {
    LayoutConstraint constraint;
    
    void* view1 = obsidian_macos_button_get_view(button1_->getHandle());
    ASSERT_NE(view1, nullptr);
    
    ObsidianLayoutConstraintParams params;
    params.firstView = view1;
    params.firstAttribute = ObsidianLayoutAttributeWidth;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = nullptr;
    params.secondAttribute = ObsidianLayoutAttributeWidth;
    params.multiplier = 1.0;
    params.constant = 200.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    ASSERT_TRUE(created);
    
    // Activate constraint
    constraint.activate();
    EXPECT_TRUE(constraint.isValid());
}

TEST_F(LayoutConstraintTest, ConstraintDeactivationWorks) {
    LayoutConstraint constraint;
    
    void* view1 = obsidian_macos_button_get_view(button1_->getHandle());
    ASSERT_NE(view1, nullptr);
    
    ObsidianLayoutConstraintParams params;
    params.firstView = view1;
    params.firstAttribute = ObsidianLayoutAttributeWidth;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = nullptr;
    params.secondAttribute = ObsidianLayoutAttributeWidth;
    params.multiplier = 1.0;
    params.constant = 200.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    ASSERT_TRUE(created);
    
    constraint.activate();
    EXPECT_TRUE(constraint.isValid());
    
    // Deactivate constraint
    constraint.deactivate();
    EXPECT_TRUE(constraint.isValid());
}

TEST_F(LayoutConstraintTest, ConstraintDestructionWorks) {
    LayoutConstraint constraint;
    
    void* view1 = obsidian_macos_button_get_view(button1_->getHandle());
    ASSERT_NE(view1, nullptr);
    
    ObsidianLayoutConstraintParams params;
    params.firstView = view1;
    params.firstAttribute = ObsidianLayoutAttributeWidth;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = nullptr;
    params.secondAttribute = ObsidianLayoutAttributeWidth;
    params.multiplier = 1.0;
    params.constant = 200.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    ASSERT_TRUE(created);
    
    constraint.activate();
    EXPECT_TRUE(constraint.isValid());
    
    // Destroy constraint
    constraint.destroy();
    EXPECT_FALSE(constraint.isValid());
}

TEST_F(LayoutConstraintTest, ConstraintBetweenViewsWorks) {
    // Create a window to hold the views (required for constraint activation)
    Window window;
    ASSERT_TRUE(window.create(400, 300, "Test Window"));
    
    // Add buttons to window so they're in a view hierarchy
    button1_->addToWindow(window);
    button2_->addToWindow(window);
    
    LayoutConstraint constraint;
    
    void* view1 = obsidian_macos_button_get_view(button1_->getHandle());
    void* view2 = obsidian_macos_button_get_view(button2_->getHandle());
    ASSERT_NE(view1, nullptr);
    ASSERT_NE(view2, nullptr);
    
    ObsidianLayoutConstraintParams params;
    params.firstView = view1;
    params.firstAttribute = ObsidianLayoutAttributeLeading;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = view2;
    params.secondAttribute = ObsidianLayoutAttributeTrailing;
    params.multiplier = 1.0;
    params.constant = 20.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    EXPECT_TRUE(created);
    EXPECT_TRUE(constraint.isValid());
    
    // Activation requires views to be in a view hierarchy
    constraint.activate();
    EXPECT_TRUE(constraint.isValid());
    
    window.close();
}

TEST_F(LayoutConstraintTest, InvalidConstraintFails) {
    LayoutConstraint constraint;
    
    // Try to create constraint with null firstView
    ObsidianLayoutConstraintParams params;
    params.firstView = nullptr;
    params.firstAttribute = ObsidianLayoutAttributeWidth;
    params.relation = ObsidianLayoutRelationEqual;
    params.secondView = nullptr;
    params.secondAttribute = ObsidianLayoutAttributeWidth;
    params.multiplier = 1.0;
    params.constant = 200.0;
    params.priority = 1000.0;
    
    bool created = constraint.create(params);
    EXPECT_FALSE(created);
    EXPECT_FALSE(constraint.isValid());
}

#else
// Non-Apple platforms
TEST(LayoutConstraintTest, NotImplemented) {
    GTEST_SKIP() << "Layout constraint API not implemented on this platform";
}
#endif
