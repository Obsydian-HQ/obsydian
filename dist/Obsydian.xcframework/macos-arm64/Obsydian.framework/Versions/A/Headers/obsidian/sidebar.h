/**
 * Obsidian Public API - Sidebar
 * 
 * Sidebar provides a native macOS sidebar using NSSplitViewController and
 * NSSplitViewItem.sidebar() with material background, native collapse button,
 * and full-height sidebar integration.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include "obsidian/window.h"

namespace obsidian {

// Forward declarations
class VStack;
class HStack;
class List;
class Button;
class ScrollView;

/**
 * Sidebar class - Native macOS sidebar
 * 
 * Provides a native macOS sidebar using NSSplitViewController and
 * NSSplitViewItem.sidebar() with:
 * - Material background (native macOS vibrancy)
 * - Native collapse/expand button
 * - Full-height sidebar integration
 * - Automatic window control integration
 */
class Sidebar {
public:
    /**
     * Create a new Sidebar
     */
    Sidebar();
    
    /**
     * Destructor - automatically removes and destroys the Sidebar
     */
    ~Sidebar();
    
    /**
     * Create the sidebar
     * @return true if creation was successful, false otherwise
     */
    bool create();
    
    /**
     * Set the sidebar content
     * The content will be wrapped in an NSViewController and added as a sidebar item
     */
    void setSidebarContent(VStack& vstack);
    void setSidebarContent(HStack& hstack);
    void setSidebarContent(List& list);
    void setSidebarContent(Button& button);
    void setSidebarContent(ScrollView& scrollView);
    
    /**
     * Set the main content (content pane)
     * The content will be wrapped in an NSViewController and added as a content list item
     */
    void setMainContent(VStack& vstack);
    void setMainContent(HStack& hstack);
    void setMainContent(List& list);
    void setMainContent(Button& button);
    void setMainContent(ScrollView& scrollView);
    
    /**
     * Set minimum sidebar width (in points)
     * @param width Minimum width in points (default: 150.0)
     */
    void setMinimumSidebarWidth(double width);
    
    /**
     * Get minimum sidebar width (in points)
     */
    double getMinimumSidebarWidth() const;
    
    /**
     * Set maximum sidebar width (in points)
     * @param width Maximum width in points (default: 400.0)
     */
    void setMaximumSidebarWidth(double width);
    
    /**
     * Get maximum sidebar width (in points)
     */
    double getMaximumSidebarWidth() const;
    
    /**
     * Collapse the sidebar
     */
    void collapseSidebar();
    
    /**
     * Expand the sidebar
     */
    void expandSidebar();
    
    /**
     * Toggle sidebar collapse state
     */
    void toggleSidebar();
    
    /**
     * Check if sidebar is collapsed
     * @return true if collapsed, false if expanded
     */
    bool isSidebarCollapsed() const;
    
    /**
     * Add the sidebar to a window
     * The split view controller's view will be set as the window's content view
     * @param window Window to add the sidebar to
     */
    void addToWindow(Window& window);
    
    /**
     * Remove the sidebar from its parent
     */
    void removeFromParent();
    
    /**
     * Check if sidebar is valid
     * @return true if sidebar is valid and ready to use
     */
    bool isValid() const;
    
    /**
     * Set callback for sidebar toggle events
     * @param callback Function called when sidebar is toggled (collapsed state passed as parameter)
     */
    void setOnSidebarToggle(std::function<void(bool collapsed)> callback);
    
    // Non-copyable
    Sidebar(const Sidebar&) = delete;
    Sidebar& operator=(const Sidebar&) = delete;
    
    // Movable
    Sidebar(Sidebar&&) noexcept;
    Sidebar& operator=(Sidebar&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
