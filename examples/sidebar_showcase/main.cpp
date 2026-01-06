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
    
    // Create main content area with proper layout
    // This demonstrates sidebar integration with the layout engine
    VStack mainContent;
    mainContent.setSpacing(20.0);
    mainContent.setAlignment(layout::Alignment::TopLeading);
    mainContent.setPadding(Padding::all(30.0));
    
    // Add some content to verify layout works
    TextView titleText;
    titleText.create("Main Content Area", 0, 0, 400, 40);
    titleText.setFontSize(24.0);
    titleText.setFontWeight(FontWeight::Bold);
    
    TextView descriptionText;
    descriptionText.create("This is the main content area. The sidebar is on the left.", 0, 0, 500, 30);
    descriptionText.setFontSize(14.0);
    
    Button actionButton;
    actionButton.create("Click Me", 0, 0, 200, 40);
    actionButton.setOnClick([]() {
        std::cout << "Main content button clicked!\n";
    });
    
    mainContent.addChild(titleText);
    mainContent.addChild(descriptionText);
    mainContent.addChild(actionButton);
    // Note: We don't call updateLayout() here because the VStack doesn't have
    // a parent view yet, so it has no bounds. Layout will be calculated when
    // addToParentView is called in sidebar.setMainContent()
    
    sidebar.setMainContent(mainContent);
    
    // After sidebar is added to window, the view controller's view will be sized
    // and we can trigger a final layout update. This happens in addToWindow().
    
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
