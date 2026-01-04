/**
 * Layout Showcase example application
 * 
 * Demonstrates VStack layout component with:
 * - Vertical stacking of buttons
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
    
    if (!window.create(800, 600, "Layout Showcase - VStack Demo")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    std::cout << "Window created successfully\n";
    
    // Create a VStack
    VStack vstack;
    vstack.setSpacing(10.0);
    vstack.setAlignment(layout::Alignment::Center);
    vstack.setPadding(Padding::all(20.0));
    
    // Create buttons to add to the stack
    Button button1;
    button1.create("Button 1", 0, 0, 100, 30);
    button1.setOnClick([]() {
        std::cout << "Button 1 clicked!\n";
    });
    
    Button button2;
    button2.create("Button 2", 0, 0, 100, 30);
    button2.setOnClick([]() {
        std::cout << "Button 2 clicked!\n";
    });
    
    Button button3;
    button3.create("Button 3", 0, 0, 100, 30);
    button3.setOnClick([]() {
        std::cout << "Button 3 clicked!\n";
    });
    
    Button button4;
    button4.create("Button 4", 0, 0, 100, 30);
    button4.setOnClick([]() {
        std::cout << "Button 4 clicked!\n";
    });
    
    // Add buttons to VStack
    vstack.addChild(button1);
    vstack.addChild(button2);
    vstack.addChild(button3);
    vstack.addChild(button4);
    
    // Add VStack to window
    vstack.addToWindow(window);
    
    window.show();
    std::cout << "Window displayed with VStack layout\n";
    std::cout << "Buttons should be arranged vertically with 10pt spacing\n";
    std::cout << "VStack should have 20pt padding on all sides\n";
    
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
