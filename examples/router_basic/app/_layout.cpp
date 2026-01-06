/**
 * Root layout for router basic example
 * This layout wraps all routes
 * 
 * Layout pattern (based on Expo Router/React Navigation):
 * 1. Create the layout structure with a content slot for children
 * 2. Register the content slot with RouteContext
 * 3. Add the layout to the screen
 * 4. Call renderChild() - children will render into the slot
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderRootLayout(RouteContext& ctx, std::function<void()> renderChild) {
    // Main layout container
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(30.0));
    
    // Header: Title
    TextView title;
    title.create("Router Basic Example", 0, 0, 0, 0);
    title.setFontSize(24.0);
    title.setFontWeight(FontWeight::Bold);
    layout.addChild(title);
    
    // Navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Button homeButton;
    homeButton.create("Home", 0, 0, 80, 30);
    Link homeLink;
    homeLink.create("/", homeButton);
    navLinks.addChild(homeLink);
    
    Button aboutButton;
    aboutButton.create("About", 0, 0, 80, 30);
    Link aboutLink;
    aboutLink.create("/about", aboutButton);
    navLinks.addChild(aboutLink);
    
    Button contactButton;
    contactButton.create("Contact", 0, 0, 80, 30);
    Link contactLink;
    contactLink.create("/contact", contactButton);
    navLinks.addChild(contactLink);
    
    layout.addChild(navLinks);
    
    // Content slot: Where child routes will render
    // This VStack acts as a container for nested route content
    VStack contentSlot;
    contentSlot.setSpacing(10.0);
    layout.addChild(contentSlot);
    
    // STEP 1: Add the layout to the screen FIRST (bypass setContent to avoid contentSlot check)
    // This is the correct pattern: layout goes to screen, then children go into contentSlot
    Screen* screen = ctx.getScreen();
    if (screen) {
        layout.addToScreen(*screen);
    } else {
        layout.addToWindow(ctx.getWindow());
    }
    
    // STEP 2: Now set the content slot for children
    // Children rendered via renderChild() will use setContent() which will add them here
    ctx.setContentSlot(contentSlot.getNativeViewHandle());
    
    // STEP 3: Render children - they will go into contentSlot via setContent()
    renderChild();
}

REGISTER_LAYOUT("/", renderRootLayout);
