/**
 * Blog layout - wraps /blog routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderBlogLayout(RouteContext& ctx, std::function<void()> renderChild) {
    Window& window = ctx.getWindow();
    
    // Use a single VStack for the layout (can't nest VStack/HStack)
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(20.0));
    
    // Sidebar navigation links
    Link allPostsLink;
    allPostsLink.create("/blog", "All Posts", 0, 0, 120, 30);
    layout.addChild(allPostsLink);
    
    Link post1Link;
    post1Link.create("/blog/post-1", "Post 1", 0, 0, 120, 30);
    layout.addChild(post1Link);
    
    Link post2Link;
    post2Link.create("/blog/post-2", "Post 2", 0, 0, 120, 30);
    layout.addChild(post2Link);
    
    // Add layout to window
    layout.addToWindow(window);
    
    // Render child route content
    renderChild();
}

// Register layout using function call (macro doesn't exist)
static bool _registered_blog_layout = []() {
    if (g_router) {
        registerLayout("/blog", renderBlogLayout);
    }
    return true;
}();
