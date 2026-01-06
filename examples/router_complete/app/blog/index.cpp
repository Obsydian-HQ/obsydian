/**
 * Blog list route (/blog)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderBlogRoute(RouteContext& ctx) {
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Blog Posts", 0, 0, 0, 0);
    title.setFontSize(28.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("All blog posts:", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    VStack postList;
    postList.setSpacing(10.0);
    
    Button post1Button;
    post1Button.create("Post 1: Getting Started", 0, 0, 200, 30);
    Link post1;
    post1.create("/blog/post-1", post1Button);
    postList.addChild(post1);
    
    Button post2Button;
    post2Button.create("Post 2: Advanced Topics", 0, 0, 200, 30);
    Link post2;
    post2.create("/blog/post-2", post2Button);
    postList.addChild(post2);
    
    content.addChild(postList);
    
    ctx.setContent(content);
}

REGISTER_ROUTE("/blog", renderBlogRoute);
