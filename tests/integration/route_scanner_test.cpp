/**
 * Integration tests for Route Scanner and Registry
 */

#include "core/routing/route_scanner.h"
#include "core/routing/route_registry.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

using namespace obsidian::routing;

class RouteScannerTest : public ::testing::Test {
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
        
        // Verify the directory exists
        if (!std::filesystem::exists(testAppDirectory) || !std::filesystem::is_directory(testAppDirectory)) {
            std::cerr << "ERROR: Test app directory not found: " << testAppDirectory << std::endl;
            std::cerr << "Current path: " << std::filesystem::current_path() << std::endl;
            std::cerr << "TEST_SRCDIR: " << (testSrcDir ? testSrcDir : "not set") << std::endl;
            std::cerr << "TEST_WORKSPACE: " << (testWorkspace ? testWorkspace : "not set") << std::endl;
        }
    }
    
    std::string testAppDirectory;
};

TEST_F(RouteScannerTest, ScanAppDirectory) {
    RouteScanner scanner;
    bool result = scanner.scan(testAppDirectory);
    
    ASSERT_TRUE(result) << "Route scanner should successfully scan the app directory";
    ASSERT_TRUE(scanner.isValid()) << "Route scanner should be valid after scanning";
    
    const auto& routeFiles = scanner.getRouteFiles();
    ASSERT_GT(routeFiles.size(), 0) << "Should discover at least one route file";
}

TEST_F(RouteScannerTest, DiscoverLayoutFiles) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundRootLayout = false;
    bool foundProductsLayout = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::Layout) {
            if (routeFile.fileName == "_layout.cpp" && routeFile.relativePath.empty()) {
                foundRootLayout = true;
            }
            if (routeFile.fileName == "_layout.cpp" && 
                routeFile.relativePath.string().find("products") != std::string::npos) {
                foundProductsLayout = true;
            }
        }
    }
    
    ASSERT_TRUE(foundRootLayout) << "Should discover root _layout.cpp";
    ASSERT_TRUE(foundProductsLayout) << "Should discover products/_layout.cpp";
}

TEST_F(RouteScannerTest, DiscoverIndexFiles) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundRootIndex = false;
    bool foundProductsIndex = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::Index) {
            if (routeFile.relativePath.empty()) {
                foundRootIndex = true;
            }
            if (routeFile.relativePath.string().find("products") != std::string::npos) {
                foundProductsIndex = true;
            }
        }
    }
    
    ASSERT_TRUE(foundRootIndex) << "Should discover root index.cpp";
    ASSERT_TRUE(foundProductsIndex) << "Should discover products/index.cpp";
}

TEST_F(RouteScannerTest, DiscoverStaticRoutes) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundAbout = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::Static && routeFile.fileName == "about.cpp") {
            foundAbout = true;
            ASSERT_EQ(routeFile.routePath, "/about") << "about.cpp should map to /about";
        }
    }
    
    ASSERT_TRUE(foundAbout) << "Should discover about.cpp static route";
}

TEST_F(RouteScannerTest, DiscoverDynamicRoutes) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundProductId = false;
    bool foundUserId = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::Dynamic) {
            if (routeFile.dynamicParamName == "id" && 
                routeFile.relativePath.string().find("products") != std::string::npos) {
                foundProductId = true;
                ASSERT_EQ(routeFile.routePath, "/products/:id") << "products/[id].cpp should map to /products/:id";
            }
            if (routeFile.dynamicParamName == "userId") {
                foundUserId = true;
                ASSERT_EQ(routeFile.routePath, "/users/:userId") << "users/[userId].cpp should map to /users/:userId";
            }
        }
    }
    
    ASSERT_TRUE(foundProductId) << "Should discover products/[id].cpp dynamic route";
    ASSERT_TRUE(foundUserId) << "Should discover users/[userId].cpp dynamic route";
}

