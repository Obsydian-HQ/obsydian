/**
 * Dashboard layout - wraps /dashboard routes
 * 
 * Uses content slot pattern for proper nesting
 * Demonstrates sidebar + content area layout
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderDashboardLayout(RouteContext& ctx, std::function<void()> renderChild) {
    // Horizontal layout: sidebar | content
    HStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(20.0));
    
    // Sidebar navigation
    VStack sidebar;
    sidebar.setSpacing(10.0);
    sidebar.setPadding(Padding::all(15.0));
    
    TextView sidebarTitle;
    sidebarTitle.create("Dashboard", 0, 0, 0, 0);
    sidebarTitle.setFontSize(18.0);
    sidebarTitle.setFontWeight(FontWeight::Bold);
    sidebar.addChild(sidebarTitle);
    
    Link overviewLink;
    overviewLink.create("/dashboard", "Overview", 0, 0, 120, 30);
    sidebar.addChild(overviewLink);
    
    Link settingsLink;
    settingsLink.create("/dashboard/settings", "Settings", 0, 0, 120, 30);
    sidebar.addChild(settingsLink);
    
    layout.addChild(sidebar);
    
    // Content slot - where child routes will render
    VStack contentSlot;
    contentSlot.setSpacing(15.0);
    contentSlot.setPadding(Padding::all(15.0));
    layout.addChild(contentSlot);
    
    // STEP 1: Add THIS layout to PARENT's content slot (via setContent)
    // setContent uses the parent's contentSlot_ which was set by the root layout
    ctx.setContent(layout);
    
    // STEP 2: Set OUR content slot for OUR children
    ctx.setContentSlot(contentSlot.getNativeViewHandle());
    
    // STEP 3: Render children - they go into OUR contentSlot
    renderChild();
}

REGISTER_LAYOUT("/dashboard", renderDashboardLayout);
