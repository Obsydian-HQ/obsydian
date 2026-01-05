/**
 * Root layout for router basic example
 * This layout wraps all routes
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
    title.create("Router Basic Example", 0, 0, 0, 0);
    title.setFontSize(24.0);
    title.setFontWeight(FontWeight::Bold);
    layout.addChild(title);
    
    // Add navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Link homeLink;
    homeLink.create("/", "Home", 0, 0, 80, 30);
    navLinks.addChild(homeLink);
    
    Link aboutLink;
    aboutLink.create("/about", "About", 0, 0, 80, 30);
    navLinks.addChild(aboutLink);
    
    Link contactLink;
    contactLink.create("/contact", "Contact", 0, 0, 80, 30);
    navLinks.addChild(contactLink);
    
    layout.addChild(navLinks);
    
    // Render child route content
    renderChild();
    
    ctx.setContent(layout);
}

REGISTER_LAYOUT("/", renderRootLayout);