TEST_F(RouteScannerTest, DiscoverCatchAllRoutes) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundCatchAll = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.type == RouteFileType::CatchAll) {
            foundCatchAll = true;
            ASSERT_TRUE(routeFile.isCatchAll) << "Catch-all route should be marked as such";
            ASSERT_EQ(routeFile.routePath, "/users/*") << "users/[...catchall].cpp should map to /users/*";
        }
    }
    
    ASSERT_TRUE(foundCatchAll) << "Should discover catch-all route";
}

TEST_F(RouteScannerTest, DiscoverNestedRoutes) {
    RouteScanner scanner;
    scanner.scan(testAppDirectory);
    
    const auto& routeFiles = scanner.getRouteFiles();
    
    bool foundNested = false;
    
    for (const auto& routeFile : routeFiles) {
        if (routeFile.fileName == "reviews.cpp" && 
            routeFile.relativePath.string().find("products") != std::string::npos &&
            routeFile.relativePath.string().find("[id]") != std::string::npos) {
            foundNested = true;
            ASSERT_EQ(routeFile.routePath, "/products/:id/reviews") 
                << "products/[id]/reviews.cpp should map to /products/:id/reviews";
        }
    }
    
    ASSERT_TRUE(foundNested) << "Should discover nested route products/[id]/reviews.cpp";
}

class RouteRegistryTest : public ::testing::Test {
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
        
        scanner.scan(testAppDirectory);
        registry.buildFromScanner(scanner);
    }
    
    std::string testAppDirectory;
    RouteScanner scanner;
    RouteRegistry registry;
};

TEST_F(RouteRegistryTest, BuildRegistryFromScanner) {
    ASSERT_TRUE(registry.isValid()) << "Registry should be valid after building from scanner";
    
    const auto* root = registry.getRoot();
    ASSERT_NE(root, nullptr) << "Registry should have a root node";
}

TEST_F(RouteRegistryTest, FindStaticRoute) {
    RouteNode* node = registry.findRoute("/about");
    ASSERT_NE(node, nullptr) << "Should find /about route";
    ASSERT_EQ(node->routeFile.fileName, "about.cpp") << "Found route should be about.cpp";
}

TEST_F(RouteRegistryTest, FindIndexRoute) {
    RouteNode* node = registry.findRoute("/");
    ASSERT_NE(node, nullptr) << "Should find root index route";
    
    node = registry.findRoute("/products");
    ASSERT_NE(node, nullptr) << "Should find /products index route";
}

TEST_F(RouteRegistryTest, MatchDynamicRoute) {
    std::map<std::string, std::string> params;
    RouteNode* node = registry.matchRoute("/products/123", params);
    
    ASSERT_NE(node, nullptr) << "Should match /products/123 to /products/:id";
    ASSERT_EQ(params["id"], "123") << "Should extract id parameter";
}

TEST_F(RouteRegistryTest, MatchNestedDynamicRoute) {
    std::map<std::string, std::string> params;
    RouteNode* node = registry.matchRoute("/products/456/reviews", params);
    
    ASSERT_NE(node, nullptr) << "Should match /products/456/reviews";
    ASSERT_EQ(params["id"], "456") << "Should extract id parameter from nested route";
}

TEST_F(RouteRegistryTest, MatchCatchAllRoute) {
    std::map<std::string, std::string> params;
    RouteNode* node = registry.matchRoute("/users/anything/here", params);
    
    ASSERT_NE(node, nullptr) << "Should match catch-all route";
}

TEST_F(RouteRegistryTest, GetAllRoutes) {
    std::vector<std::string> routes = registry.getAllRoutes();
    
    ASSERT_GT(routes.size(), 0) << "Should return at least one route";
    
    // Check for some expected routes
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

TEST_F(RouteRegistryTest, RouteExists) {
    ASSERT_TRUE(registry.routeExists("/about")) << "/about should exist";
    ASSERT_TRUE(registry.routeExists("/products")) << "/products should exist";
    ASSERT_FALSE(registry.routeExists("/nonexistent")) << "/nonexistent should not exist";
}

TEST_F(RouteRegistryTest, LayoutFiles) {
    const auto* root = registry.getRoot();
    ASSERT_NE(root, nullptr);
    
    // Check if root has layout
    // Note: This depends on the test app structure
    // The root layout should be associated with the root node
}
