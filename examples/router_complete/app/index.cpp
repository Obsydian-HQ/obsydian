/**
 * Home route (/)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderHomeRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Welcome", 0, 0, 0, 0);
    title.setFontSize(36.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is a complete example application demonstrating all routing features:", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    VStack features;
    features.setSpacing(8.0);
    
    TextView feature1;
    feature1.create("✓ File-based routing", 0, 0, 0, 0);
    feature1.setFontSize(12.0);
    features.addChild(feature1);
    
    TextView feature2;
    feature2.create("✓ Nested routes and layouts", 0, 0, 0, 0);
    feature2.setFontSize(12.0);
    features.addChild(feature2);
    
    TextView feature3;
    feature3.create("✓ Dynamic route segments", 0, 0, 0, 0);
    feature3.setFontSize(12.0);
    features.addChild(feature3);
    
    TextView feature4;
    feature4.create("✓ Navigation patterns", 0, 0, 0, 0);
    feature4.setFontSize(12.0);
    features.addChild(feature4);
    
    content.addChild(features);
    
    window.setContent(content);
}

REGISTER_ROUTE("/", renderHomeRoute);
