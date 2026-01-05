/**
 * Dashboard layout - wraps /dashboard routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderDashboardLayout(RouteContext& ctx, std::function<void()> renderChild) {
    
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
    
    // Main content area
    VStack content;
    content.setSpacing(15.0);
    content.setPadding(Padding::all(15.0));
    
    // Render child route content
    renderChild();
    
    layout.addChild(content);
    
    ctx.setContent(layout);
}

REGISTER_LAYOUT("/dashboard", renderDashboardLayout);
