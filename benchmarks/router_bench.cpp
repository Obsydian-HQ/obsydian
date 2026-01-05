/**
 * Router Performance Benchmarks
 * 
 * Benchmarks for router performance:
 * - Route matching performance
 * - Navigation performance
 * - Route scanning performance
 * - History management performance
 */

#include <benchmark/benchmark.h>
#include <obsidian/router.h>
#include <obsidian/window.h>
#include <filesystem>
#include <string>

using namespace obsidian;

// Helper to find test app directory
std::string findTestAppDirectory() {
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
        return (workspaceRoot / "test_apps" / "route_scanner_test" / "app").string();
    }
    return (std::filesystem::current_path() / "test_apps" / "route_scanner_test" / "app").string();
}

// Benchmark: Router initialization (includes route scanning)
static void BM_RouterInitialization(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    
    for (auto _ : state) {
        Router router;
        bool result = router.initialize(appDir);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_RouterInitialization);

// Benchmark: Route matching (static routes)
static void BM_RouteMatchingStatic(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    std::vector<std::string> testRoutes = {
        "/about",
        "/products",
        "/users"
    };
    
    for (auto _ : state) {
        for (const auto& route : testRoutes) {
            bool exists = router.routeExists(route);
            benchmark::DoNotOptimize(exists);
        }
    }
}
BENCHMARK(BM_RouteMatchingStatic);

// Benchmark: Route matching (dynamic routes)
static void BM_RouteMatchingDynamic(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    std::vector<std::string> testRoutes = {
        "/products/123",
        "/products/456",
        "/users/alice",
        "/users/bob"
    };
    
    for (auto _ : state) {
        for (const auto& route : testRoutes) {
            bool exists = router.routeExists(route);
            benchmark::DoNotOptimize(exists);
        }
    }
}
BENCHMARK(BM_RouteMatchingDynamic);

// Benchmark: Navigation (without rendering)
static void BM_Navigation(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    std::vector<std::string> routes = {
        "/about",
        "/products",
        "/users",
        "/about"
    };
    
    for (auto _ : state) {
        for (const auto& route : routes) {
            router.navigate(route);
            benchmark::DoNotOptimize(router.getCurrentPath());
        }
    }
}
BENCHMARK(BM_Navigation);

// Benchmark: History operations (back/forward)
static void BM_HistoryOperations(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    // Build up history
    router.navigate("/about");
    router.navigate("/products");
    router.navigate("/users");
    
    for (auto _ : state) {
        router.goBack();
        router.goForward();
        benchmark::DoNotOptimize(router.getCurrentPath());
    }
}
BENCHMARK(BM_HistoryOperations);

// Benchmark: History size operations
static void BM_HistorySize(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    // Build up history
    for (int i = 0; i < 10; ++i) {
        router.navigate("/about");
        router.navigate("/products");
    }
    
    for (auto _ : state) {
        int size = router.getHistorySize();
        benchmark::DoNotOptimize(size);
    }
}
BENCHMARK(BM_HistorySize);

// Benchmark: GetAllRoutes
static void BM_GetAllRoutes(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    for (auto _ : state) {
        auto routes = router.getAllRoutes();
        benchmark::DoNotOptimize(routes);
    }
}
BENCHMARK(BM_GetAllRoutes);

// Benchmark: Multiple router instances
static void BM_MultipleRouterInstances(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    
    for (auto _ : state) {
        std::vector<Router> routers;
        routers.reserve(state.range(0));
        
        for (int i = 0; i < state.range(0); ++i) {
            Router router;
            router.initialize(appDir);
            routers.push_back(std::move(router));
        }
        
        benchmark::DoNotOptimize(routers);
    }
}
BENCHMARK(BM_MultipleRouterInstances)
    ->Arg(1)
    ->Arg(5)
    ->Arg(10)
    ->Arg(50);

// Benchmark: Route replacement
static void BM_RouteReplacement(benchmark::State& state) {
    std::string appDir = findTestAppDirectory();
    Router router;
    router.initialize(appDir);
    
    router.navigate("/about");
    
    for (auto _ : state) {
        router.replace("/products");
        router.replace("/users");
        router.replace("/about");
        benchmark::DoNotOptimize(router.getCurrentPath());
    }
}
BENCHMARK(BM_RouteReplacement);

BENCHMARK_MAIN();
