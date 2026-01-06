/**
 * Root layout - wraps all routes
 * 
 * Uses content slot pattern for proper nesting
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    // Main layout container
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Button homeButton;
    homeButton.create("Home", 0, 0, 80, 30);
    Link homeLink;
    homeLink.create("/", homeButton);
    navLinks.addChild(homeLink);
    
    Button blogButton;
    blogButton.create("Blog", 0, 0, 80, 30);
    Link blogLink;
    blogLink.create("/blog", blogButton);
    navLinks.addChild(blogLink);
    
    Button adminButton;
    adminButton.create("Admin", 0, 0, 80, 30);
    Link adminLink;
    adminLink.create("/admin", adminButton);
    navLinks.addChild(adminLink);
    
    layout.addChild(navLinks);
    
    // Content slot for child routes
    VStack contentSlot;
    contentSlot.setSpacing(10.0);
    layout.addChild(contentSlot);
    
    // STEP 1: Add layout to screen FIRST (bypass setContent to avoid contentSlot check)
    Screen* screen = ctx.getScreen();
    if (screen) {
        layout.addToScreen(*screen);
    } else {
        layout.addToWindow(ctx.getWindow());
    }
    
    // STEP 2: Set content slot for children
    ctx.setContentSlot(contentSlot.getNativeViewHandle());
    
    // STEP 3: Render children - they go into contentSlot via setContent()
    renderChild();
}

REGISTER_LAYOUT("/", renderRootLayout);
