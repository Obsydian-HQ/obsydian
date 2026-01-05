/**
 * Obsidian Public API - List
 * 
 * Simple single-column list component for displaying text items.
 * 
 * The List component provides a clean API for creating simple lists of text items,
 * similar to SwiftUI's List view. On macOS, it uses NSTableView configured as a
 * single-column list (no headers, simple text items).
 * 
 * Example usage:
 * 
 *     List list;
 *     list.create(10, 10, 300, 200);
 *     list.addItem("Item 1");
 *     list.addItem("Item 2");
 *     list.addItem("Item 3");
 *     list.setOnSelection([](int index) {
 *         std::cout << "Selected: " << index << "\n";
 *     });
 *     list.addToWindow(window);
 */

#pragma once

#include <string>
#include <functional>
#include <memory>

namespace obsidian {

class Window;

/**
 * List class - Platform-agnostic list management
 * 
 * Provides a simple API for creating and managing single-column text lists.
 * Each item in the list is a simple text string.
 */
class List {
public:
    /**
     * Create a new list
     */
    List();
    
    /**
     * Destructor - automatically removes and destroys the list
     */
    ~List();
    
    /**
     * Create a list with the given position and size
     * @param x X position in window coordinates
     * @param y Y position in window coordinates
     * @param width Width of the list
     * @param height Height of the list
     * @return true if list was created successfully
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Add an item to the list
     * @param itemText The text to display for this item
     * @return The index of the newly added item, or -1 on failure
     */
    int addItem(const std::string& itemText);
    
    /**
     * Remove an item from the list
     * @param itemIndex Index of the item to remove (0-based)
     * @return true if item was removed successfully
     */
    bool removeItem(int itemIndex);
    
    /**
     * Remove all items from the list
     */
    void clear();
    
    /**
     * Get the number of items in the list
     */
    int getItemCount() const;
    
    /**
     * Get the text for a specific item
     * @param itemIndex Index of the item to get (0-based)
     * @return The item text, or empty string if invalid
     */
    std::string getItem(int itemIndex) const;
    
    /**
     * Set the text for a specific item
     * @param itemIndex Index of the item to update (0-based)
     * @param itemText The new text for this item
     * @return true if item was updated successfully
     */
    bool setItem(int itemIndex, const std::string& itemText);
    
    /**
     * Set the list selection callback
     * The callback will be called when an item is selected
     * @param callback Function to call when an item is selected (receives item index)
     */
    void setOnSelection(std::function<void(int itemIndex)> callback);
    
    /**
     * Get the currently selected item index
     * @return The index of the selected item (0-based), or -1 if no item is selected
     */
    int getSelectedIndex() const;
    
    /**
     * Set the selected item
     * @param itemIndex Index of the item to select (0-based), or -1 to deselect all
     * @return true if selection was set successfully
     */
    bool setSelectedIndex(int itemIndex);
    
    /**
     * Set list visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if list is visible
     */
    bool isVisible() const;
    
    /**
     * Set list enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if list is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add list to a window's content view
     * The list will be positioned according to the coordinates specified in create()
     */
    void addToWindow(Window& window);
    
    /**
     * Remove list from its parent view
     * Note: List remains valid after removal - it's just not attached to a parent
     */
    void removeFromParent();
    
    /**
     * Check if list is valid (has been created)
     */
    bool isValid() const;
    
    /**
     * Get the native view handle for this List
     * Returns an opaque pointer to the underlying NSView (NSScrollView), or nullptr if invalid
     * This is useful for integrating with other components that need direct view access (e.g., SplitView)
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    List(const List&) = delete;
    List& operator=(const List&) = delete;
    
    // Movable
    List(List&& other) noexcept = default;
    List& operator=(List&& other) noexcept = default;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

