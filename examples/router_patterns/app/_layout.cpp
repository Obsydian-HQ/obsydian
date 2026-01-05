/**
 * Root layout for navigation patterns example
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Add title
    TextView title;
    title.create("Router Patterns Example", 0, 0, 0, 0);
    title.setFontSize(24.0);
    title.setFontWeight(FontWeight::Bold);
    layout.addChild(title);
    
    // Add navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Link homeLink;
    homeLink.create("/", "Home", 0, 0, 80, 30);
    navLinks.addChild(homeLink);
    
    Link page1Link;
    page1Link.create("/page1", "Page 1", 0, 0, 80, 30);
    navLinks.addChild(page1Link);
    
    Link page2Link;
    page2Link.create("/page2", "Page 2", 0, 0, 80, 30);
    navLinks.addChild(page2Link);
    
    Link page3Link;
    page3Link.create("/page3", "Page 3", 0, 0, 80, 30);
    navLinks.addChild(page3Link);
    
    layout.addChild(navLinks);
    
    // Add separator
    TextView separator;
    separator.create("────────────────────────────────────", 0, 0, 0, 0);
    layout.addChild(separator);
    
    // Render child route content
    renderChild();
    
    ctx.setContent(layout);
}

REGISTER_LAYOUT("/", renderRootLayout);
