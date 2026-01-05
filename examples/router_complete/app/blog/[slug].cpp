/**
 * Dynamic blog post route - maps to "/blog/:slug"
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderBlogPostRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    // Get route parameters
    const auto& params = ctx.getParams();
    
    // Extract slug from params
    std::string slug = "unknown";
    if (params.find("slug") != params.end()) {
        slug = params.at("slug");
    }
    
    TextView title;
    std::string titleText = "Blog Post: " + slug;
    title.create(titleText, 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    std::string descText = "This is a dynamic blog post route. The slug '" + slug + "' was extracted from the route path.";
    description.create(descText, 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    TextView contentText;
    contentText.create("This demonstrates how dynamic routes work in a real-world application. The route /blog/:slug matches any slug value.", 0, 0, 0, 0);
    contentText.setFontSize(12.0);
    content.addChild(contentText);
    
    // Back link
    Link backLink;
    backLink.create("/blog", "Back to Blog", 0, 0, 150, 30);
    content.addChild(backLink);
    
    window.setContent(content);
}

REGISTER_ROUTE("/blog/:slug", renderBlogPostRoute);
