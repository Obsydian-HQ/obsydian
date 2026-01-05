/**
 * Users list route (/users)
 */

#include <obsidian/obsidian.h>
#include <obsidian/route_registry_helper.h>

using namespace obsidian;

void renderUsersRoute(RouteContext& ctx) {
    Window& window = ctx.getWindow();
    
    VStack content;
    content.setSpacing(20.0);
    content.setPadding(Padding::all(20.0));
    
    TextView title;
    title.create("Users", 0, 0, 0, 0);
    title.setFontSize(32.0);
    title.setFontWeight(FontWeight::Bold);
    content.addChild(title);
    
    TextView description;
    description.create("User List - Click on a user to view profile", 0, 0, 0, 0);
    description.setFontSize(14.0);
    content.addChild(description);
    
    // Example user links
    VStack userList;
    userList.setSpacing(10.0);
    
    Link user1;
    user1.create("/users/alice", "Alice", 0, 0, 150, 30);
    userList.addChild(user1);
    
    Link user2;
    user2.create("/users/bob", "Bob", 0, 0, 150, 30);
    userList.addChild(user2);
    
    Link user3;
    user3.create("/users/charlie", "Charlie", 0, 0, 150, 30);
    userList.addChild(user3);
    
    content.addChild(userList);
    
    window.setContent(content);
}

REGISTER_ROUTE("/users", renderUsersRoute);
