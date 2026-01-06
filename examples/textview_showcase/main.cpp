/**
 * TextView Showcase Example Application
 * 
 * This example demonstrates all TextView features and capabilities:
 * - Display-only mode (read-only, selectable)
 * - Editable mode (can type in it)
 * - Different font sizes and weights
 * - Standalone TextView (not wrapped in ScrollView)
 * - Multiple TextView instances
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
    std::cout << "=== Obsidian TextView Showcase ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Obsidian application\n";
        return 1;
    }
    std::cout << "Obsidian application initialized successfully\n";
    
    // Create a window using the public API
    Window window;
    if (!window.create(1000, 800, "TextView Showcase")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Window created\n";
    
    // Store TextViews to keep them alive
    std::vector<std::unique_ptr<TextView>> textViews;
    
    // ============================================
    // Section 1: Display-only TextViews (Labels)
    // ============================================
    
    // Title TextView - Large, Bold
    auto titleView = std::make_unique<TextView>();
    if (titleView->create("TextView Showcase", 50, 50, 900, 50)) {
        titleView->setFontSize(36.0);
        titleView->setFontWeight(FontWeight::Bold);
        titleView->setEditable(false);
        titleView->setSelectable(true);
        titleView->addToWindow(window);
        textViews.push_back(std::move(titleView));
        std::cout << "Title TextView created\n";
    }
    
    // Subtitle TextView - Medium, Regular
    auto subtitleView = std::make_unique<TextView>();
    if (subtitleView->create("Demonstrating all TextView features and capabilities", 50, 110, 900, 30)) {
        subtitleView->setFontSize(16.0);
        subtitleView->setFontWeight(FontWeight::Regular);
        subtitleView->setEditable(false);
        subtitleView->setSelectable(true);
        subtitleView->addToWindow(window);
        textViews.push_back(std::move(subtitleView));
        std::cout << "Subtitle TextView created\n";
    }
    
    // ============================================
    // Section 2: Different Font Sizes
    // ============================================
    
    auto sizeLabel = std::make_unique<TextView>();
    if (sizeLabel->create("Font Sizes:", 50, 160, 200, 25)) {
        sizeLabel->setFontSize(14.0);
        sizeLabel->setFontWeight(FontWeight::Bold);
        sizeLabel->setEditable(false);
        sizeLabel->setSelectable(true);
        sizeLabel->addToWindow(window);
        textViews.push_back(std::move(sizeLabel));
    }
    
    // Small text (10pt)
    auto smallText = std::make_unique<TextView>();
    if (smallText->create("Small text (10pt) - This is small text", 50, 190, 300, 20)) {
        smallText->setFontSize(10.0);
        smallText->setFontWeight(FontWeight::Regular);
        smallText->setEditable(false);
        smallText->setSelectable(true);
        smallText->addToWindow(window);
        textViews.push_back(std::move(smallText));
    }
    
    // Regular text (14pt)
    auto regularText = std::make_unique<TextView>();
    if (regularText->create("Regular text (14pt) - This is regular size text", 50, 220, 300, 25)) {
        regularText->setFontSize(14.0);
        regularText->setFontWeight(FontWeight::Regular);
        regularText->setEditable(false);
        regularText->setSelectable(true);
        regularText->addToWindow(window);
        textViews.push_back(std::move(regularText));
    }
    
    // Large text (20pt)
    auto largeText = std::make_unique<TextView>();
    if (largeText->create("Large text (20pt) - This is large text", 50, 250, 300, 30)) {
        largeText->setFontSize(20.0);
        largeText->setFontWeight(FontWeight::Regular);
        largeText->setEditable(false);
        largeText->setSelectable(true);
        largeText->addToWindow(window);
        textViews.push_back(std::move(largeText));
    }
    
    // ============================================
    // Section 3: Different Font Weights
    // ============================================
    
    auto weightLabel = std::make_unique<TextView>();
    if (weightLabel->create("Font Weights:", 400, 160, 200, 25)) {
        weightLabel->setFontSize(14.0);
        weightLabel->setFontWeight(FontWeight::Bold);
        weightLabel->setEditable(false);
        weightLabel->setSelectable(true);
        weightLabel->addToWindow(window);
        textViews.push_back(std::move(weightLabel));
    }
    
    // Thin
    auto thinText = std::make_unique<TextView>();
    if (thinText->create("Thin weight text", 400, 190, 200, 25)) {
        thinText->setFontSize(14.0);
        thinText->setFontWeight(FontWeight::Thin);
        thinText->setEditable(false);
        thinText->setSelectable(true);
        thinText->addToWindow(window);
        textViews.push_back(std::move(thinText));
    }
    
    // Regular
    auto regularWeightText = std::make_unique<TextView>();
    if (regularWeightText->create("Regular weight text", 400, 220, 200, 25)) {
        regularWeightText->setFontSize(14.0);
        regularWeightText->setFontWeight(FontWeight::Regular);
        regularWeightText->setEditable(false);
        regularWeightText->setSelectable(true);
        regularWeightText->addToWindow(window);
        textViews.push_back(std::move(regularWeightText));
    }
    
    // Bold
    auto boldText = std::make_unique<TextView>();
    if (boldText->create("Bold weight text", 400, 250, 200, 25)) {
        boldText->setFontSize(14.0);
        boldText->setFontWeight(FontWeight::Bold);
        boldText->setEditable(false);
        boldText->setSelectable(true);
        boldText->addToWindow(window);
        textViews.push_back(std::move(boldText));
    }
    
    // ============================================
    // Section 4: Multi-line Display TextView
    // ============================================
    
    auto multiLineLabel = std::make_unique<TextView>();
    if (multiLineLabel->create("Multi-line TextView (read-only, selectable):", 50, 300, 900, 25)) {
        multiLineLabel->setFontSize(14.0);
        multiLineLabel->setFontWeight(FontWeight::Bold);
        multiLineLabel->setEditable(false);
        multiLineLabel->setSelectable(true);
        multiLineLabel->addToWindow(window);
        textViews.push_back(std::move(multiLineLabel));
    }
    
    auto multiLineView = std::make_unique<TextView>();
    if (multiLineView->create("", 50, 330, 900, 150)) {
        multiLineView->setFontSize(12.0);
        multiLineView->setFontWeight(FontWeight::Regular);
        multiLineView->setEditable(false);
        multiLineView->setSelectable(true);
        multiLineView->setString(
            "This is a multi-line TextView.\n"
            "You can select and copy text from it, but you cannot edit it.\n"
            "This is perfect for displaying:\n"
            "  - Log messages\n"
            "  - Command output\n"
            "  - Documentation\n"
            "  - Read-only content\n\n"
            "Notice: There's no border or background - it's just text floating on the window.\n"
            "This is the default behavior of TextView (drawsBackground = NO)."
        );
        multiLineView->addToWindow(window);
        textViews.push_back(std::move(multiLineView));
    }
    
    // ============================================
    // Section 5: Editable TextView
    // ============================================
    
    auto editableLabel = std::make_unique<TextView>();
    if (editableLabel->create("Editable TextView (you can type in this!):", 50, 500, 900, 25)) {
        editableLabel->setFontSize(14.0);
        editableLabel->setFontWeight(FontWeight::Bold);
        editableLabel->setEditable(false);
        editableLabel->setSelectable(true);
        editableLabel->addToWindow(window);
        textViews.push_back(std::move(editableLabel));
    }
    
    auto editableView = std::make_unique<TextView>();
    if (editableView->create("Click here and start typing...", 50, 530, 900, 100)) {
        editableView->setFontSize(14.0);
        editableView->setFontWeight(FontWeight::Regular);
        editableView->setEditable(true);  // THIS IS THE KEY - makes it editable!
        editableView->setSelectable(true);
        editableView->addToWindow(window);
        textViews.push_back(std::move(editableView));
        std::cout << "Editable TextView created - you can type in it!\n";
    }
    
    // ============================================
    // Section 6: Instructions
    // ============================================
    
    auto instructionsLabel = std::make_unique<TextView>();
    if (instructionsLabel->create("Instructions:", 50, 650, 200, 25)) {
        instructionsLabel->setFontSize(14.0);
        instructionsLabel->setFontWeight(FontWeight::Bold);
        instructionsLabel->setEditable(false);
        instructionsLabel->setSelectable(true);
        instructionsLabel->addToWindow(window);
        textViews.push_back(std::move(instructionsLabel));
    }
    
    auto instructionsView = std::make_unique<TextView>();
    if (instructionsView->create("", 50, 680, 900, 100)) {
        instructionsView->setFontSize(11.0);
        instructionsView->setFontWeight(FontWeight::Regular);
        instructionsView->setEditable(false);
        instructionsView->setSelectable(true);
        instructionsView->setString(
            "1. All TextViews above are display-only (read-only, selectable) except the 'Editable TextView' section.\n"
            "2. Try selecting text from any TextView - you should be able to copy it.\n"
            "3. Click in the 'Editable TextView' section and type - you should see a cursor and be able to edit.\n"
            "4. Notice: TextViews have NO border or background by default - they're just text.\n"
            "5. TextView is perfect for labels, read-only content, and editable multi-line text areas.\n"
            "6. For single-line input with a border, use TextField instead."
        );
        instructionsView->addToWindow(window);
        textViews.push_back(std::move(instructionsView));
    }
    
    // Show the window
    window.show();
    std::cout << "\nWindow displayed with " << textViews.size() << " TextView instances\n";
    std::cout << "Interact with the TextViews to test functionality:\n";
    std::cout << "  - Try selecting text from display-only TextViews\n";
    std::cout << "  - Try typing in the editable TextView\n";
    std::cout << "  - Notice the visual differences (no borders, just text)\n";
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
    textViews.clear();  // TextViews will be destroyed automatically
    window.close();
    app.shutdown();
    
    std::cout << "\n=== TextView Showcase exited ===\n";
    return 0;
}
