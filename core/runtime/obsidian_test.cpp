/**
 * Unit tests for Obsidian runtime lifecycle
 */

#include <gtest/gtest.h>
#include "obsidian.h"
// Include public API for AppCallbacks definition
#include "obsidian/app.h"

using namespace obsidian;

TEST(ObsidianRuntime, CanCreateRuntime) {
    Runtime runtime;
    // Runtime should be constructible
    SUCCEED();
}

TEST(ObsidianRuntime, CanInitializeRuntime) {
    Runtime runtime;
    bool initialized = runtime.initialize();
    EXPECT_TRUE(initialized);
    
    // Should be able to initialize again (idempotent)
    EXPECT_TRUE(runtime.initialize());
}

TEST(ObsidianRuntime, HMRDefaultsToDisabled) {
    Runtime runtime;
    runtime.initialize();
    
    EXPECT_FALSE(runtime.isHMREnabled());
}

TEST(ObsidianRuntime, CanEnableHMR) {
    Runtime runtime;
    runtime.initialize();
    
    runtime.enableHMR(true);
    EXPECT_TRUE(runtime.isHMREnabled());
    
    runtime.enableHMR(false);
    EXPECT_FALSE(runtime.isHMREnabled());
}

TEST(ObsidianRuntime, CanRunWithCallbacks) {
    Runtime runtime;
    runtime.initialize();
    
    bool initCalled = false;
    bool updateCalled = false;
    bool shutdownCalled = false;
    
    AppCallbacks callbacks;
    callbacks.onInit = [&initCalled]() {
        initCalled = true;
    };
    callbacks.onUpdate = [&updateCalled]() {
        updateCalled = true;
    };
    callbacks.onShutdown = [&shutdownCalled]() {
        shutdownCalled = true;
    };
    
    // Run the runtime (will exit immediately due to placeholder implementation)
    runtime.run(callbacks);
    
    // Init should be called
    EXPECT_TRUE(initCalled);
    // Update may or may not be called depending on implementation
    // Shutdown should be called
    EXPECT_TRUE(shutdownCalled);
}

TEST(ObsidianRuntime, CanShutdown) {
    Runtime runtime;
    runtime.initialize();
    
    // Should be able to shutdown without errors
    runtime.shutdown();
    SUCCEED();
}

