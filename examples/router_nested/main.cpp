/**
 * Router Nested Example
 * 
 * Demonstrates nested routes and layout nesting.
 * This example shows:
 * - Nested route structures
 * - Layout components that wrap child routes
 * - Multiple levels of nesting
 */

#include <iostream>
#include <obsidian/obsidian.h>
#include <filesystem>

using namespace obsidian;

// Helper to find app directory - works when run via bazel or directly
std::string findAppDirectory(const std::string& appDirName) {
    // First, try relative to current working directory (works if run from example dir)
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path testPath = currentDir / appDirName;
    if (std::filesystem::exists(testPath) && std::filesystem::is_directory(testPath)) {
        return testPath.string();
    }
    
    // Try from workspace root (where bazel runs from)
    std::filesystem::path searchPath = currentDir;
    for (int i = 0; i < 10 && !searchPath.empty() && searchPath.has_parent_path(); ++i) {
        // Check if this looks like workspace root
        if (std::filesystem::exists(searchPath / "MODULE.bazel") ||
            (std::filesystem::exists(searchPath / "BUILD") && 
             std::filesystem::exists(searchPath / "examples"))) {
            // Try examples/router_nested/app
            std::filesystem::path appPath = searchPath / "examples" / "router_nested" / appDirName;
            if (std::filesystem::exists(appPath) && std::filesystem::is_directory(appPath)) {
                return appPath.string();
            }
            break;
        }
        searchPath = searchPath.parent_path();
    }
    
    // Last resort: return original (will fail gracefully)
    return appDirName;
}

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Obsidian Router Nested Example ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Obsidian application\n";
        return 1;
    }
    std::cout << "Obsidian application initialized successfully\n";
    
    // Create a window
    std::cout << "Creating window...\n";
    Window window;
    
    if (!window.create(1000, 700, "Router Nested Example")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    std::cout << "Window created successfully\n";
    
    // Find app directory
    std::string appDir = findAppDirectory("app");
    std::cout << "Using app directory: " << appDir << std::endl;
    
    // Initialize router with app directory
    Router router;
    if (!router.initialize(appDir)) {
        std::cerr << "Failed to initialize router\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Router initialized successfully\n";
    
    // Set window and attach router to it
    router.setWindow(window);
    router.attachToWindow(window);
    
    // Set up navigation callback
    router.setOnNavigation([](const std::string& path) {
        std::cout << "Navigated to: " << path << std::endl;
    });
    
    // Navigate to initial route
    router.navigate("/");
    
    window.show();
    std::cout << "Window displayed\n";
    std::cout << "\nRunning application...\n";
    std::cout << "This example demonstrates nested routes and layouts.\n";
    std::cout << "Close the window to exit.\n\n";
    
    // Set up application callbacks
    AppCallbacks callbacks;
    callbacks.onInit = []() {
        std::cout << "Application initialized\n";
    };
    callbacks.onUpdate = []() {
        // Process events
    };
    callbacks.onShutdown = []() {
        std::cout << "Application shutting down\n";
    };
    
    // Run the application event loop
    app.run(callbacks);
    
    // Cleanup
    window.close();
    app.shutdown();
    
    std::cout << "\n=== Application exited ===\n";
    return 0;
}
