/**
 * Sidebar with Router Example Application
 * 
 * This is the CANONICAL example showing how to properly integrate
 * Sidebar with Router in Obsidian. This pattern should be used by
 * all real-world applications that need sidebar navigation.
 * 
 * Key architecture:
 * - Sidebar owns the window's content view (via NSSplitViewController)
 * - Router's ScreenContainer is attached to Sidebar's main content area
 * - Navigation buttons in sidebar call router.navigate()
 * - Route components render inside the sidebar's main area
 * 
 * This matches React Navigation's drawer pattern where:
 * - DrawerNavigator wraps the content
 * - Navigation state is managed by the router
 * - Screen content renders in the drawer's main area
 */

#include <iostream>
#include <filesystem>
#include <obsidian/obsidian.h>

using namespace obsidian;

// Helper to find app directory - works when run via bazel or directly
std::string findAppDirectory(const std::string& appDirName) {
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path testPath = currentDir / appDirName;
    if (std::filesystem::exists(testPath) && std::filesystem::is_directory(testPath)) {
        return testPath.string();
    }
    
    // Try from workspace root (where bazel runs from)
    std::filesystem::path searchPath = currentDir;
    for (int i = 0; i < 10 && !searchPath.empty() && searchPath.has_parent_path(); ++i) {
        if (std::filesystem::exists(searchPath / "MODULE.bazel") ||
            (std::filesystem::exists(searchPath / "BUILD") && 
             std::filesystem::exists(searchPath / "examples"))) {
            std::filesystem::path appPath = searchPath / "examples" / "sidebar_with_router" / appDirName;
            if (std::filesystem::exists(appPath) && std::filesystem::is_directory(appPath)) {
                return appPath.string();
            }
            break;
        }
        searchPath = searchPath.parent_path();
    }
    
    return appDirName;
}

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "=== Sidebar with Router Example ===\n\n";
    
    // Create and initialize the application
    App app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application\n";
        return 1;
    }
    
    // Create a window
    Window window;
    if (!window.create(1000, 700, "Sidebar + Router Example")) {
        std::cerr << "Failed to create window\n";
        app.shutdown();
        return 1;
    }
    
    // Set up toolbar for sidebar collapse button
    window.setupToolbarForSidebar();
    
    // =========================================================================
    // STEP 1: Initialize Router FIRST (but don't attach to window yet)
    // =========================================================================
    std::string appDir = findAppDirectory("app");
    std::cout << "Using app directory: " << appDir << std::endl;
    
    Router router;
    if (!router.initialize(appDir)) {
        std::cerr << "Failed to initialize router\n";
        app.shutdown();
        return 1;
    }
    std::cout << "Router initialized successfully\n";
    
    // =========================================================================
    // STEP 2: Create Sidebar
    // =========================================================================
    Sidebar sidebar;
    if (!sidebar.create()) {
        std::cerr << "Failed to create sidebar\n";
        app.shutdown();
        return 1;
    }
    
    sidebar.setMinimumSidebarWidth(200.0);
    sidebar.setMaximumSidebarWidth(300.0);
    
    // =========================================================================
    // STEP 3: Create sidebar navigation using VStack with Buttons
    // Each button navigates to a different route
    // =========================================================================
    VStack sidebarNav;
    sidebarNav.setSpacing(8.0);
    sidebarNav.setAlignment(layout::Alignment::TopLeading);
    sidebarNav.setPadding(Padding::all(16.0));
    
    // App title in sidebar
    TextView titleText;
    titleText.create("Navigation", 0, 0, 180, 30);
    titleText.setFontSize(18.0);
    titleText.setFontWeight(FontWeight::Bold);
    sidebarNav.addChild(titleText);
    
    // Navigation buttons - each calls router.navigate()
    Button homeBtn;
    homeBtn.create("🏠 Home", 0, 0, 180, 36);
    homeBtn.setOnClick([&router]() {
        std::cout << "Navigating to Home\n";
        router.navigate("/");
    });
    sidebarNav.addChild(homeBtn);
    
    Button aboutBtn;
    aboutBtn.create("ℹ️ About", 0, 0, 180, 36);
    aboutBtn.setOnClick([&router]() {
        std::cout << "Navigating to About\n";
        router.navigate("/about");
    });
    sidebarNav.addChild(aboutBtn);
    
    Button settingsBtn;
    settingsBtn.create("⚙️ Settings", 0, 0, 180, 36);
    settingsBtn.setOnClick([&router]() {
        std::cout << "Navigating to Settings\n";
        router.navigate("/settings");
    });
    sidebarNav.addChild(settingsBtn);
    
    
    // Set sidebar content
    sidebar.setSidebarContent(sidebarNav);
    
    // =========================================================================
    // STEP 4: CRITICAL - Set Router as main content
    // This is the KEY integration point. The Router's ScreenContainer
    // will be attached to the Sidebar's main content area.
    // =========================================================================
    sidebar.setMainContent(router);
    
    // =========================================================================
    // STEP 5: Add sidebar to window (this triggers layout and attaches everything)
    // =========================================================================
    sidebar.addToWindow(window);
    
    // =========================================================================
    // STEP 6: Now navigate to initial route
    // =========================================================================
    router.setOnNavigation([](const std::string& path) {
        std::cout << "Navigated to: " << path << std::endl;
    });
    
    router.navigate("/");
    
    // Show window and run
    window.show();
    
    std::cout << "\n";
    std::cout << "Architecture:\n";
    std::cout << "  - Sidebar owns window's content view\n";
    std::cout << "  - Router's ScreenContainer is in sidebar's main area\n";
    std::cout << "  - Navigation buttons call router.navigate()\n";
    std::cout << "\n";
    std::cout << "Click sidebar buttons to navigate between routes.\n";
    std::cout << "Close the window to exit.\n\n";
    
    // Set up application callbacks
    AppCallbacks callbacks;
    callbacks.onInit = []() {};
    callbacks.onUpdate = []() {};
    callbacks.onShutdown = []() {
        std::cout << "Application shutting down\n";
    };
    
    // Run the application event loop
    app.run(callbacks);
    
    window.close();
    app.shutdown();
    
    return 0;
}
