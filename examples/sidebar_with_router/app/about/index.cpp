/**
 * About Page Route (/about)
 */

#include <iostream>
#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void AboutPage(RouteContext& ctx) {
    std::cout << "[AboutPage] Rendering\n";
    
    VStack content;
    content.setSpacing(16.0);
    content.setAlignment(layout::Alignment::TopLeading);
    content.setPadding(Padding::all(32.0));
    
    TextView title;
    title.create("About", 0, 0, 400, 50);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create(
        "Obsidian Framework\n\n"
        "A modern C++ framework for building native macOS applications.\n\n"
        "Features:\n"
        "- Native macOS components (NSView, NSSplitViewController, etc.)\n"
        "- Flexbox-inspired layout engine\n"
        "- File-based routing system\n"
        "- Layout containers (VStack, HStack, ZStack)\n"
        "- Native controls (Button, TextView, etc.)",
        0, 0, 600, 250
    );
    description.setFontSize(14.0);
    content.addChild(description);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/about", AboutPage);
