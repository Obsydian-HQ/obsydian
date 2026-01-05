/**
 * Router Patterns Example
 * 
 * Demonstrates all navigation patterns:
 * - navigate() - standard navigation
 * - push() - push onto history stack
 * - replace() - replace current history entry
 * - goBack() - navigate back in history
 * - goForward() - navigate forward in history
 */

#include <iostream>
#include <obsidian/obsidian.h>

using namespace obsidian;

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Obsidian Router Patterns Example ===\n\n";
    
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
    
    if (!window.create(1000, 700, "Router Patterns Example")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    std::cout << "Window created successfully\n";
    
    // Initialize router with app directory
    Router router;
    if (!router.initialize("app")) {
        std::cerr << "Failed to initialize router\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Router initialized successfully\n";
    
    // Set window for router
    router.setWindow(window);
    
    // Set up navigation callback
    router.setOnNavigation([](const std::string& path) {
        std::cout << "Navigated to: " << path << std::endl;
    });
    
    // Navigate to initial route
    router.navigate("/");
    
    window.show();
    std::cout << "Window displayed\n";
    std::cout << "\nRunning application...\n";
    std::cout << "This example demonstrates all navigation patterns.\n";
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
