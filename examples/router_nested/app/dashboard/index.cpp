/**
 * Dashboard overview route (/dashboard)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderDashboardRoute(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Dashboard Overview", 0, 0, 0, 0);
    title.setFontSize(28.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the dashboard overview page. It's nested under /dashboard with its own layout.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    TextView note;
    note.create("Notice how the dashboard layout provides sidebar navigation that persists across dashboard routes.", 0, 0, 0, 0);
    note.setFontSize(12.0);
    content.addChild(note);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/dashboard", renderDashboardRoute);
