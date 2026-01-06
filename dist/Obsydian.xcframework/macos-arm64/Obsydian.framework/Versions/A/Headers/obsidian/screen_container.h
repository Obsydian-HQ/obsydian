/**
 * ScreenContainer - Route view lifecycle manager
 * 
 * The ScreenContainer owns and manages screen views for routing.
 * Based on react-native-screens' architecture.
 * 
 * Key concepts:
 * - Container OWNS all screens (native views persist beyond C++ scope)
 * - Screens are created per-route and reused
 * - Only one screen is active/visible at a time
 * - Container handles all view lifecycle automatically
 * 
 * Usage:
 * ```cpp
 * ScreenContainer container;
 * container.attachToWindow(window);
 * 
 * // When navigating to a route:
 * Screen* screen = container.getOrCreateScreen("/about");
 * screen->clear();  // Clear old content
 * // Render route content into screen...
 * container.setActiveScreen(screen);
 * ```
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

namespace obsidian {

// Forward declarations
class Window;
class Screen;

/**
 * Screen - A retained view that holds content for a route
 * 
 * Screens are owned by ScreenContainer. Content added to a screen
 * persists until explicitly cleared.
 */
class Screen {
public:
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;
    Screen(Screen&& other) noexcept;
    Screen& operator=(Screen&& other) noexcept;
    ~Screen();

    /**
     * Get the route path this screen represents
     */
    const std::string& getRoutePath() const;

    /**
     * Clear all content from this screen
     */
    void clear();

    /**
     * Get native handle (for adding content)
     */
    void* getNativeHandle() const;

    /**
     * Check if screen is currently active/visible
     */
    bool isActive() const;
    
    /**
     * Set content for this screen
     * The component must have an addToScreen(Screen&) method
     */
    template<typename T>
    void setContent(T& component) {
        component.addToScreen(*this);
    }

private:
    friend class ScreenContainer;
    Screen(void* handle, const std::string& routePath);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * ScreenContainer - Owns and manages route screens
 */
class ScreenContainer {
public:
    ScreenContainer();
    ~ScreenContainer();
    
    ScreenContainer(const ScreenContainer&) = delete;
    ScreenContainer& operator=(const ScreenContainer&) = delete;
    ScreenContainer(ScreenContainer&& other) noexcept;
    ScreenContainer& operator=(ScreenContainer&& other) noexcept;

    /**
     * Attach container to a window
     * Container's view becomes a child of window's content view
     */
    void attachToWindow(Window& window);

    /**
     * Get or create a screen for a route path
     * If screen exists, returns it. Otherwise creates new one.
     */
    Screen* getOrCreateScreen(const std::string& routePath);

    /**
     * Get existing screen (returns nullptr if not found)
     */
    Screen* getScreen(const std::string& routePath);

    /**
     * Set the active screen (makes it visible, hides others)
     */
    void setActiveScreen(Screen* screen);

    /**
     * Get currently active screen (nullptr if none)
     */
    Screen* getActiveScreen();

    /**
     * Remove a specific screen
     */
    void removeScreen(const std::string& routePath);

    /**
     * Clear all screens
     */
    void clearAll();

    /**
     * Get native container handle
     */
    void* getNativeHandle() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
