/**
 * Settings Page Route (/settings)
 */

#include <iostream>
#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void SettingsPage(RouteContext& ctx) {
    std::cout << "[SettingsPage] Rendering\n";
    
    VStack content;
    content.setSpacing(16.0);
    content.setAlignment(layout::Alignment::TopLeading);
    content.setPadding(Padding::all(32.0));
    
    TextView title;
    title.create("Settings", 0, 0, 400, 50);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create(
        "Application Settings\n\n"
        "Configure your application preferences here.\n\n"
        "(Settings controls would go here)",
        0, 0, 600, 150
    );
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Example buttons
    Button saveBtn;
    saveBtn.create("Save Settings", 0, 0, 150, 36);
    saveBtn.setOnClick([]() {
        std::cout << "Save Settings clicked\n";
    });
    content.addChild(saveBtn);
    
    Button resetBtn;
    resetBtn.create("Reset to Defaults", 0, 0, 150, 36);
    resetBtn.setOnClick([]() {
        std::cout << "Reset to Defaults clicked\n";
    });
    content.addChild(resetBtn);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/settings", SettingsPage);
