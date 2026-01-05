/**
 * Blog layout - wraps /blog routes
 * 
 * Uses content slot pattern for proper nesting
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderBlogLayout(RouteContext& ctx, std::function<void()> renderChild) {
    // Main layout container
    VStack layout;
    layout.setSpacing(20.0);
    layout.setPadding(Padding::all(20.0));
    
    // Blog navigation links
    HStack navLinks;
    navLinks.setSpacing(10.0);
    
    Link allPostsLink;
    allPostsLink.create("/blog", "All Posts", 0, 0, 120, 30);
    navLinks.addChild(allPostsLink);
    
    Link post1Link;
    post1Link.create("/blog/post-1", "Post 1", 0, 0, 120, 30);
    navLinks.addChild(post1Link);
    
    Link post2Link;
    post2Link.create("/blog/post-2", "Post 2", 0, 0, 120, 30);
    navLinks.addChild(post2Link);
    
    layout.addChild(navLinks);
    
    // Content slot for child routes
    VStack contentSlot;
    contentSlot.setSpacing(10.0);
    layout.addChild(contentSlot);
    
    // STEP 1: Add THIS layout to PARENT's content slot (via setContent)
    // setContent uses the parent's contentSlot_ which was set by the root layout
    ctx.setContent(layout);
    
    // STEP 2: Set OUR content slot for OUR children
    ctx.setContentSlot(contentSlot.getNativeViewHandle());
    
    // STEP 3: Render children - they go into OUR contentSlot
    renderChild();
}

REGISTER_LAYOUT("/blog", renderBlogLayout);
