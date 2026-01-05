/**
 * Native Sidebar Showcase Example Application
 * 
 * This example demonstrates the native macOS Sidebar using NSSplitViewController
 * and NSSplitViewItem.sidebar() with:
 * - Native material background (vibrancy effect)
 * - Native collapse/expand button
 * - Full-height sidebar integration
 * - Automatic window control integration
 * - List component in sidebar
 * - Main content area
 * 
 * This example uses the public Obsidian API - exactly how real users
 * (like Glass) will use Obsidian.
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <obsidian/obsidian.h>

using namespace obsidian;

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Obsidian Native Sidebar Showcase ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Obsidian application\n";
        return 1;
    }
    std::cout << "Obsidian application initialized successfully\n";
    
    // Create a window using the public API
    Window window;
    if (!window.create(1200, 800, "Native Sidebar Showcase")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Window created\n";
    
    // Create native Sidebar
    Sidebar sidebar;
    if (!sidebar.create()) {
        std::cerr << "Failed to create Sidebar\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Native Sidebar created\n";
    
    // Configure sidebar width constraints
    sidebar.setMinimumSidebarWidth(200.0);  // Minimum width: 200pt
    sidebar.setMaximumSidebarWidth(400.0);  // Maximum width: 400pt
    std::cout << "Sidebar configured: min=200pt, max=400pt\n";
    
    // Create sidebar content using List
    List sidebarList;
    sidebarList.create(0, 0, 250, 600);
    
    // Add items to sidebar list
    sidebarList.addItem("Home");
    sidebarList.addItem("Projects");
    sidebarList.addItem("Documents");
    sidebarList.addItem("Settings");
    sidebarList.addItem("Help");
    std::cout << "Sidebar list created with 5 items\n";
    
    // Set sidebar content
    sidebar.setSidebarContent(sidebarList);
    std::cout << "Sidebar content set (List with 5 items)\n";
    
    // Create main content area using VStack
    VStack mainContentVStack;
    mainContentVStack.setSpacing(20.0);
    mainContentVStack.setPadding(Padding::all(30.0));
    mainContentVStack.setAlignment(layout::Alignment::TopLeading);
    
    // Create buttons for main content area
    Button mainButton1;
    mainButton1.create("Primary Action", 0, 0, 300, 40);
    mainButton1.setOnClick([]() {
        std::cout << "Primary Action clicked!\n";
    });
    
    Button mainButton2;
    mainButton2.create("Secondary Action", 0, 0, 300, 40);
    mainButton2.setOnClick([]() {
        std::cout << "Secondary Action clicked!\n";
    });
    
    // Add buttons to main content VStack
    mainContentVStack.addChild(mainButton1);
    mainContentVStack.addChild(mainButton2);
    
    // Update main content VStack layout
    mainContentVStack.updateLayout();
    
    // Set main content
    sidebar.setMainContent(mainContentVStack);
    std::cout << "Main content set (VStack with 2 buttons)\n";
    
    // Add Sidebar to window
    sidebar.addToWindow(window);
    std::cout << "Sidebar added to window\n";
    
    // Set up sidebar toggle callback
    sidebar.setOnSidebarToggle([](bool collapsed) {
        if (collapsed) {
            std::cout << "Sidebar collapsed (native button)\n";
        } else {
            std::cout << "Sidebar expanded (native button)\n";
        }
    });
    
    // Show the window
    window.show();
    std::cout << "\nWindow displayed with Native Sidebar\n";
    std::cout << "Native Sidebar features:\n";
    std::cout << "  - Material background (native macOS vibrancy)\n";
    std::cout << "  - Native collapse/expand button\n";
    std::cout << "  - Full-height sidebar integration\n";
    std::cout << "  - Automatic window control integration\n";
    std::cout << "  - List component with 5 items\n";
    std::cout << "\nMain content features:\n";
    std::cout << "  - VStack with 2 action buttons\n";
    std::cout << "  - Resizes automatically when sidebar changes\n";
    std::cout << "\nInteract with the sidebar list, use the native collapse button, and resize the sidebar.\n";
    std::cout << "Close the window to exit.\n\n";
    
    // Set up application callbacks
    AppCallbacks callbacks;
    callbacks.onInit = []() {
        std::cout << "Application initialized\n";
    };
    callbacks.onUpdate = []() {
        // In a real app, this would process events
    };
    callbacks.onShutdown = []() {
        std::cout << "Application shutting down\n";
    };
    
    // Run the application event loop (blocks until app quits)
    app.run(callbacks);
    
    // Cleanup is automatic (RAII), but we can be explicit
    window.close();
    app.shutdown();
    
    std::cout << "\n=== Native Sidebar Showcase exited ===\n";
    return 0;
}
