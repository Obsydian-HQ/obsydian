/**
 * Root layout - wraps all routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    // Use a single VStack for the layout
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Navigation links
    Link homeLink;
    homeLink.create("/", "Home", 0, 0, 80, 30);
    layout.addChild(homeLink);
    
    Link blogLink;
    blogLink.create("/blog", "Blog", 0, 0, 80, 30);
    layout.addChild(blogLink);
    
    Link adminLink;
    adminLink.create("/admin", "Admin", 0, 0, 80, 30);
    layout.addChild(adminLink);
    
    // Render child route content first
    renderChild();
    
    // Add layout to screen/window
    ctx.setContent(layout);
}

// Register layout using function call (macro doesn't exist)
static bool _registered_root_layout = []() {
    if (g_router) {
        registerLayout("/", renderRootLayout);
    }
    return true;
}();
