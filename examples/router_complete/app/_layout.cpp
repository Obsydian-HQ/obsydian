/**
 * Root layout - wraps all routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    Window& window = ctx.getWindow();
    
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Header
    HStack header;
    header.setSpacing(20.0);
    
    TextView title;
    title.create("My App", 0, 0, 0, 0);
    title.setFontSize(28.0);
    title.setFontWeight(FontWeight::Bold);
    header.addChild(title);
    
    Spacer spacer;
    header.addChild(spacer);
    
    // Navigation
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Link homeLink;
    homeLink.create("/", "Home", 0, 0, 80, 30);
    navLinks.addChild(homeLink);
    
    Link blogLink;
    blogLink.create("/blog", "Blog", 0, 0, 80, 30);
    navLinks.addChild(blogLink);
    
    Link adminLink;
    adminLink.create("/admin", "Admin", 0, 0, 80, 30);
    navLinks.addChild(adminLink);
    
    header.addChild(navLinks);
    layout.addChild(header);
    
    // Separator
    TextView separator;
    separator.create("────────────────────────────────────", 0, 0, 0, 0);
    layout.addChild(separator);
    
    // Render child route content
    renderChild();
    
    window.setContent(layout);
}

REGISTER_LAYOUT("/", renderRootLayout);
