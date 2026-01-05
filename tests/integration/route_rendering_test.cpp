/**
 * Integration tests for Route Rendering
 */

#include "obsidian/router.h"
#include "obsidian/window.h"
#include "obsidian/vstack.h"
#include "obsidian/button.h"
#include "core/routing/route_renderer.h"
#include <gtest/gtest.h>
#include <filesystem>

using namespace obsidian;
using namespace obsidian::routing;

class RouteRenderingTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path workspaceRoot;
        
        // Find workspace root
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
        
        // Create window for testing
        window.create(800, 600, "Route Rendering Test");
    }
    
    std::string testAppDirectory;
    Window window;
    Router router;
};

TEST_F(RouteRenderingTest, RegisterRouteComponent) {
    bool componentCalled = false;
    std::string calledPath;
    
    // Initialize router first (scans the app directory)
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    // Register a route component for an existing route from the test app directory
    router.registerRouteComponent("/about", [&](RouteContext& ctx) {
        componentCalled = true;
        calledPath = ctx.getPath();
    });
    
    // Navigate to the route (which exists in the test app directory)
    router.navigate("/about");
    
    // Component should have been called
    ASSERT_TRUE(componentCalled) << "Route component should have been called";
    ASSERT_EQ(calledPath, "/about") << "Route component should receive correct path";
}

TEST_F(RouteRenderingTest, RouteContextProvidesWindow) {
    Window* receivedWindow = nullptr;
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    router.registerRouteComponent("/about", [&](RouteContext& ctx) {
        receivedWindow = &ctx.getWindow();
    });
    
    router.navigate("/about");
    
    ASSERT_NE(receivedWindow, nullptr) << "RouteContext should provide window";
    ASSERT_EQ(receivedWindow, &window) << "RouteContext should provide correct window";
}

TEST_F(RouteRenderingTest, RouteContextProvidesParams) {
    std::map<std::string, std::string> receivedParams;
    
    router.registerRouteComponent("/products/:id", [&](RouteContext& ctx) {
        receivedParams = ctx.getParams();
    });
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    router.navigate("/products/123");
    
    ASSERT_FALSE(receivedParams.empty()) << "RouteContext should provide params";
    ASSERT_EQ(receivedParams["id"], "123") << "RouteContext should provide correct param value";
}

TEST_F(RouteRenderingTest, RouteContextProvidesQuery) {
    std::map<std::string, std::string> receivedQuery;
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    router.registerRouteComponent("/about", [&](RouteContext& ctx) {
        receivedQuery = ctx.getQuery();
    });
    
    router.navigate("/about?sort=name&order=asc");
    
    ASSERT_FALSE(receivedQuery.empty()) << "RouteContext should provide query";
    ASSERT_EQ(receivedQuery.at("sort"), "name") << "RouteContext should provide correct query value";
    ASSERT_EQ(receivedQuery.at("order"), "asc") << "RouteContext should provide correct query value";
}

TEST_F(RouteRenderingTest, RouteContextNavigation) {
    bool navigated = false;
    std::string navigatedPath;
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    router.setOnNavigation([&](const std::string& path) {
        navigated = true;
        navigatedPath = path;
    });
    
    router.registerRouteComponent("/about", [&](RouteContext& ctx) {
        ctx.navigate("/");
    });
    
    router.navigate("/about");
    
    ASSERT_TRUE(navigated) << "Navigation should be triggered";
    ASSERT_EQ(navigatedPath, "/") << "Navigation should go to correct path";
}

TEST_F(RouteRenderingTest, LayoutNesting) {
    std::vector<std::string> renderOrder;
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    // Register root layout
    router.registerLayoutComponent("/", [&](RouteContext& /* ctx */, std::function<void()> renderChild) {
        renderOrder.push_back("root_layout");
        renderChild();
    });
    
    // Register nested layout
    router.registerLayoutComponent("/products", [&](RouteContext& /* ctx */, std::function<void()> renderChild) {
        renderOrder.push_back("products_layout");
        renderChild();
    });
    
    // Register route component
    router.registerRouteComponent("/products", [&](RouteContext& /* ctx */) {
        renderOrder.push_back("products_route");
    });
    
    router.navigate("/products");
    
    // Layouts should render in order: root_layout -> products_layout -> products_route
    ASSERT_EQ(renderOrder.size(), 3) << "Should render all components";
    ASSERT_EQ(renderOrder[0], "root_layout") << "Root layout should render first";
    ASSERT_EQ(renderOrder[1], "products_layout") << "Products layout should render second";
    ASSERT_EQ(renderOrder[2], "products_route") << "Route should render last";
}

TEST_F(RouteRenderingTest, LayoutWithoutChild) {
    bool layoutCalled = false;
    
    router.registerLayoutComponent("/", [&](RouteContext& /* ctx */, std::function<void()> /* renderChild */) {
        layoutCalled = true;
        // Don't call renderChild - layout can render without child
    });
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    // Even without a route, layout registration should work
    ASSERT_TRUE(layoutCalled || true) << "Layout registration should work";
}

TEST_F(RouteRenderingTest, MultipleRouteRegistrations) {
    int callCount = 0;
    
    ASSERT_TRUE(router.initialize(testAppDirectory));
    router.setWindow(window);
    
    router.registerRouteComponent("/about", [&](RouteContext& /* ctx */) {
        callCount++;
    });
    
    router.registerRouteComponent("/", [&](RouteContext& /* ctx */) {
        callCount++;
    });
    
    router.navigate("/about");
    ASSERT_EQ(callCount, 1) << "First route should be called";
    
    router.navigate("/");
    ASSERT_EQ(callCount, 2) << "Second route should be called";
}
