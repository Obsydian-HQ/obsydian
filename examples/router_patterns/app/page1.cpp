/**
 * Page 1 route (/page1)
 * Demonstrates replace() pattern
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderPage1Route(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Page 1", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This page demonstrates navigation with history. Use navigate() to add routes to history.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Navigate to Page 2 button
    Button toPage2Btn;
    toPage2Btn.create("Go to Page 2", 0, 0, 150, 30);
    toPage2Btn.setOnClick([&router = ctx.getRouter()]() {
        router.navigate("/page2");
    });
    content.addChild(toPage2Btn);
    
    // Navigation buttons
    HStack navButtons;
    navButtons.setSpacing(10.0);
    
    Button backBtn;
    backBtn.create("Back (to Home)", 0, 0, 150, 30);
    backBtn.setOnClick([&router = ctx.getRouter()]() {
        if (router.canGoBack()) {
            router.goBack();
        }
    });
    navButtons.addChild(backBtn);
    
    Button forwardBtn;
    forwardBtn.create("Forward (to Page 2)", 0, 0, 180, 30);
    forwardBtn.setOnClick([&router = ctx.getRouter()]() {
        if (router.canGoForward()) {
            router.goForward();
        }
    });
    navButtons.addChild(forwardBtn);
    
    content.addChild(navButtons);
    
    // History info
    TextView historyInfo;
    std::string info = "Can go back: " + std::string(ctx.canGoBack() ? "Yes (to Home)" : "No") + 
                       " | Can go forward: " + std::string(ctx.canGoForward() ? "Yes (to Page 2)" : "No");
    historyInfo.create(info.c_str(), 0, 0, 0, 0);
    historyInfo.setFontSize(12.0);
    content.addChild(historyInfo);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/page1", renderPage1Route);
