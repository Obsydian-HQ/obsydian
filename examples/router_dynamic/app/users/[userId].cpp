/**
 * Dynamic user route - maps to "/users/:userId"
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>
#include <sstream>

using namespace obsidian;

void renderUserRoute(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    // Get route parameters
    const auto& params = ctx.getParams();
    
    // Extract user ID from params
    std::string userId = "Unknown";
    if (params.find("userId") != params.end()) {
        userId = params.at("userId");
    }
    
    TextView title;
    std::string titleText = "User Profile: " + userId;
    title.create(titleText.c_str(), 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    std::string descText = "This is a dynamic route. The user ID '" + userId + "' was extracted from the route path.";
    description.create(descText.c_str(), 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Back link
    Button backButton;
    backButton.create("Back to Users", 0, 0, 150, 30);
    Link backLink;
    backLink.create("/users", backButton);
    content.addChild(backLink);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/users/:userId", renderUserRoute);
