/**
 * Native Sidebar Showcase Example Application
 * 
 * This example demonstrates the native macOS Sidebar using NSSplitViewController
 * and NSSplitViewItem.sidebar() with:
 * - Native material background (vibrancy effect)
 * - Native collapse/expand button in toolbar (positioned correctly)
 * - Full-height sidebar integration
 * - Automatic window control integration
 * - Native source list style for sidebar items
 * 
 * This example uses ONLY the sidebar - no extra content to verify
 * the native implementation is correct.
 */

#include <iostream>
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
    
    // Create a window using the public API
    Window window;
    if (!window.create(900, 600, "Native Sidebar Showcase")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    // IMPORTANT: Set up toolbar BEFORE creating sidebar
    // The toolbar provides the native collapse button for the sidebar
    // The toggle button will appear in the sidebar area (after traffic lights)
    if (!window.setupToolbarForSidebar()) {
        std::cerr << "Failed to setup toolbar for sidebar\n";
    }
    
    // Create native Sidebar
    Sidebar sidebar;
    if (!sidebar.create()) {
        std::cerr << "Failed to create Sidebar\n";
        app.shutdown();
        return 1;
    }
    
    // Configure sidebar width constraints
    sidebar.setMinimumSidebarWidth(200.0);
    sidebar.setMaximumSidebarWidth(350.0);
    
    // Create sidebar content using List
    // Using native source list style - text will be left-aligned, properly sized
    List sidebarList;
    sidebarList.create(0, 0, 250, 600);
    
    // Add items to sidebar list
    sidebarList.addItem("Home");
    sidebarList.addItem("Projects");
    sidebarList.addItem("Documents");
    sidebarList.addItem("Settings");
    sidebarList.addItem("Help");
    
    // Set sidebar content
    sidebar.setSidebarContent(sidebarList);
    
    // Create a simple main content area (required by NSSplitViewController)
    // Just an empty VStack - the focus is on the sidebar
    VStack mainContent;
    sidebar.setMainContent(mainContent);
    
    // Add Sidebar to window
    sidebar.addToWindow(window);
    
    // Set up sidebar toggle callback
    sidebar.setOnSidebarToggle([](bool collapsed) {
        std::cout << "Sidebar " << (collapsed ? "collapsed" : "expanded") << "\n";
    });
    
    // Show the window
    window.show();
    
    std::cout << "Native Sidebar Features:\n";
    std::cout << "  - Toggle button in toolbar (sidebar area, after traffic lights)\n";
    std::cout << "  - Source list style items (left-aligned text, proper sizing)\n";
    std::cout << "  - No horizontal scrolling\n";
    std::cout << "  - Vibrancy material background\n";
    std::cout << "\nClose the window to exit.\n\n";
    
    // Set up application callbacks
    AppCallbacks callbacks;
    callbacks.onInit = []() {};
    callbacks.onUpdate = []() {};
    callbacks.onShutdown = []() {
        std::cout << "Application shutting down\n";
    };
    
    // Run the application event loop
    app.run(callbacks);
    
    window.close();
    app.shutdown();
    
    return 0;
}
