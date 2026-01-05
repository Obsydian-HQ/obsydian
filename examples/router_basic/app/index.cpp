/**
 * Home route (/)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderHomeRoute(RouteContext& ctx) {
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Welcome Home", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the home page. Use the navigation links above to explore other routes.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Use ctx.setContent() which uses Screen if available, falls back to Window
    ctx.setContent(content);
}

REGISTER_ROUTE("/", renderHomeRoute);
