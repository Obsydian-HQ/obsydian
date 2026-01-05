/**
 * Obsidian Public API - SplitView
 * 
 * SplitView provides a native macOS sidebar layout using NSSplitView with
 * collapse/expand, resizing, and content customization capabilities.
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
 * Sidebar position enumeration
 */
enum class SidebarPosition {
    Leading,   // Left side (LTR)
    Trailing   // Right side (LTR)
};

/**
 * SplitView class - Sidebar layout container
 * 
 * Provides a native macOS sidebar using NSSplitView with collapse/expand,
 * resizing, and content customization capabilities.
 */
class SplitView {
public:
    /**
     * Create a new SplitView
     */
    SplitView();
    
    /**
     * Destructor - automatically removes and destroys the SplitView
     */
    ~SplitView();
    
    /**
     * Creation and configuration
     */
    
    /**
     * Create the SplitView with the given sidebar position
     * @param position Sidebar position (Leading or Trailing)
     * @return true if SplitView was created successfully
     */
    bool create(SidebarPosition position = SidebarPosition::Leading);
    
    /**
     * Sidebar width management
     */
    
    /**
     * Set the sidebar width (in points)
     * The width will be clamped to min/max values
     */
    void setSidebarWidth(double width);
    
    /**
     * Get the current sidebar width (in points)
     */
    double getSidebarWidth() const;
    
    /**
     * Set minimum sidebar width (in points, default: 150.0)
     */
    void setMinimumSidebarWidth(double width);
    
    /**
     * Set maximum sidebar width (in points, default: 400.0)
     */
    void setMaximumSidebarWidth(double width);
    
    /**
     * Collapse/expand management
     */
    
    /**
     * Collapse the sidebar pane
     */
    void collapseSidebar();
    
    /**
     * Expand the sidebar pane (restore from collapsed state)
     */
    void expandSidebar();
    
    /**
     * Toggle sidebar collapse/expand state
     */
    void toggleSidebar();
    
    /**
     * Check if sidebar is currently collapsed
     */
    bool isSidebarCollapsed() const;
    
    /**
     * Content management
     */
    
    /**
     * Set the sidebar content view
     */
    void setSidebarContent(VStack& vstack);
    void setSidebarContent(HStack& hstack);
    void setSidebarContent(List& list);
    void setSidebarContent(Button& button);
    void setSidebarContent(ScrollView& scrollView);
    
    /**
     * Set the main content view
     */
    void setMainContent(VStack& vstack);
    void setMainContent(HStack& hstack);
    void setMainContent(List& list);
    void setMainContent(Button& button);
    void setMainContent(ScrollView& scrollView);
    
    /**
     * Window integration
     */
    
    /**
     * Add SplitView to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove SplitView from its parent view
     */
    void removeFromParent();
    
    /**
     * State
     */
    
    /**
     * Check if SplitView is valid
     */
    bool isValid() const;
    
    /**
     * Callbacks
     */
    
    /**
     * Set callback for sidebar toggle events
     * The callback will be called when the sidebar is collapsed or expanded
     * @param callback Function that takes a bool parameter (true if collapsed, false if expanded)
     */
    void setOnSidebarToggle(std::function<void(bool collapsed)> callback);
    
    // Non-copyable
    SplitView(const SplitView&) = delete;
    SplitView& operator=(const SplitView&) = delete;
    
    // Movable
    SplitView(SplitView&&) noexcept;
    SplitView& operator=(SplitView&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
