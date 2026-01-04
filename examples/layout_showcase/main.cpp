/**
 * Layout Showcase example application
 * 
 * Demonstrates VStack, HStack, and ZStack layout components with:
 * - Vertical and horizontal stacking of buttons
 * - Overlay stacking (ZStack)
 * - Spacing configuration
 * - Alignment options
 * - Padding support
 */

#include <iostream>
#include <obsidian/obsidian.h>

using namespace obsidian;

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Obsidian Layout Showcase Example ===\n\n";
    
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
    
    if (!window.create(800, 600, "Layout Showcase - VStack, HStack & ZStack Demo")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    std::cout << "Window created successfully\n";
    
    // Create a VStack for vertical layout (positioned at top)
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::TopLeading);
    vstack.setPadding(Padding::all(20.0));
    
    // Create buttons for VStack
    Button vbutton1;
    vbutton1.create("VButton 1", 0, 0, 100, 30);
    vbutton1.setOnClick([]() {
        std::cout << "VButton 1 clicked!\n";
    });
    
    Button vbutton2;
    vbutton2.create("VButton 2", 0, 0, 100, 30);
    vbutton2.setOnClick([]() {
        std::cout << "VButton 2 clicked!\n";
    });
    
    Button vbutton3;
    vbutton3.create("VButton 3", 0, 0, 100, 30);
    vbutton3.setOnClick([]() {
        std::cout << "VButton 3 clicked!\n";
    });
    
    // Add buttons to VStack
    vstack.addChild(vbutton1);
    vstack.addChild(vbutton2);
    vstack.addChild(vbutton3);
    
    // Add VStack to window (positioned at top)
    vstack.addToWindow(window);
    
    // Create an HStack for horizontal layout (positioned at bottom)
    HStack hstack;
    hstack.setSpacing(15.0);
    hstack.setAlignment(layout::Alignment::BottomLeading);
    hstack.setPadding(Padding::symmetric(10.0, 20.0));
    
    // Create buttons for HStack
    Button hbutton1;
    hbutton1.create("HButton 1", 0, 0, 80, 30);
    hbutton1.setOnClick([]() {
        std::cout << "HButton 1 clicked!\n";
    });
    
    Button hbutton2;
    hbutton2.create("HButton 2", 0, 0, 80, 30);
    hbutton2.setOnClick([]() {
        std::cout << "HButton 2 clicked!\n";
    });
    
    Button hbutton3;
    hbutton3.create("HButton 3", 0, 0, 80, 30);
    hbutton3.setOnClick([]() {
        std::cout << "HButton 3 clicked!\n";
    });
    
    Button hbutton4;
    hbutton4.create("HButton 4", 0, 0, 80, 30);
    hbutton4.setOnClick([]() {
        std::cout << "HButton 4 clicked!\n";
    });
    
    // Add buttons to HStack
    hstack.addChild(hbutton1);
    hstack.addChild(hbutton2);
    hstack.addChild(hbutton3);
    hstack.addChild(hbutton4);
    
    // Add HStack to window
    hstack.addToWindow(window);
    
    // ZStack demonstration
    // Note: ZStack fills the parent view, so it will overlay VStack and HStack
    // To see ZStack separately, comment out VStack and HStack addToWindow calls above
    // and uncomment the ZStack code below
    
    /*
    ZStack zstack;
    zstack.setAlignment(layout::Alignment::Center);
    
    // Create overlapping buttons for ZStack
    Button zbutton1;
    zbutton1.create("ZButton 1 (Bottom)", 0, 0, 150, 50);
    zbutton1.setOnClick([]() {
        std::cout << "ZButton 1 clicked!\n";
    });
    
    Button zbutton2;
    zbutton2.create("ZButton 2 (Middle)", 0, 0, 120, 40);
    zbutton2.setOnClick([]() {
        std::cout << "ZButton 2 clicked!\n";
    });
    
    Button zbutton3;
    zbutton3.create("ZButton 3 (Top)", 0, 0, 100, 30);
    zbutton3.setOnClick([]() {
        std::cout << "ZButton 3 clicked!\n";
    });
    
    // Add buttons to ZStack (last one will be on top)
    zstack.addChild(zbutton1);
    zstack.addChild(zbutton2);
    zstack.addChild(zbutton3);
    
    // Add ZStack to window (fills the window)
    zstack.addToWindow(window);
    */
    
    window.show();
    std::cout << "Window displayed with VStack and HStack layouts\n";
    std::cout << "VStack (top): Buttons arranged vertically with 10pt spacing, 20pt padding, TopLeading alignment\n";
    std::cout << "HStack (bottom): Buttons arranged horizontally with 15pt spacing, 10pt vertical/20pt horizontal padding, BottomLeading alignment\n";
    std::cout << "\nTo test ZStack: Comment out VStack/HStack addToWindow calls and uncomment ZStack code\n";
    std::cout << "ZStack: Overlays buttons at center alignment, last button on top\n";
    std::cout << "\nManual Verification Checklist:\n";
    std::cout << "  ✓ VStack: 3 buttons stacked vertically at top-left\n";
    std::cout << "  ✓ HStack: 4 buttons arranged horizontally at bottom-left\n";
    std::cout << "  ✓ HStack spacing: 15pt between buttons\n";
    std::cout << "  ✓ HStack padding: 10pt top/bottom, 20pt left/right\n";
    std::cout << "  ✓ HStack vertical alignment: buttons aligned at bottom\n";
    std::cout << "  ✓ ZStack: Buttons overlaid at center, last button on top (when enabled)\n";
    std::cout << "  ✓ Window resize: layouts update correctly\n";
    
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
    
    // Run the application event loop
    std::cout << "\nRunning Obsidian application...\n";
    std::cout << "Window should be visible. Close it to exit.\n";
    
    // Run the application (this blocks until app quits)
    app.run(callbacks);
    
    // Cleanup is automatic (RAII), but we can be explicit
    window.close();
    app.shutdown();
    
    std::cout << "\n=== Application exited ===\n";
    return 0;
}
