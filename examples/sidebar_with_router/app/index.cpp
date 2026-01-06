/**
 * Home Page Route (/)
 */

#include <iostream>
#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void HomePage(RouteContext& ctx) {
    std::cout << "[HomePage] Rendering\n";
    
    VStack content;
    content.setSpacing(16.0);
    content.setAlignment(layout::Alignment::TopLeading);
    content.setPadding(Padding::all(32.0));
    
    TextView title;
    title.create("Home", 0, 0, 400, 50);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView subtitle;
    subtitle.create("Welcome to the Sidebar + Router example!", 0, 0, 500, 30);
    subtitle.setFontSize(16.0);
    content.addChild(subtitle);
    
    TextView description;
    description.create(
        "This example demonstrates the proper way to integrate a Sidebar with a Router.\n\n"
        "Key points:\n"
        "- The Sidebar owns the window's content view\n"
        "- The Router's ScreenContainer is attached to the sidebar's main area\n"
        "- Navigation buttons in the sidebar call router.navigate()\n"
        "- This route content renders inside the sidebar's main area",
        0, 0, 600, 200
    );
    description.setFontSize(14.0);
    content.addChild(description);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/", HomePage);
