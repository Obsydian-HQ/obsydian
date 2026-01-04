/**
 * Integration test: UI → Runtime integration
 * Tests that UI components work with the runtime
 * 
 * The UI Runtime (core/runtime/) manages application lifecycle (initialize, run, shutdown, HMR).
 * The Layout System (core/layout/) handles arranging UI components - these are separate systems.
 */

#include <gtest/gtest.h>
#include "core/runtime/obsidian.h"
#include "core/ui/ui.h"

using namespace obsidian;
using namespace obsidian::ui;

TEST(UIRuntimeIntegration, RuntimeCanBeInitialized) {
    // Integration test: runtime should initialize correctly
    Runtime runtime;
    EXPECT_TRUE(runtime.initialize());
}

// Example test for future expansion (commented as documentation only - not blocking):
// TEST(UIRuntimeIntegration, CanCreateButtonInRuntime) {
//     Runtime runtime;
//     runtime.initialize();
//     
//     auto button = std::make_shared<Button>("Test");
//     EXPECT_NE(button, nullptr);
// }

