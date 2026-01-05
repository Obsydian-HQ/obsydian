/**
 * SplitView Showcase Example Application
 * 
 * This example demonstrates the SplitView layout with:
 * - Primary pane with VStack containing buttons
 * - List component in primary pane
 * - Collapse/expand functionality
 * - Secondary pane with multiple components
 * - Resizing behavior (drag divider to resize)
 * - Minimum/maximum primary pane width constraints
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
    std::cout << "=== Obsidian SplitView Showcase ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Obsidian application\n";
        return 1;
    }
    std::cout << "Obsidian application initialized successfully\n";
    
    // Create a window using the public API
    Window window;
    if (!window.create(1000, 700, "SplitView Showcase")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Window created\n";
    
    // Create SplitView with primary pane on the leading (left) side
    SplitView splitView;
    if (!splitView.create(SplitPosition::Leading)) {
        std::cerr << "Failed to create SplitView\n";
        app.shutdown();
        return 1;
    }
    std::cout << "SplitView created with leading primary pane\n";
    
    // Configure primary pane width constraints
    splitView.setMinimumPrimaryPaneWidth(150.0);  // Minimum width: 150pt
    splitView.setMaximumPrimaryPaneWidth(400.0);  // Maximum width: 400pt
    splitView.setPrimaryPaneWidth(250.0);         // Initial width: 250pt
    std::cout << "Primary pane configured: min=150pt, max=400pt, initial=250pt\n";
    
    // Create primary pane content using VStack with buttons
    VStack primaryPaneVStack;
    primaryPaneVStack.setSpacing(10.0);
    primaryPaneVStack.setPadding(Padding::all(15.0));
    primaryPaneVStack.setAlignment(layout::Alignment::TopLeading);
    
    // Create buttons for primary pane navigation
    Button navButton1;
    navButton1.create("Home", 0, 0, 200, 30);
    navButton1.setOnClick([]() {
        std::cout << "Home button clicked!\n";
    });
    
    Button navButton2;
    navButton2.create("Projects", 0, 0, 200, 30);
    navButton2.setOnClick([]() {
        std::cout << "Projects button clicked!\n";
    });
    
    Button navButton3;
    navButton3.create("Settings", 0, 0, 200, 30);
    navButton3.setOnClick([]() {
        std::cout << "Settings button clicked!\n";
    });
    
    // Add buttons to primary pane VStack
    primaryPaneVStack.addChild(navButton1);
    primaryPaneVStack.addChild(navButton2);
    primaryPaneVStack.addChild(navButton3);
    
    // Update VStack layout (required after adding children)
    primaryPaneVStack.updateLayout();
    
    // Set primary pane content to VStack
    splitView.setPrimaryPaneContent(primaryPaneVStack);
    std::cout << "Primary pane content set (VStack with 3 buttons)\n";
    
    // Create secondary pane area using VStack
    VStack secondaryPaneVStack;
    secondaryPaneVStack.setSpacing(15.0);
    secondaryPaneVStack.setPadding(Padding::all(20.0));
    secondaryPaneVStack.setAlignment(layout::Alignment::TopLeading);
    
    // Create buttons for secondary pane area
    Button secondaryButton1;
    secondaryButton1.create("Action 1", 0, 0, 200, 35);
    secondaryButton1.setOnClick([]() {
        std::cout << "Action 1 clicked!\n";
    });
    
    Button secondaryButton2;
    secondaryButton2.create("Action 2", 0, 0, 200, 35);
    secondaryButton2.setOnClick([]() {
        std::cout << "Action 2 clicked!\n";
    });
    
    // Add buttons to secondary pane VStack
    secondaryPaneVStack.addChild(secondaryButton1);
    secondaryPaneVStack.addChild(secondaryButton2);
    
    // Update secondary pane VStack layout
    secondaryPaneVStack.updateLayout();
    
    // Set secondary pane content to VStack
    splitView.setSecondaryPaneContent(secondaryPaneVStack);
    std::cout << "Secondary pane content set (VStack with 2 buttons)\n";
    
    // Add SplitView to window
    splitView.addToWindow(window);
    std::cout << "SplitView added to window\n";
    
    // Set up primary pane toggle callback
    splitView.setOnPrimaryPaneToggle([](bool collapsed) {
        if (collapsed) {
            std::cout << "Primary pane collapsed\n";
        } else {
            std::cout << "Primary pane expanded\n";
        }
    });
    
    // Show the window
    window.show();
    std::cout << "\nWindow displayed with SplitView\n";
    std::cout << "Primary pane features:\n";
    std::cout << "  - VStack with 3 navigation buttons\n";
    std::cout << "  - Initial width: 250pt\n";
    std::cout << "  - Drag divider to resize (constrained to 150-400pt)\n";
    std::cout << "  - Double-click divider to collapse/expand\n";
    std::cout << "\nSecondary pane features:\n";
    std::cout << "  - VStack with 2 action buttons\n";
    std::cout << "  - Resizes automatically when primary pane changes\n";
    std::cout << "\nInteract with the buttons, resize the primary pane, and toggle collapse/expand.\n";
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
    
    std::cout << "\n=== SplitView Showcase exited ===\n";
    return 0;
}
