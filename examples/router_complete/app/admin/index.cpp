/**
 * Admin route (/admin)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderAdminRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Admin Panel", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This is the admin panel. In a real application, this would require authentication.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    VStack adminFeatures;
    adminFeatures.setSpacing(10.0);
    
    TextView feature1;
    feature1.create("• User Management", 0, 0, 0, 0);
    feature1.setFontSize(12.0);
    adminFeatures.addChild(feature1);
    
    TextView feature2;
    feature2.create("• Content Management", 0, 0, 0, 0);
    feature2.setFontSize(12.0);
    adminFeatures.addChild(feature2);
    
    TextView feature3;
    feature3.create("• Settings", 0, 0, 0, 0);
    feature3.setFontSize(12.0);
    adminFeatures.addChild(feature3);
    
    content.addChild(adminFeatures);
    
    window.setContent(content);
}

REGISTER_ROUTE("/admin", renderAdminRoute);
