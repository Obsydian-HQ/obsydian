/**
 * UI Showcase Example Application
 * 
 * This example demonstrates all Obsidian UI components working together.
 * As we add more UI elements (text fields, labels, etc.), they will be
 * added to this showcase app.
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
    std::cout << "=== Obsidian UI Showcase ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Obsidian application\n";
        return 1;
    }
    std::cout << "Obsidian application initialized successfully\n";
    
    // Create a window using the public API
    Window window;
    if (!window.create(800, 600, "Obsidian UI Showcase")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Window created\n";
    
    // Create multiple buttons to showcase button functionality
    std::vector<std::unique_ptr<Button>> buttons;
    
    // Button 1: Basic button
    auto button1 = std::make_unique<Button>();
    if (button1->create("Click Me!", 50, 300, 150, 30)) {
        button1->addToWindow(window);
        button1->setOnClick([]() {
            std::cout << "Button 1 clicked!\n";
        });
        buttons.push_back(std::move(button1));
        std::cout << "Button 1 created and added to window\n";
    }
    
    // Button 2: Disabled button
    auto button2 = std::make_unique<Button>();
    if (button2->create("Disabled", 220, 300, 150, 30)) {
        button2->setEnabled(false);
        button2->addToWindow(window);
        buttons.push_back(std::move(button2));
        std::cout << "Button 2 created (disabled)\n";
    }
    
    // Button 3: Hidden button (will be shown later)
    auto button3 = std::make_unique<Button>();
    if (button3->create("Hidden Button", 50, 250, 150, 30)) {
        button3->setVisible(false);
        button3->addToWindow(window);
        buttons.push_back(std::move(button3));
        std::cout << "Button 3 created (hidden)\n";
    }
    
    // Button 4: Counter button
    int clickCount = 0;
    auto button4 = std::make_unique<Button>();
    if (button4->create("Count: 0", 220, 250, 150, 30)) {
        button4->addToWindow(window);
        // Store button pointer before moving it
        Button* button4Ptr = button4.get();
        button4->setOnClick([button4Ptr, &clickCount]() {
            clickCount++;
            button4Ptr->setTitle("Count: " + std::to_string(clickCount));
            std::cout << "Counter: " << clickCount << "\n";
        });
        buttons.push_back(std::move(button4));
        std::cout << "Button 4 created (counter)\n";
    }
    
    // Button 5: Toggle visibility button
    auto button5 = std::make_unique<Button>();
    if (button5->create("Show Hidden", 50, 200, 150, 30)) {
        button5->addToWindow(window);
        button5->setOnClick([&buttons]() {
            if (buttons.size() >= 3) {
                bool isVisible = buttons[2]->isVisible();
                buttons[2]->setVisible(!isVisible);
                std::cout << "Button 3 visibility toggled: " << (isVisible ? "hidden" : "visible") << "\n";
            }
        });
        buttons.push_back(std::move(button5));
        std::cout << "Button 5 created (toggle visibility)\n";
    }
    
    // Create text fields to showcase text input functionality
    std::vector<std::unique_ptr<TextField>> textFields;
    
    // TextField 1: Basic text field
    auto textField1 = std::make_unique<TextField>();
    if (textField1->create("Enter text here...", 50, 150, 200, 25)) {
        textField1->addToWindow(window);
        textField1->setOnEnter([textField1Ptr = textField1.get()]() {
            std::string text = textField1Ptr->getText();
            std::cout << "TextField 1 entered: " << text << "\n";
        });
        textFields.push_back(std::move(textField1));
        std::cout << "TextField 1 created and added to window\n";
    }
    
    // TextField 2: Disabled text field
    auto textField2 = std::make_unique<TextField>();
    if (textField2->create("Disabled field", 50, 120, 200, 25)) {
        textField2->setEnabled(false);
        textField2->setText("Cannot edit this");
        textField2->addToWindow(window);
        textFields.push_back(std::move(textField2));
        std::cout << "TextField 2 created (disabled)\n";
    }
    
    // TextField 3: Echo text field (shows input in console)
    auto textField3 = std::make_unique<TextField>();
    if (textField3->create("Type and press Enter", 50, 90, 200, 25)) {
        textField3->addToWindow(window);
        textField3->setOnEnter([textField3Ptr = textField3.get()]() {
            std::string text = textField3Ptr->getText();
            std::cout << "Echo: " << text << "\n";
            // Clear the field after echoing
            textField3Ptr->setText("");
        });
        textFields.push_back(std::move(textField3));
        std::cout << "TextField 3 created (echo)\n";
    }
    
    // Create a scrollable text view to showcase TextView and ScrollView
    ScrollView scrollView;
    TextView textView;
    
    if (scrollView.create(300, 50, 450, 200)) {
        if (textView.create(0, 0, 450, 200)) {
            // Configure text view as read-only but selectable (for terminal output simulation)
            textView.setEditable(false);
            textView.setSelectable(true);
            
            // Set some sample text
            textView.setString("=== Scrollable Text View ===\n\n"
                             "This is a TextView wrapped in a ScrollView.\n"
                             "You can scroll this content if it exceeds the visible area.\n\n"
                             "Line 1: This demonstrates the low-level TextView API\n"
                             "Line 2: Which is a 1:1 mapping to NSTextView\n"
                             "Line 3: And ScrollView is a 1:1 mapping to NSScrollView\n"
                             "Line 4: The Glass app will compose these to create text areas\n\n"
                             "Line 5: This text is selectable but not editable\n"
                             "Line 6: Perfect for displaying command output\n"
                             "Line 7: Or log messages\n"
                             "Line 8: Or any multi-line read-only content\n\n"
                             "Line 9: Keep scrolling to see more lines...\n"
                             "Line 10: The scroll view handles scrolling automatically\n"
                             "Line 11: When content exceeds the visible area\n"
                             "Line 12: Scroll bars appear automatically\n"
                             "Line 13: This is native AppKit behavior\n"
                             "Line 14: Exposed through Obsidian's low-level APIs\n"
                             "Line 15: End of sample text.");
            
            // Set the text view as the document view of the scroll view
            scrollView.setDocumentView(textView);
            
            // Add scroll view to window
            scrollView.addToWindow(window);
            
            std::cout << "ScrollView and TextView created and added to window\n";
        }
    }
    
    // Create a table to showcase Table component functionality (multi-column)
    Table table;
    
    if (table.create(50, 300, 700, 250)) {
        // Add columns
        table.addColumn("name", "Name", 200);
        table.addColumn("status", "Status", 150);
        table.addColumn("value", "Value", 150);
        table.addColumn("description", "Description", 200);
        
        // Add sample rows
        table.addRow(std::vector<std::string>{"Process 1", "Running", "100%", "Main application process"});
        table.addRow(std::vector<std::string>{"Process 2", "Stopped", "0%", "Background service"});
        table.addRow(std::vector<std::string>{"Process 3", "Running", "45%", "Worker thread"});
        table.addRow(std::vector<std::string>{"Process 4", "Paused", "25%", "Suspended task"});
        table.addRow(std::vector<std::string>{"Process 5", "Running", "80%", "Network handler"});
        table.addRow(std::vector<std::string>{"Process 6", "Stopped", "0%", "Idle process"});
        table.addRow(std::vector<std::string>{"Process 7", "Running", "60%", "File I/O handler"});
        table.addRow(std::vector<std::string>{"Process 8", "Running", "30%", "UI renderer"});
        
        // Set selection callback
        table.setOnSelection([](int rowIndex) {
            std::cout << "Table row selected: " << rowIndex << "\n";
        });
        
        // Add table to window
        table.addToWindow(window);
        
        std::cout << "Table created with " << table.getColumnCount() << " columns and " << table.getRowCount() << " rows\n";
    }
    
    // Create a list to showcase List component functionality (single-column)
    List list;
    
    if (list.create(770, 300, 300, 250)) {
        // Add simple text items (no columns needed!)
        list.addItem("Item 1: First task");
        list.addItem("Item 2: Second task");
        list.addItem("Item 3: Third task");
        list.addItem("Item 4: Fourth task");
        list.addItem("Item 5: Fifth task");
        list.addItem("Item 6: Sixth task");
        list.addItem("Item 7: Seventh task");
        list.addItem("Item 8: Eighth task");
        list.addItem("Item 9: Ninth task");
        list.addItem("Item 10: Tenth task");
        
        // Set selection callback
        list.setOnSelection([](int itemIndex) {
            std::cout << "List item selected: " << itemIndex << "\n";
        });
        
        // Add list to window
        list.addToWindow(window);
        
        std::cout << "List created with " << list.getItemCount() << " items\n";
    }
    
    // Show the window
    window.show();
    std::cout << "\nWindow displayed with " << buttons.size() << " buttons, " << textFields.size() << " text fields, 1 scrollable text view, 1 table (multi-column), and 1 list (single-column)\n";
    std::cout << "Interact with the buttons, text fields, table, and list to test functionality.\n";
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
    buttons.clear();  // Buttons will be destroyed automatically
    textFields.clear();  // Text fields will be destroyed automatically
    window.close();
    app.shutdown();
    
    std::cout << "\n=== UI Showcase exited ===\n";
    return 0;
}
