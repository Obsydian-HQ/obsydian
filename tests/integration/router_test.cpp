/**
 * Integration tests for Router
 */

#include "obsidian/router.h"
#include "core/routing/route_scanner.h"
#include "core/routing/route_registry.h"
#include "core/routing/navigation_history.h"
#include <gtest/gtest.h>
#include <filesystem>

using namespace obsidian;
using namespace obsidian::routing;

class RouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path workspaceRoot;
        
        // First, try TEST_SRCDIR which points to the source directory in Bazel
        const char* testSrcDir = std::getenv("TEST_SRCDIR");
        const char* testWorkspace = std::getenv("TEST_WORKSPACE");
        
        if (testSrcDir && testWorkspace) {
            // In Bazel, TEST_SRCDIR points to the source tree
            // Format: TEST_SRCDIR/TEST_WORKSPACE/...
            workspaceRoot = std::filesystem::path(testSrcDir) / testWorkspace;
            
            // Verify it exists
            if (!std::filesystem::exists(workspaceRoot / "test_apps" / "route_scanner_test" / "app")) {
                workspaceRoot.clear();
            }
        }
        
        // Fallback: search from current directory up to find workspace root
        if (workspaceRoot.empty()) {
            std::filesystem::path searchPath = std::filesystem::current_path();
            
            for (int i = 0; i < 20 && !searchPath.empty() && searchPath.has_parent_path(); ++i) {
                std::filesystem::path testAppPath = searchPath / "test_apps" / "route_scanner_test" / "app";
                if (std::filesystem::exists(testAppPath) && std::filesystem::is_directory(testAppPath)) {
                    workspaceRoot = searchPath;
                    break;
                }
                
                // Also check for workspace markers
                if (std::filesystem::exists(searchPath / "MODULE.bazel") ||
                    (std::filesystem::exists(searchPath / "BUILD") && 
                     std::filesystem::exists(searchPath / "test_apps"))) {
                    workspaceRoot = searchPath;
                    break;
                }
                
                std::filesystem::path parent = searchPath.parent_path();
                if (parent == searchPath) break; // Reached filesystem root
                searchPath = parent;
            }
        }
        
        if (!workspaceRoot.empty()) {
            testAppDirectory = (workspaceRoot / "test_apps" / "route_scanner_test" / "app").string();
        } else {
            // Last resort: try relative to current directory
            testAppDirectory = (std::filesystem::current_path() / "test_apps" / "route_scanner_test" / "app").string();
        }
    }
    
    std::string testAppDirectory;
};

TEST_F(RouterTest, InitializeRouter) {
    Router router;
    bool result = router.initialize(testAppDirectory);
    
    ASSERT_TRUE(result) << "Router should initialize successfully";
    ASSERT_TRUE(router.isValid()) << "Router should be valid after initialization";
}

TEST_F(RouterTest, RouteExists) {
    Router router;
    router.initialize(testAppDirectory);
    
    ASSERT_TRUE(router.routeExists("/about")) << "/about should exist";
    ASSERT_TRUE(router.routeExists("/products")) << "/products should exist";
    ASSERT_FALSE(router.routeExists("/nonexistent")) << "/nonexistent should not exist";
}

TEST_F(RouterTest, GetAllRoutes) {
    Router router;
    router.initialize(testAppDirectory);
    
    auto routes = router.getAllRoutes();
    ASSERT_GT(routes.size(), 0) << "Should return at least one route";
    
    bool foundAbout = false;
    bool foundProducts = false;
    
    for (const auto& route : routes) {
        if (route == "/about") {
            foundAbout = true;
        }
        if (route == "/products") {
            foundProducts = true;
        }
    }
    
    ASSERT_TRUE(foundAbout) << "Should include /about route";
    ASSERT_TRUE(foundProducts) << "Should include /products route";
}

TEST_F(RouterTest, NavigateToRoute) {
    Router router;
    router.initialize(testAppDirectory);
    
    bool navigationCalled = false;
    std::string navigatedPath;
    
    router.setOnNavigation([&](const std::string& path) {
        navigationCalled = true;
        navigatedPath = path;
    });
    
    router.navigate("/about");
    
    ASSERT_TRUE(navigationCalled) << "Navigation callback should be called";
    ASSERT_EQ(navigatedPath, "/about") << "Should navigate to /about";
    ASSERT_EQ(router.getCurrentPath(), "/about") << "Current path should be /about";
}

TEST_F(RouterTest, NavigateToDynamicRoute) {
    Router router;
    router.initialize(testAppDirectory);
    
    router.navigate("/products/123");
    
    // getCurrentPath() returns the actual path navigated to, not the route pattern
    ASSERT_EQ(router.getCurrentPath(), "/products/123") << "Should return the actual path navigated to";
    ASSERT_TRUE(router.routeExists("/products/123")) << "Dynamic route should exist";
}

TEST_F(RouterTest, NavigationHistory) {
    Router router;
    router.initialize(testAppDirectory);
    
    // Navigate to multiple routes
    router.navigate("/about");
    ASSERT_EQ(router.getCurrentPath(), "/about");
    
    router.navigate("/products");
    ASSERT_EQ(router.getCurrentPath(), "/products");
    
    // Check history size
    ASSERT_GE(router.getHistorySize(), 2) << "History should have at least 2 entries";
    
    // Navigate back
    ASSERT_TRUE(router.canGoBack()) << "Should be able to go back";
    router.goBack();
    ASSERT_EQ(router.getCurrentPath(), "/about") << "Should navigate back to /about";
    
    // Navigate forward
    ASSERT_TRUE(router.canGoForward()) << "Should be able to go forward";
    router.goForward();
    ASSERT_EQ(router.getCurrentPath(), "/products") << "Should navigate forward to /products";
}

TEST_F(RouterTest, ReplaceRoute) {
    Router router;
    router.initialize(testAppDirectory);
    
    router.navigate("/about");
    int historySizeBefore = router.getHistorySize();
    
    router.replace("/products");
    
    ASSERT_EQ(router.getCurrentPath(), "/products") << "Should replace with /products";
    ASSERT_EQ(router.getHistorySize(), historySizeBefore) << "History size should not change on replace";
}

TEST_F(RouterTest, ClearHistory) {
    Router router;
    router.initialize(testAppDirectory);
    
    router.navigate("/about");
    router.navigate("/products");
    
    ASSERT_GT(router.getHistorySize(), 0) << "History should have entries";
    
    router.clearHistory();
    
    ASSERT_EQ(router.getHistorySize(), 0) << "History should be cleared";
}

TEST_F(RouterTest, NavigateWithQueryParameters) {
    Router router;
    router.initialize(testAppDirectory);
    
    router.navigate("/about?sort=name&order=asc");
    
    ASSERT_EQ(router.getCurrentPath(), "/about") << "Path should not include query parameters";
}

TEST_F(RouterTest, CanGoBackAndForward) {
    Router router;
    router.initialize(testAppDirectory);
    
    ASSERT_FALSE(router.canGoBack()) << "Should not be able to go back initially";
    ASSERT_FALSE(router.canGoForward()) << "Should not be able to go forward initially";
    
    router.navigate("/about");
    ASSERT_FALSE(router.canGoBack()) << "Should not be able to go back after first navigation";
    
    router.navigate("/products");
    ASSERT_TRUE(router.canGoBack()) << "Should be able to go back after second navigation";
    ASSERT_FALSE(router.canGoForward()) << "Should not be able to go forward";
    
    router.goBack();
    ASSERT_TRUE(router.canGoForward()) << "Should be able to go forward after going back";
}
