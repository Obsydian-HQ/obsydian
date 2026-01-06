/**
 * Page 2 route (/page2)
 * Demonstrates push() pattern
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderPage2Route(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Page 2", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This page is in the middle of the navigation stack. You can go back to Page 1 or forward to Page 3.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Navigate to Page 3 button
    Button toPage3Btn;
    toPage3Btn.create("Go to Page 3", 0, 0, 150, 30);
    toPage3Btn.setOnClick([&router = ctx.getRouter()]() {
        router.navigate("/page3");
    });
    content.addChild(toPage3Btn);
    
    // Navigation buttons
    HStack navButtons;
    navButtons.setSpacing(10.0);
    
    Button backBtn;
    backBtn.create("Back (to Page 1)", 0, 0, 170, 30);
    backBtn.setOnClick([&router = ctx.getRouter()]() {
        if (router.canGoBack()) {
            router.goBack();
        }
    });
    navButtons.addChild(backBtn);
    
    Button forwardBtn;
    forwardBtn.create("Forward (to Page 3)", 0, 0, 180, 30);
    forwardBtn.setOnClick([&router = ctx.getRouter()]() {
        if (router.canGoForward()) {
            router.goForward();
        }
    });
    navButtons.addChild(forwardBtn);
    
    content.addChild(navButtons);
    
    // History info
    TextView historyInfo;
    std::string info = "Can go back: " + std::string(ctx.canGoBack() ? "Yes (to Page 1)" : "No") + 
                       " | Can go forward: " + std::string(ctx.canGoForward() ? "Yes (to Page 3)" : "No");
    historyInfo.create(info.c_str(), 0, 0, 0, 0);
    historyInfo.setFontSize(12.0);
    content.addChild(historyInfo);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/page2", renderPage2Route);
