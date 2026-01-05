/**
 * Page 2 route (/page2)
 * Demonstrates push() pattern
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderPage2Route(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Page 2", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This page demonstrates the push() pattern. Use push() to add a new route to the history stack.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Push button
    Button pushBtn;
    pushBtn.create("Push Page 3", 0, 0, 150, 30);
    pushBtn.setOnClick([&ctx]() {
        ctx.navigate("/page3");
    });
    content.addChild(pushBtn);
    
    TextView note;
    note.create("Note: After push, you can go back to Page 2", 0, 0, 0, 0);
    note.setFontSize(12.0);
    content.addChild(note);
    
    window.setContent(content);
}

REGISTER_ROUTE("/page2", renderPage2Route);
