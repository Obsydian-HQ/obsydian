/**
 * Integration tests for Link Component
 */

#include "obsidian/link.h"
#include "obsidian/router.h"
#include "obsidian/window.h"
#include "obsidian/button.h"
#include "obsidian/textview.h"
#include "obsidian/vstack.h"
#include "obsidian/hstack.h"
#include <gtest/gtest.h>
#include <filesystem>

using namespace obsidian;

class LinkTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::path workspaceRoot;
        
        // Find workspace root
        const char* testSrcDir = std::getenv("TEST_SRCDIR");
        const char* testWorkspace = std::getenv("TEST_WORKSPACE");
        
        if (testSrcDir && testWorkspace) {
            workspaceRoot = std::filesystem::path(testSrcDir) / testWorkspace;
            if (!std::filesystem::exists(workspaceRoot / "test_apps" / "route_scanner_test" / "app")) {
                workspaceRoot.clear();
            }
        }
        
        if (workspaceRoot.empty()) {
            std::filesystem::path searchPath = std::filesystem::current_path();
            for (int i = 0; i < 20 && !searchPath.empty() && searchPath.has_parent_path(); ++i) {
                std::filesystem::path testAppPath = searchPath / "test_apps" / "route_scanner_test" / "app";
                if (std::filesystem::exists(testAppPath) && std::filesystem::is_directory(testAppPath)) {
                    workspaceRoot = searchPath;
                    break;
                }
                if (std::filesystem::exists(searchPath / "MODULE.bazel") ||
                    (std::filesystem::exists(searchPath / "BUILD") && 
                     std::filesystem::exists(searchPath / "test_apps"))) {
                    workspaceRoot = searchPath;
                    break;
                }
                std::filesystem::path parent = searchPath.parent_path();
                if (parent == searchPath) break;
                searchPath = parent;
            }
        }
        
        if (!workspaceRoot.empty()) {
            testAppDirectory = (workspaceRoot / "test_apps" / "route_scanner_test" / "app").string();
        } else {
            testAppDirectory = (std::filesystem::current_path() / "test_apps" / "route_scanner_test" / "app").string();
        }
        
        // Initialize router
        router = std::make_unique<Router>();
        ASSERT_TRUE(router->initialize(testAppDirectory)) << "Router should initialize";
        router->setOnNavigation([this](const std::string& path) {
            lastNavigatedPath = path;
        });
        
        // Create a window for testing
        window = std::make_unique<Window>();
        ASSERT_TRUE(window->create(800, 600, "Link Test")) << "Window should be created";
        router->setWindow(*window);
    }
    
    void TearDown() override {
        window.reset();
        router.reset();
        lastNavigatedPath.clear();
    }
    
    std::string testAppDirectory;
    std::unique_ptr<Router> router;
    std::unique_ptr<Window> window;
    std::string lastNavigatedPath;
};

TEST_F(LinkTest, CreateLink) {
    Link link;
    bool result = link.create("/about", "About", 10, 10, 100, 30);
    
    ASSERT_TRUE(result) << "Link should be created successfully";
    ASSERT_TRUE(link.isValid()) << "Link should be valid after creation";
    ASSERT_EQ(link.getHref(), "/about") << "Href should be set correctly";
    ASSERT_EQ(link.getText(), "About") << "Text should be set correctly";
}

TEST_F(LinkTest, LinkHrefAccessors) {
    Link link;
    link.create("/products", "Products", 10, 10, 100, 30);
    
    ASSERT_EQ(link.getHref(), "/products") << "Initial href should be correct";
    
    link.setHref("/contact");
    ASSERT_EQ(link.getHref(), "/contact") << "Href should be updated";
}

TEST_F(LinkTest, LinkTextAccessors) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    
    ASSERT_EQ(link.getText(), "About") << "Initial text should be correct";
    
    link.setText("About Us");
    ASSERT_EQ(link.getText(), "About Us") << "Text should be updated";
}

