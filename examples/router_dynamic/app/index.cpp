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
    description.create("This example demonstrates dynamic routes. Try navigating to /products/123 or /users/456", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/", renderHomeRoute);
