/**
 * Shadow Tree Tests
 */

#include "shadow_node.h"
#include "shadow_tree.h"
#include "mounting_manager.h"
#include <gtest/gtest.h>
#include <vector>

using namespace obsidian::shadow;

class ShadowTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        tree_ = std::make_unique<ShadowTree>(1);
        mutations_.clear();
    }

    std::unique_ptr<ShadowTree> tree_;
    std::vector<ViewMutation> mutations_;
};

TEST_F(ShadowTreeTest, CreateTreeWithRootNode) {
    EXPECT_NE(tree_->getRootNode(), nullptr);
    EXPECT_EQ(tree_->getRootNode()->getComponentType(), ComponentType::Root);
}

TEST_F(ShadowTreeTest, CreateShadowNode) {
    auto* node = tree_->createNode(ComponentType::VStack);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->getComponentType(), ComponentType::VStack);
}

TEST_F(ShadowTreeTest, AddChildToRoot) {
    auto* root = tree_->getRootNode();
    auto* child = tree_->createNode(ComponentType::VStack);
    
    root->addChild(child);
    
    EXPECT_EQ(root->getChildCount(), 1);
    EXPECT_EQ(root->getChild(0), child);
    EXPECT_EQ(child->getParent(), root);
}

TEST_F(ShadowTreeTest, NestedChildren) {
    auto* root = tree_->getRootNode();
    auto* vstack = tree_->createNode(ComponentType::VStack);
    auto* hstack = tree_->createNode(ComponentType::HStack);
    auto* button = tree_->createNode(ComponentType::Button);
    
    root->addChild(vstack);
    vstack->addChild(hstack);
    hstack->addChild(button);
    
    EXPECT_EQ(root->getChildCount(), 1);
    EXPECT_EQ(vstack->getChildCount(), 1);
    EXPECT_EQ(hstack->getChildCount(), 1);
    EXPECT_EQ(button->getChildCount(), 0);
}

TEST_F(ShadowTreeTest, CommitGeneratesMutations) {
    auto* root = tree_->getRootNode();
    auto* vstack = tree_->createNode(ComponentType::VStack);
    
    // Set native views (mock pointers)
    void* mockRootView = reinterpret_cast<void*>(0x1000);
    void* mockVstackView = reinterpret_cast<void*>(0x2000);
    
    root->setNativeView(mockRootView);
    vstack->setNativeView(mockVstackView);
    
    root->addChild(vstack);
    
    // Set up mutation callback
    std::vector<ViewMutation> receivedMutations;
    tree_->setMountingCallback([&](const MutationList& mutations) {
        receivedMutations = mutations;
    });
    
    // Commit with 800x600 viewport
    bool result = tree_->commit(800.0f, 600.0f);
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(receivedMutations.empty());
    
    // Check that mutations include Update types with layout metrics
    bool hasRootUpdate = false;
    bool hasVstackUpdate = false;
    for (const auto& m : receivedMutations) {
        if (m.type == MutationType::Update && m.nativeView == mockRootView) {
            hasRootUpdate = true;
            EXPECT_EQ(m.layoutMetrics.width, 800.0f);
            EXPECT_EQ(m.layoutMetrics.height, 600.0f);
        }
        if (m.type == MutationType::Update && m.nativeView == mockVstackView) {
            hasVstackUpdate = true;
        }
    }
    
    EXPECT_TRUE(hasRootUpdate);
    EXPECT_TRUE(hasVstackUpdate);
}

TEST_F(ShadowTreeTest, LayoutMetricsComputed) {
    auto* root = tree_->getRootNode();
    auto* child1 = tree_->createNode(ComponentType::VStack);
    auto* child2 = tree_->createNode(ComponentType::VStack);
    
    // Configure children to split space
    child1->getStyle().flexGrow = 1.0f;
    child2->getStyle().flexGrow = 1.0f;
    
    root->addChild(child1);
    root->addChild(child2);
    
    tree_->commit(800.0f, 600.0f);
    
    // Root should be full size
    auto& rootMetrics = root->getLayoutMetrics();
    EXPECT_EQ(rootMetrics.x, 0.0f);
    EXPECT_EQ(rootMetrics.y, 0.0f);
    EXPECT_EQ(rootMetrics.width, 800.0f);
    EXPECT_EQ(rootMetrics.height, 600.0f);
    
    // Children should split height (Column layout)
    auto& child1Metrics = child1->getLayoutMetrics();
    auto& child2Metrics = child2->getLayoutMetrics();
    
    EXPECT_EQ(child1Metrics.width, 800.0f);
    EXPECT_EQ(child2Metrics.width, 800.0f);
    EXPECT_EQ(child1Metrics.height + child2Metrics.height, 600.0f);
}

TEST_F(ShadowTreeTest, RegistrySingleton) {
    auto& registry = ShadowTreeRegistry::getInstance();
    
    auto surfaceId = registry.generateSurfaceId();
    auto* tree1 = registry.createTree(surfaceId);
    auto* tree2 = registry.getTree(surfaceId);
    
    EXPECT_NE(tree1, nullptr);
    EXPECT_EQ(tree1, tree2);
    
    registry.removeTree(surfaceId);
    auto* tree3 = registry.getTree(surfaceId);
    EXPECT_EQ(tree3, nullptr);
}

TEST_F(ShadowTreeTest, HStackLayout) {
    auto* root = tree_->getRootNode();
    auto* hstack = tree_->createNode(ComponentType::HStack);
    auto* child1 = tree_->createNode(ComponentType::Button);
    auto* child2 = tree_->createNode(ComponentType::Button);
    
    // Set fixed sizes for buttons
    child1->getStyle().width = obsidian::layout::LayoutValue::points(100.0f);
    child1->getStyle().height = obsidian::layout::LayoutValue::points(40.0f);
    child2->getStyle().width = obsidian::layout::LayoutValue::points(100.0f);
    child2->getStyle().height = obsidian::layout::LayoutValue::points(40.0f);
    
    root->addChild(hstack);
    hstack->addChild(child1);
    hstack->addChild(child2);
    
    tree_->commit(800.0f, 600.0f);
    
    // Children should be side by side (Row layout)
    auto& child1Metrics = child1->getLayoutMetrics();
    auto& child2Metrics = child2->getLayoutMetrics();
    
    EXPECT_EQ(child1Metrics.x, 0.0f);
    EXPECT_EQ(child2Metrics.x, 100.0f);  // After child1
    EXPECT_EQ(child1Metrics.width, 100.0f);
    EXPECT_EQ(child2Metrics.width, 100.0f);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
