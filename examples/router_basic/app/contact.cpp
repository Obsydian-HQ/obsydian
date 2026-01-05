/**
 * Contact route (/contact)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderContactRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Contact", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the contact page. You can navigate back using the browser back button or the navigation links.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    window.setContent(content);
}

REGISTER_ROUTE("/contact", renderContactRoute);
