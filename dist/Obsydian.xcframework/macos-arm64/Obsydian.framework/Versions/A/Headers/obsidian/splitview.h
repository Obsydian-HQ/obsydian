/**
 * Obsidian Public API - SplitView
 * 
 * SplitView provides a split view layout using NSSplitView with
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
 * Split position enumeration
 * Specifies which side the primary pane appears on
 */
enum class SplitPosition {
    Leading,   // Primary pane on left side (LTR)
    Trailing   // Primary pane on right side (LTR)
};

/**
 * SplitView class - Split view layout container
 * 
 * Provides a split view using NSSplitView with collapse/expand,
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
     * Create the SplitView with the given split position
     * @param position Split position (Leading or Trailing)
     * @return true if SplitView was created successfully
     */
    bool create(SplitPosition position = SplitPosition::Leading);
    
    /**
     * Primary pane width management
     */
    
    /**
     * Set the primary pane width (in points)
     * The width will be clamped to min/max values
     */
    void setPrimaryPaneWidth(double width);
    
    /**
     * Get the current primary pane width (in points)
     */
    double getPrimaryPaneWidth() const;
    
    /**
     * Set minimum primary pane width (in points, default: 150.0)
     */
    void setMinimumPrimaryPaneWidth(double width);
    
    /**
     * Set maximum primary pane width (in points, default: 400.0)
     */
    void setMaximumPrimaryPaneWidth(double width);
    
    /**
     * Collapse/expand management
     */
    
    /**
     * Collapse the primary pane
     */
    void collapsePrimaryPane();
    
    /**
     * Expand the primary pane (restore from collapsed state)
     */
    void expandPrimaryPane();
    
    /**
     * Toggle primary pane collapse/expand state
     */
    void togglePrimaryPane();
    
    /**
     * Check if primary pane is currently collapsed
     */
    bool isPrimaryPaneCollapsed() const;
    
    /**
     * Content management
     */
    
    /**
     * Set the primary pane content view
     */
    void setPrimaryPaneContent(VStack& vstack);
    void setPrimaryPaneContent(HStack& hstack);
    void setPrimaryPaneContent(List& list);
    void setPrimaryPaneContent(Button& button);
    void setPrimaryPaneContent(ScrollView& scrollView);
    
    /**
     * Set the secondary pane content view
     */
    void setSecondaryPaneContent(VStack& vstack);
    void setSecondaryPaneContent(HStack& hstack);
    void setSecondaryPaneContent(List& list);
    void setSecondaryPaneContent(Button& button);
    void setSecondaryPaneContent(ScrollView& scrollView);
    
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
     * Set callback for primary pane toggle events
     * The callback will be called when the primary pane is collapsed or expanded
     * @param callback Function that takes a bool parameter (true if collapsed, false if expanded)
     */
    void setOnPrimaryPaneToggle(std::function<void(bool collapsed)> callback);
    
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
