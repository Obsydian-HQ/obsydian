/**
 * Root layout for nested routes example
 * This layout wraps all routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    Window& window = ctx.getWindow();
    
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Add title
    TextView title;
    title.create("Router Nested Example", 0, 0, 0, 0);
    title.setFontSize(24.0);
    title.setFontWeight(FontWeight::Bold);
    layout.addChild(title);
    
    // Add navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Link homeLink;
    homeLink.create("/", "Home", 0, 0, 80, 30);
    navLinks.addChild(homeLink);
    
    Link dashboardLink;
    dashboardLink.create("/dashboard", "Dashboard", 0, 0, 100, 30);
    navLinks.addChild(dashboardLink);
    
    layout.addChild(navLinks);
    
    // Add separator
    TextView separator;
    separator.create("────────────────────────────────────", 0, 0, 0, 0);
    layout.addChild(separator);
    
    // Render child route content
    renderChild();
    
    window.setContent(layout);
}

REGISTER_LAYOUT("/", renderRootLayout);
