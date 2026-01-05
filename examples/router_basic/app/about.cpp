/**
 * About route (/about)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderAboutRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("About", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the about page. It demonstrates basic routing functionality.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    window.setContent(content);
}

REGISTER_ROUTE("/about", renderAboutRoute);
