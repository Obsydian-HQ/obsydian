/**
 * Dashboard settings route (/dashboard/settings)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderDashboardSettingsRoute(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Dashboard Settings", 0, 0, 0, 0);
    title.setFontSize(28.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the dashboard settings page. It shares the dashboard layout with the overview page.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    TextView note;
    note.create("The sidebar navigation is provided by the dashboard layout, demonstrating nested layout functionality.", 0, 0, 0, 0);
    note.setFontSize(12.0);
    content.addChild(note);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/dashboard/settings", renderDashboardSettingsRoute);