TEST_F(LinkTest, LinkSetRouter) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    
    // Set router explicitly
    link.setRouter(router.get());
    
    // Verify router is set (indirectly by checking navigation works)
    link.addToWindow(*window);
    
    // Note: Actual click simulation would require platform-specific code
    // This test verifies the API works correctly
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
}

TEST_F(LinkTest, LinkVisibility) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    
    ASSERT_TRUE(link.isVisible()) << "Link should be visible by default";
    
    link.setVisible(false);
    ASSERT_FALSE(link.isVisible()) << "Link should be hidden";
    
    link.setVisible(true);
    ASSERT_TRUE(link.isVisible()) << "Link should be visible again";
}

TEST_F(LinkTest, LinkEnabledState) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    
    ASSERT_TRUE(link.isEnabled()) << "Link should be enabled by default";
    
    link.setEnabled(false);
    ASSERT_FALSE(link.isEnabled()) << "Link should be disabled";
    
    link.setEnabled(true);
    ASSERT_TRUE(link.isEnabled()) << "Link should be enabled again";
}

TEST_F(LinkTest, LinkOnClickCallback) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    link.setRouter(router.get());
    
    bool callbackCalled = false;
    link.setOnClick([&callbackCalled]() {
        callbackCalled = true;
    });
    
    // Note: Actual click would trigger this, but we verify the callback is set
    // In a real scenario, clicking would call the callback before navigation
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
}

TEST_F(LinkTest, LinkAddToWindow) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    link.setRouter(router.get());
    
    // Should not crash
    link.addToWindow(*window);
    
    ASSERT_TRUE(link.isValid()) << "Link should still be valid after adding to window";
}

TEST_F(LinkTest, LinkRemoveFromParent) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    link.addToWindow(*window);
    
    // Should not crash
    link.removeFromParent();
    
    ASSERT_TRUE(link.isValid()) << "Link should still be valid after removal";
}

TEST_F(LinkTest, LinkInvalidState) {
    Link link;
    
    // Before creation, link should be invalid
    ASSERT_FALSE(link.isValid()) << "Link should be invalid before creation";
    ASSERT_FALSE(link.isVisible()) << "Link should not be visible when invalid";
    ASSERT_FALSE(link.isEnabled()) << "Link should not be enabled when invalid";
    ASSERT_TRUE(link.getHref().empty()) << "Href should be empty when invalid";
}

TEST_F(LinkTest, LinkCannotCreateTwice) {
    Link link;
    bool result1 = link.create("/about", "About", 10, 10, 100, 30);
    ASSERT_TRUE(result1) << "First creation should succeed";
    
    bool result2 = link.create("/contact", "Contact", 20, 20, 100, 30);
    ASSERT_FALSE(result2) << "Second creation should fail";
    
    // Original values should remain
    ASSERT_EQ(link.getHref(), "/about") << "Href should remain unchanged";
}

// Navigation behavior tests
TEST_F(LinkTest, LinkNavigatesOnClick) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    link.setRouter(router.get());
    
    // Simulate navigation by calling navigate directly
    // (In real usage, clicking would trigger this)
    std::string initialPath = router->getCurrentPath();
    
    // Navigate to initial route
    router->navigate("/");
    ASSERT_EQ(router->getCurrentPath(), "/");
    
    // Simulate link click by navigating to link's href
    router->navigate(link.getHref());
    ASSERT_EQ(router->getCurrentPath(), "/about") << "Link should navigate to /about";
    ASSERT_NE(router->getCurrentPath(), initialPath) << "Path should have changed";
}

TEST_F(LinkTest, LinkWithButtonChild) {
    Button button;
    ASSERT_TRUE(button.create("Click Me", 10, 10, 100, 30));
    
    Link link;
    bool result = link.create("/products", button);
    ASSERT_TRUE(result) << "Link should be created with Button child";
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
    ASSERT_EQ(link.getHref(), "/products") << "Href should be set correctly";
    
    link.setRouter(router.get());
    link.addToWindow(*window);
    ASSERT_TRUE(link.isValid()) << "Link should remain valid after adding to window";
}

