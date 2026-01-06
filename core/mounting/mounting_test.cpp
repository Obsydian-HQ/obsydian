/**
 * Tests for ComponentViewRegistry and MountingCoordinator
 */

#include "component_view_registry.h"
#include "mounting_coordinator.h"
#include "view_mutation.h"
#include <gtest/gtest.h>

using namespace obsidian::mounting;

// Mock native view - just an integer to track creation
struct MockView {
    int id;
    static int nextId;
    MockView() : id(nextId++) {}
};
int MockView::nextId = 1;

class ComponentViewRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Register mock factories
        auto& registry = ComponentViewRegistry::getInstance();
        
        registry.registerViewFactory(
            ComponentHandles::Button,
            [](const ComponentHandle&) -> void* { return new MockView(); },
            [](void* view, const ComponentHandle&) { delete static_cast<MockView*>(view); },
            [](void* view, const ComponentHandle&) { /* prepare for recycle */ }
        );
        
        registry.registerViewFactory(
            ComponentHandles::VStack,
            [](const ComponentHandle&) -> void* { return new MockView(); },
            [](void* view, const ComponentHandle&) { delete static_cast<MockView*>(view); }
        );
    }
    
    void TearDown() override {
        // Clear registry between tests
        ComponentViewRegistry::getInstance().clearRecyclePool();
    }
};

TEST_F(ComponentViewRegistryTest, GeneratesUniqueTags) {
    auto& registry = ComponentViewRegistry::getInstance();
    
    Tag tag1 = registry.generateTag();
    Tag tag2 = registry.generateTag();
    Tag tag3 = registry.generateTag();
    
    EXPECT_NE(tag1, tag2);
    EXPECT_NE(tag2, tag3);
    EXPECT_NE(tag1, tag3);
}

TEST_F(ComponentViewRegistryTest, DequeueCreatesView) {
    auto& registry = ComponentViewRegistry::getInstance();
    Tag tag = registry.generateTag();
    
    auto descriptor = registry.dequeueComponentView(ComponentHandles::Button, tag);
    
    EXPECT_NE(descriptor.view, nullptr);
    EXPECT_EQ(descriptor.componentHandle, ComponentHandles::Button);
    EXPECT_TRUE(registry.hasTag(tag));
}

TEST_F(ComponentViewRegistryTest, EnqueueRecyclesView) {
    auto& registry = ComponentViewRegistry::getInstance();
    Tag tag = registry.generateTag();
    
    auto descriptor = registry.dequeueComponentView(ComponentHandles::Button, tag);
    void* originalView = descriptor.view;
    
    // Return to pool
    registry.enqueueComponentView(ComponentHandles::Button, tag);
    
    EXPECT_FALSE(registry.hasTag(tag));
    EXPECT_EQ(registry.recycledViewCount(ComponentHandles::Button), 1);
    
    // Dequeue again - should get recycled view
    Tag newTag = registry.generateTag();
    auto newDescriptor = registry.dequeueComponentView(ComponentHandles::Button, newTag);
    
    EXPECT_EQ(newDescriptor.view, originalView);  // Same view reused!
}

TEST_F(ComponentViewRegistryTest, FindNativeView) {
    auto& registry = ComponentViewRegistry::getInstance();
    Tag tag = registry.generateTag();
    
    auto descriptor = registry.dequeueComponentView(ComponentHandles::Button, tag);
    
    void* found = registry.findNativeView(tag);
    EXPECT_EQ(found, descriptor.view);
    
    void* notFound = registry.findNativeView(99999);
    EXPECT_EQ(notFound, nullptr);
}

