/**
 * Dynamic product route - maps to "/products/:id"
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>
#include <sstream>

using namespace obsidian;

void renderProductRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    // Get route parameters
    const auto& params = ctx.getParams();
    const auto& query = ctx.getQuery();
    
    // Extract product ID from params
    std::string productId = "Unknown";
    if (params.find("id") != params.end()) {
        productId = params.at("id");
    }
    
    TextView title;
    std::string titleText = "Product #" + productId;
    title.create(titleText.c_str(), 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    std::string descText = "This is a dynamic route. The product ID '" + productId + "' was extracted from the route path.";
    description.create(descText.c_str(), 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Show query parameters if any
    if (!query.empty()) {
        TextView queryTitle;
        queryTitle.create("Query Parameters:", 0, 0, 0, 0);
        queryTitle.setFontSize(16.0);
        queryTitle.setFontWeight(FontWeight::Bold);
        content.addChild(queryTitle);
        
        for (const auto& [key, value] : query) {
            TextView queryParam;
            std::string paramText = key + " = " + value;
            queryParam.create(paramText.c_str(), 0, 0, 0, 0);
            queryParam.setFontSize(12.0);
            content.addChild(queryParam);
        }
    }
    
    // Back link
    Link backLink;
    backLink.create("/products", "Back to Products", 0, 0, 150, 30);
    content.addChild(backLink);
    
    window.setContent(content);
}

REGISTER_ROUTE("/products/:id", renderProductRoute);
