/**
 * Home route (/)
 * Demonstrates navigation patterns
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
    title.create("Navigation Patterns", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This example demonstrates all navigation patterns:", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    VStack patterns;
    patterns.setSpacing(10.0);
    
    TextView pattern1;
    pattern1.create("• navigate() - Standard navigation (adds to history)", 0, 0, 0, 0);
    pattern1.setFontSize(12.0);
    patterns.addChild(pattern1);
    
    TextView pattern2;
    pattern2.create("• push() - Push onto history stack", 0, 0, 0, 0);
    pattern2.setFontSize(12.0);
    patterns.addChild(pattern2);
    
    TextView pattern3;
    pattern3.create("• replace() - Replace current history entry", 0, 0, 0, 0);
    pattern3.setFontSize(12.0);
    patterns.addChild(pattern3);
    
    TextView pattern4;
    pattern4.create("• goBack() - Navigate back in history", 0, 0, 0, 0);
    pattern4.setFontSize(12.0);
    patterns.addChild(pattern4);
    
    TextView pattern5;
    pattern5.create("• goForward() - Navigate forward in history", 0, 0, 0, 0);
    pattern5.setFontSize(12.0);
    patterns.addChild(pattern5);
    
    content.addChild(patterns);
    
    // Navigation buttons
    HStack navButtons;
    navButtons.setSpacing(10.0);
    
    Button backBtn;
    backBtn.create("Back", 0, 0, 100, 30);
    backBtn.setOnClick([&ctx]() {
        if (ctx.canGoBack()) {
            ctx.goBack();
        }
    });
    navButtons.addChild(backBtn);
    
    Button forwardBtn;
    forwardBtn.create("Forward", 0, 0, 100, 30);
    forwardBtn.setOnClick([&ctx]() {
        if (ctx.canGoForward()) {
            ctx.goForward();
        }
    });
    navButtons.addChild(forwardBtn);
    
    content.addChild(navButtons);
    
    // History info
    TextView historyInfo;
    std::string info = "Can go back: " + std::string(ctx.canGoBack() ? "Yes" : "No") + 
                       " | Can go forward: " + std::string(ctx.canGoForward() ? "Yes" : "No");
    historyInfo.create(info.c_str(), 0, 0, 0, 0);
    historyInfo.setFontSize(12.0);
    content.addChild(historyInfo);
    
    window.setContent(content);
}

REGISTER_ROUTE("/", renderHomeRoute);