// MountingCoordinator tests
class MountingCoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& registry = ComponentViewRegistry::getInstance();
        
        registry.registerViewFactory(
            ComponentHandles::Button,
            [](const ComponentHandle&) -> void* { return new MockView(); },
            [](void* view, const ComponentHandle&) { delete static_cast<MockView*>(view); }
        );
        
        registry.registerViewFactory(
            ComponentHandles::VStack,
            [](const ComponentHandle&) -> void* { return new MockView(); },
            [](void* view, const ComponentHandle&) { delete static_cast<MockView*>(view); }
        );
        
        // Set up mock platform delegate
        MountingPlatformDelegate delegate;
        insertCalls = 0;
        removeCalls = 0;
        updateCalls = 0;
        
        delegate.mountChildView = [this](void*, void*, int32_t) { insertCalls++; };
        delegate.unmountChildView = [this](void*, void*, int32_t) { removeCalls++; };
        delegate.updateLayoutMetrics = [this](void*, const LayoutMetrics&) { updateCalls++; };
        
        MountingCoordinator::getInstance().setPlatformDelegate(delegate);
    }
    
    void TearDown() override {
        ComponentViewRegistry::getInstance().clearRecyclePool();
    }
    
    int insertCalls = 0;
    int removeCalls = 0;
    int updateCalls = 0;
};

TEST_F(MountingCoordinatorTest, CreateMutation) {
    auto& coordinator = MountingCoordinator::getInstance();
    auto& registry = coordinator.getRegistry();
    
    Tag tag = registry.generateTag();
    auto mutation = ViewMutation::CreateMutation(tag, ComponentHandles::Button);
    
    coordinator.performMutation(mutation);
    
    EXPECT_TRUE(registry.hasTag(tag));
    EXPECT_NE(registry.findNativeView(tag), nullptr);
}

TEST_F(MountingCoordinatorTest, DeleteMutation) {
    auto& coordinator = MountingCoordinator::getInstance();
    auto& registry = coordinator.getRegistry();
    
    Tag tag = registry.generateTag();
    
    // First create
    coordinator.performMutation(ViewMutation::CreateMutation(tag, ComponentHandles::Button));
    EXPECT_TRUE(registry.hasTag(tag));
    
    // Then delete
    coordinator.performMutation(ViewMutation::DeleteMutation(tag, ComponentHandles::Button));
    EXPECT_FALSE(registry.hasTag(tag));
}

TEST_F(MountingCoordinatorTest, InsertMutation) {
    auto& coordinator = MountingCoordinator::getInstance();
    auto& registry = coordinator.getRegistry();
    
    Tag parentTag = registry.generateTag();
    Tag childTag = registry.generateTag();
    
    // Create parent and child
    coordinator.performMutation(ViewMutation::CreateMutation(parentTag, ComponentHandles::VStack));
    coordinator.performMutation(ViewMutation::CreateMutation(childTag, ComponentHandles::Button));
    
    // Insert child into parent
    coordinator.performMutation(ViewMutation::InsertMutation(parentTag, childTag, 0));
    
    EXPECT_EQ(insertCalls, 1);
}

TEST_F(MountingCoordinatorTest, UpdateMutation) {
    auto& coordinator = MountingCoordinator::getInstance();
    auto& registry = coordinator.getRegistry();
    
    Tag tag = registry.generateTag();
    coordinator.performMutation(ViewMutation::CreateMutation(tag, ComponentHandles::Button));
    
    LayoutMetrics oldMetrics{0, 0, 100, 50};
    LayoutMetrics newMetrics{10, 20, 200, 100};
    
    coordinator.performMutation(ViewMutation::UpdateMutation(tag, oldMetrics, newMetrics));
    
    EXPECT_EQ(updateCalls, 1);
}

TEST_F(MountingCoordinatorTest, MutationList) {
    auto& coordinator = MountingCoordinator::getInstance();
    auto& registry = coordinator.getRegistry();
    
    Tag parentTag = registry.generateTag();
    Tag child1Tag = registry.generateTag();
    Tag child2Tag = registry.generateTag();
    
    ViewMutationList mutations = {
        ViewMutation::CreateMutation(parentTag, ComponentHandles::VStack),
        ViewMutation::CreateMutation(child1Tag, ComponentHandles::Button),
        ViewMutation::CreateMutation(child2Tag, ComponentHandles::Button),
        ViewMutation::InsertMutation(parentTag, child1Tag, 0),
        ViewMutation::InsertMutation(parentTag, child2Tag, 1),
    };
    
    coordinator.performMutations(mutations);
    
    EXPECT_TRUE(registry.hasTag(parentTag));
    EXPECT_TRUE(registry.hasTag(child1Tag));
    EXPECT_TRUE(registry.hasTag(child2Tag));
    EXPECT_EQ(insertCalls, 2);
}