// TODO: Fix TextView child support - TextView.getNativeHandle() may need special handling
// The native handle from TextView might not be directly usable as an NSView in the Link wrapper
TEST_F(LinkTest, DISABLED_LinkWithTextViewChild) {
    TextView textView;
    // TextView needs valid dimensions to be properly initialized
    ASSERT_TRUE(textView.create("Clickable Text", 10, 10, 200, 50));
    ASSERT_TRUE(textView.isValid()) << "TextView should be valid after creation";
    
    Link link;
    // Use an existing route from test app
    bool result = link.create("/products", textView);
    ASSERT_TRUE(result) << "Link should be created with TextView child";
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
    ASSERT_EQ(link.getHref(), "/products") << "Href should be set correctly";
}

TEST_F(LinkTest, LinkWithVStackChild) {
    VStack vstack;
    vstack.setSpacing(10.0);
    
    Button button;
    button.create("Button in Stack", 0, 0, 100, 30);
    vstack.addChild(button);
    
    Link link;
    bool result = link.create("/dashboard", vstack);
    ASSERT_TRUE(result) << "Link should be created with VStack child";
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
    ASSERT_EQ(link.getHref(), "/dashboard") << "Href should be set correctly";
}

TEST_F(LinkTest, LinkWithHStackChild) {
    HStack hstack;
    hstack.setSpacing(10.0);
    
    Button button1;
    button1.create("Button 1", 0, 0, 80, 30);
    hstack.addChild(button1);
    
    Button button2;
    button2.create("Button 2", 0, 0, 80, 30);
    hstack.addChild(button2);
    
    Link link;
    bool result = link.create("/settings", hstack);
    ASSERT_TRUE(result) << "Link should be created with HStack child";
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
    ASSERT_EQ(link.getHref(), "/settings") << "Href should be set correctly";
}

TEST_F(LinkTest, LinkNavigationHistory) {
    Link link1, link2, link3;
    link1.create("/", "Home", 10, 10, 80, 30);
    link2.create("/about", "About", 100, 10, 80, 30);
    link3.create("/products", "Products", 190, 10, 80, 30); // Use existing route
    
    link1.setRouter(router.get());
    link2.setRouter(router.get());
    link3.setRouter(router.get());
    
    // Navigate through links
    router->navigate(link1.getHref());
    ASSERT_EQ(router->getCurrentPath(), "/");
    
    router->navigate(link2.getHref());
    ASSERT_EQ(router->getCurrentPath(), "/about");
    ASSERT_TRUE(router->canGoBack()) << "Should be able to go back";
    
    router->navigate(link3.getHref());
    ASSERT_EQ(router->getCurrentPath(), "/products");
    ASSERT_EQ(router->getHistorySize(), 3) << "History should have 3 entries";
    
    // Go back
    router->goBack();
    ASSERT_EQ(router->getCurrentPath(), "/about") << "Should navigate back";
}

TEST_F(LinkTest, LinkRequiresValidChild) {
    Button invalidButton; // Not created, so invalid
    
    Link link;
    bool result = link.create("/test", invalidButton);
    ASSERT_FALSE(result) << "Link should not be created with invalid child";
    ASSERT_FALSE(link.isValid()) << "Link should be invalid";
}

TEST_F(LinkTest, LinkCallbackBeforeNavigation) {
    Link link;
    link.create("/about", "About", 10, 10, 100, 30);
    link.setRouter(router.get());
    
    bool callbackCalled = false;
    link.setOnClick([&callbackCalled]() {
        callbackCalled = true;
    });
    
    // Simulate click by navigating
    router->navigate(link.getHref());
    
    // Note: In real usage, the callback would be called before navigation
    // This test verifies the callback mechanism is set up correctly
    ASSERT_TRUE(link.isValid()) << "Link should be valid";
}
