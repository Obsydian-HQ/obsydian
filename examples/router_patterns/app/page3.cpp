/**
 * Page 3 route (/page3)
 * Demonstrates back/forward navigation
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderPage3Route(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Page 3", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("This page demonstrates back/forward navigation. Use the buttons to navigate through history.", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Navigation buttons
    HStack navButtons;
    navButtons.setSpacing(10.0);
    
    Button backBtn;
    backBtn.create("Go Back", 0, 0, 100, 30);
    backBtn.setOnClick([&ctx]() {
        if (ctx.canGoBack()) {
            ctx.goBack();
        }
    });
    navButtons.addChild(backBtn);
    
    Button forwardBtn;
    forwardBtn.create("Go Forward", 0, 0, 100, 30);
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
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/page3", renderPage3Route);
