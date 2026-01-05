/**
 * Page 1 route (/page1)
 * Demonstrates replace() pattern
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderPage1Route(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Page 1", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This page demonstrates the replace() pattern. Use replace() to change the current route without adding to history.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Replace button
    Button replaceBtn;
    replaceBtn.create("Replace with Page 2", 0, 0, 180, 30);
    replaceBtn.setOnClick([&ctx]() {
        ctx.navigate("/page2");
    });
    content.addChild(replaceBtn);
    
    TextView note;
    note.create("Note: After replace, you won't be able to go back to Page 1", 0, 0, 0, 0);
    note.setFontSize(12.0);
    content.addChild(note);
    
    window.setContent(content);
}

REGISTER_ROUTE("/page1", renderPage1Route);
