/**
 * GUI Tests: macOS Router Integration
 * 
 * These tests require a GUI environment (display server) to run.
 * They test the macOS router integration with NSViewController system.
 * 
 * To run these tests:
 *   bazel test //tests/gui:macos_router_test --config=macos --test_tag_filters=requires-gui
 * 
 * Note: These tests will be skipped in headless CI environments.
 * Run them manually on a machine with a display.
 */

#include <gtest/gtest.h>

#ifdef __APPLE__
#include "obsidian/router.h"
#include "obsidian/window.h"
#include "packages/apple/macos_router.h"
#include "packages/apple/macos_ffi.h"
#include <filesystem>

using namespace obsidian;

class MacOSRouterGUITest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize platform first
        ASSERT_TRUE(obsidian::ffi::macos::initializePlatform());
        
        // Find test app directory
        std::filesystem::path workspaceRoot;
        const char* testSrcDir = std::getenv("TEST_SRCDIR");
        const char* testWorkspace = std::getenv("TEST_WORKSPACE");
        
        if (testSrcDir && testWorkspace) {
            workspaceRoot = std::filesystem::path(testSrcDir) / testWorkspace;
            if (!std::filesystem::exists(workspaceRoot / "test_apps" / "route_scanner_test" / "app")) {
                workspaceRoot.clear();
            }
        }
        
        if (workspaceRoot.empty()) {
            std::filesystem::path searchPath = std::filesystem::current_path();
            for (int i = 0; i < 20 && !searchPath.empty() && searchPath.has_parent_path(); ++i) {
                std::filesystem::path testAppPath = searchPath / "test_apps" / "route_scanner_test" / "app";
                if (std::filesystem::exists(testAppPath) && std::filesystem::is_directory(testAppPath)) {
                    workspaceRoot = searchPath;
                    break;
                }
                if (std::filesystem::exists(searchPath / "MODULE.bazel") ||
                    (std::filesystem::exists(searchPath / "BUILD") && 
                     std::filesystem::exists(searchPath / "test_apps"))) {
                    workspaceRoot = searchPath;
                    break;
                }
                std::filesystem::path parent = searchPath.parent_path();
                if (parent == searchPath) break;
                searchPath = parent;
            }
        }
        
        if (!workspaceRoot.empty()) {
            testAppDirectory = (workspaceRoot / "test_apps" / "route_scanner_test" / "app").string();
        } else {
            testAppDirectory = (std::filesystem::current_path() / "test_apps" / "route_scanner_test" / "app").string();
        }
    }
    
    void TearDown() override {
        obsidian::ffi::macos::cleanupPlatform();
    }
    
    std::string testAppDirectory;
};

TEST_F(MacOSRouterGUITest, RouterCreatesMacOSIntegration) {
    // Create window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Router Test"));
    ASSERT_TRUE(window.isValid());
    
    // Create and initialize router
    Router router;
    ASSERT_TRUE(router.initialize(testAppDirectory));
    ASSERT_TRUE(router.isValid());
    
    // Set window and attach - this should create macOS router integration
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Verify router is still valid
    ASSERT_TRUE(router.isValid());
    
    // Cleanup
    window.close();
}

TEST_F(MacOSRouterGUITest, RouterNavigationUpdatesMacOSViewControllers) {
    // Create window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Router Test"));
    ASSERT_TRUE(window.isValid());
    
    // Create and initialize router
    Router router;
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Navigate to a route
    router.navigate("/about");
    
    // Verify navigation occurred
    ASSERT_EQ(router.getCurrentPath(), "/about");
    
    // Navigate to another route
    router.navigate("/products");
    ASSERT_EQ(router.getCurrentPath(), "/products");
    
    // Cleanup
    window.close();
}

TEST_F(MacOSRouterGUITest, RouterBackForwardNavigation) {
    // Create window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Router Test"));
    ASSERT_TRUE(window.isValid());
    
    // Create and initialize router
    Router router;
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Navigate to multiple routes
    router.navigate("/about");
    ASSERT_EQ(router.getCurrentPath(), "/about");
    
    router.navigate("/products");
    ASSERT_EQ(router.getCurrentPath(), "/products");
    
    // Go back
    ASSERT_TRUE(router.canGoBack());
    router.goBack();
    ASSERT_EQ(router.getCurrentPath(), "/about");
    
    // Go forward
    ASSERT_TRUE(router.canGoForward());
    router.goForward();
    ASSERT_EQ(router.getCurrentPath(), "/products");
    
    // Cleanup
    window.close();
}

TEST_F(MacOSRouterGUITest, RouterReplaceNavigation) {
    // Create window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Router Test"));
    ASSERT_TRUE(window.isValid());
    
    // Create and initialize router
    Router router;
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Navigate to a route
    router.navigate("/about");
    int historySizeBefore = router.getHistorySize();
    
    // Replace current route
    router.replace("/products");
    ASSERT_EQ(router.getCurrentPath(), "/products");
    ASSERT_EQ(router.getHistorySize(), historySizeBefore) << "History size should not change on replace";
    
    // Cleanup
    window.close();
}

TEST_F(MacOSRouterGUITest, RouterNavigationCallback) {
    // Create window
    Window window;
    ASSERT_TRUE(window.create(800, 600, "Router Test"));
    ASSERT_TRUE(window.isValid());
    
    // Create and initialize router
    Router router;
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Set up navigation callback
    bool callbackCalled = false;
    std::string navigatedPath;
    
    router.setOnNavigation([&](const std::string& path) {
        callbackCalled = true;
        navigatedPath = path;
    });
    
    // Navigate
    router.navigate("/about");
    
    // Verify callback was called
    ASSERT_TRUE(callbackCalled);
    ASSERT_EQ(navigatedPath, "/about");
    
    // Cleanup
    window.close();
}

#endif // __APPLE__
