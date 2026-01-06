/**
 * Root layout for navigation patterns example
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
    
    // Add title
    TextView title;
    title.create("Router Patterns Example", 0, 0, 0, 0);
    title.setFontSize(24.0);
    title.setFontWeight(FontWeight::Bold);
    layout.addChild(title);
    
    // Add navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Button homeButton;
    homeButton.create("Home", 0, 0, 80, 30);
    Link homeLink;
    homeLink.create("/", homeButton);
    navLinks.addChild(homeLink);
    
    Button page1Button;
    page1Button.create("Page 1", 0, 0, 80, 30);
    Link page1Link;
    page1Link.create("/page1", page1Button);
    navLinks.addChild(page1Link);
    
    Button page2Button;
    page2Button.create("Page 2", 0, 0, 80, 30);
    Link page2Link;
    page2Link.create("/page2", page2Button);
    navLinks.addChild(page2Link);
    
    Button page3Button;
    page3Button.create("Page 3", 0, 0, 80, 30);
    Link page3Link;
    page3Link.create("/page3", page3Button);
    navLinks.addChild(page3Link);
    
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
