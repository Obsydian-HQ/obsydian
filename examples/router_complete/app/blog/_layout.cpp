/**
 * Blog layout - wraps /blog/* routes
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderBlogLayout(RouteContext& ctx, std::function<void()> renderChild) {
    Window& window = ctx.getWindow();
    
    HStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(20.0));
    
    // Sidebar
    VStack sidebar;
    sidebar.setSpacing(10.0);
    sidebar.setPadding(Padding::all(15.0));
    
    TextView sidebarTitle;
    sidebarTitle.create("Blog", 0, 0, 0, 0);
    sidebarTitle.setFontSize(18.0);
    sidebarTitle.setFontWeight(FontWeight::Bold);
    sidebar.addChild(sidebarTitle);
    
    Link allPostsLink;
    allPostsLink.create("/blog", "All Posts", 0, 0, 120, 30);
    sidebar.addChild(allPostsLink);
    
    Link post1Link;
    post1Link.create("/blog/post-1", "Post 1", 0, 0, 120, 30);
    sidebar.addChild(post1Link);
    
    Link post2Link;
    post2Link.create("/blog/post-2", "Post 2", 0, 0, 120, 30);
    sidebar.addChild(post2Link);
    
    layout.addChild(sidebar);
    
    // Main content
    VStack content;
    content.setSpacing(15.0);
    content.setPadding(Padding::all(15.0));
    
    // Render child route content
    renderChild();
    
    layout.addChild(content);
    
    window.setContent(layout);
}

REGISTER_LAYOUT("/blog", renderBlogLayout);
