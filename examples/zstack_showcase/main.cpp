/**
 * ZStack Showcase Example Application
 * 
 * Comprehensive demonstration of ZStack functionality for manual verification:
 * - Basic overlay behavior
 * - All alignment options
 * - Z-order verification
 * - Window resize behavior
 */

#include <iostream>
#include <obsidian/obsidian.h>

using namespace obsidian;

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Obsidian ZStack Showcase Example ===\n\n";
    
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
    
    if (!window.create(900, 700, "ZStack Showcase - Manual Verification")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    std::cout << "Window created successfully\n";
    std::cout << "\n=== ZStack Test: Center Alignment ===\n";
    
    // TEST 1: Center Alignment - Basic Overlay (MiddleCenter = center both horizontally and vertically)
    ZStack zstack_center;
    zstack_center.setAlignment(layout::Alignment::MiddleCenter);
    
    // Create three overlapping buttons of different sizes
    // Last button added should be on top
    Button center_button1;
    center_button1.create("Bottom Layer (150x50)", 0, 0, 150, 50);
    center_button1.setOnClick([]() {
        std::cout << "Center Button 1 (Bottom) clicked!\n";
    });
    
    Button center_button2;
    center_button2.create("Middle (120x40)", 0, 0, 120, 40);
    center_button2.setOnClick([]() {
        std::cout << "Center Button 2 (Middle) clicked!\n";
    });
    
    Button center_button3;
    center_button3.create("Top (100x30)", 0, 0, 100, 30);
    center_button3.setOnClick([]() {
        std::cout << "Center Button 3 (Top) clicked!\n";
    });
    
    // Add buttons to ZStack (last one should be on top)
    zstack_center.addChild(center_button1);
    zstack_center.addChild(center_button2);
    zstack_center.addChild(center_button3);
    
    zstack_center.addToWindow(window);
    
    std::cout << "VERIFICATION CHECKLIST:\n";
    std::cout << "  [ ] Three buttons overlaid at window center\n";
    std::cout << "  [ ] Largest button (150x50) is behind\n";
    std::cout << "  [ ] Medium button (120x40) is in middle\n";
    std::cout << "  [ ] Smallest button (100x30) is on top\n";
    std::cout << "  [ ] All buttons are centered horizontally and vertically\n";
    std::cout << "  [ ] Clicking should activate the top button only\n";
    std::cout << "  [ ] Resize window - buttons should stay centered\n\n";
    
    window.show();
    std::cout << "Window displayed with ZStack (Center alignment)\n";
    std::cout << "\nINSTRUCTIONS:\n";
    std::cout << "1. Verify the three buttons are overlaid at the center of the window\n";
    std::cout << "2. Verify the smallest button (100x30) is on top\n";
    std::cout << "3. Try clicking - only the top button should respond\n";
    std::cout << "4. Resize the window - buttons should stay centered\n";
    std::cout << "5. Close the window to exit\n\n";
    
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
    
    // Run the application (this blocks until app quits)
    std::cout << "Running Obsidian application...\n";
    std::cout << "Close the window to exit.\n\n";
    
    app.run(callbacks);
    
    // Cleanup is automatic (RAII)
    window.close();
    app.shutdown();
    
    std::cout << "\n=== Application exited ===\n";
    return 0;
}
