/**
 * Integration test: Spacer API
 * Tests that the Spacer component can be created, configured, and works correctly
 */

#include <gtest/gtest.h>
#include <memory>

#ifdef __APPLE__

#include "obsidian/spacer.h"
#include "macos_ffi.h"

using namespace obsidian;
using namespace obsidian::ffi::macos;

class SpacerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize platform (required for NSApplication)
        ASSERT_TRUE(initializePlatform());
    }
    
    void TearDown() override {
        cleanupPlatform();
    }
};

TEST_F(SpacerTest, SpacerCreationWorks) {
    Spacer spacer;
    
    // Create spacer without constraints
    bool created = spacer.create();
    EXPECT_TRUE(created);
    EXPECT_TRUE(spacer.isValid());
    EXPECT_NE(spacer.getNativeViewHandle(), nullptr);
}

TEST_F(SpacerTest, SpacerCreationWithMinSizeWorks) {
    Spacer spacer;
    
    // Create spacer with minimum size constraints
    bool created = spacer.create(50.0, 30.0, 0.0, 0.0);
    EXPECT_TRUE(created);
    EXPECT_TRUE(spacer.isValid());
    
    EXPECT_DOUBLE_EQ(spacer.getMinWidth(), 50.0);
    EXPECT_DOUBLE_EQ(spacer.getMinHeight(), 30.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxWidth(), 0.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxHeight(), 0.0);
}

TEST_F(SpacerTest, SpacerCreationWithMaxSizeWorks) {
    Spacer spacer;
    
    // Create spacer with maximum size constraints
    bool created = spacer.create(0.0, 0.0, 200.0, 100.0);
    EXPECT_TRUE(created);
    EXPECT_TRUE(spacer.isValid());
    
    EXPECT_DOUBLE_EQ(spacer.getMinWidth(), 0.0);
    EXPECT_DOUBLE_EQ(spacer.getMinHeight(), 0.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxWidth(), 200.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxHeight(), 100.0);
}

TEST_F(SpacerTest, SpacerCreationWithMinMaxSizeWorks) {
    Spacer spacer;
    
    // Create spacer with both min and max size constraints
    bool created = spacer.create(50.0, 30.0, 200.0, 100.0);
    EXPECT_TRUE(created);
    EXPECT_TRUE(spacer.isValid());
    
    EXPECT_DOUBLE_EQ(spacer.getMinWidth(), 50.0);
    EXPECT_DOUBLE_EQ(spacer.getMinHeight(), 30.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxWidth(), 200.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxHeight(), 100.0);
}

TEST_F(SpacerTest, SpacerSetMinWidthWorks) {
    Spacer spacer;
    bool created = spacer.create();
    ASSERT_TRUE(created);
    
    spacer.setMinWidth(75.0);
    EXPECT_DOUBLE_EQ(spacer.getMinWidth(), 75.0);
    
    spacer.setMinWidth(100.0);
    EXPECT_DOUBLE_EQ(spacer.getMinWidth(), 100.0);
}

TEST_F(SpacerTest, SpacerSetMinHeightWorks) {
    Spacer spacer;
    bool created = spacer.create();
    ASSERT_TRUE(created);
    
    spacer.setMinHeight(40.0);
    EXPECT_DOUBLE_EQ(spacer.getMinHeight(), 40.0);
    
    spacer.setMinHeight(60.0);
    EXPECT_DOUBLE_EQ(spacer.getMinHeight(), 60.0);
}

TEST_F(SpacerTest, SpacerSetMaxWidthWorks) {
    Spacer spacer;
    bool created = spacer.create();
    ASSERT_TRUE(created);
    
    spacer.setMaxWidth(150.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxWidth(), 150.0);
    
    spacer.setMaxWidth(250.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxWidth(), 250.0);
}

TEST_F(SpacerTest, SpacerSetMaxHeightWorks) {
    Spacer spacer;
    bool created = spacer.create();
    ASSERT_TRUE(created);
    
    spacer.setMaxHeight(80.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxHeight(), 80.0);
    
    spacer.setMaxHeight(120.0);
    EXPECT_DOUBLE_EQ(spacer.getMaxHeight(), 120.0);
}

TEST_F(SpacerTest, SpacerCannotCreateTwice) {
    Spacer spacer;
    
    bool created1 = spacer.create();
    EXPECT_TRUE(created1);
    EXPECT_TRUE(spacer.isValid());
    
    // Attempting to create again should fail
    bool created2 = spacer.create();
    EXPECT_FALSE(created2);
}

TEST_F(SpacerTest, SpacerIsValidAfterCreation) {
    Spacer spacer;
    EXPECT_FALSE(spacer.isValid());
    
    bool created = spacer.create();
    ASSERT_TRUE(created);
    EXPECT_TRUE(spacer.isValid());
    EXPECT_NE(spacer.getNativeViewHandle(), nullptr);
}

TEST_F(SpacerTest, SpacerGetNativeViewHandleReturnsNullBeforeCreation) {
    Spacer spacer;
    EXPECT_EQ(spacer.getNativeViewHandle(), nullptr);
    
    bool created = spacer.create();
    ASSERT_TRUE(created);
    EXPECT_NE(spacer.getNativeViewHandle(), nullptr);
}

#endif // __APPLE__
