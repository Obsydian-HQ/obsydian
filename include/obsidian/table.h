/**
 * Obsidian Public API - Table
 * 
 * This is the public API header for table management.
 * Users should include <obsidian/obsidian.h> for the full API.
 * 
 * The Table component provides a unified interface for displaying tabular data.
 * On macOS, it uses NSTableView wrapped in NSScrollView for scrolling.
 * The API is designed to be extensible to support NSOutlineView and NSCollectionView
 * in the future without breaking changes.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace obsidian {

// Forward declaration
class Window;

/**
 * Table class - Platform-agnostic table management
 * 
 * This class provides a clean, high-level interface for creating and managing
 * tables (multi-column data grids) across all platforms. Platform-specific details are hidden.
 * 
 * The Table component supports:
 * - Multiple columns with customizable headers
 * - Adding, removing, and updating rows
 * - Row selection with callbacks
 * - Automatic scrolling (wrapped in ScrollView on macOS)
 * - Visibility and enabled state management
 */
class Table {
public:
    /**
     * Create a new list
     */
    Table();
    
    /**
     * Destructor - automatically removes and destroys the table
     */
    ~Table();
    
    /**
     * Create a table with the given parameters
     * @param x X position in pixels
     * @param y Y position in pixels
     * @param width Table width in pixels
     * @param height Table height in pixels
     * @return true if table was created successfully
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Add a column to the list
     * Columns must be added before adding rows.
     * @param identifier Unique identifier for the column (used internally)
     * @param title Display title for the column header
     * @param width Initial width of the column in pixels
     * @return true if column was added successfully
     */
    bool addColumn(const std::string& identifier, const std::string& title, int width);
    
    /**
     * Add a row to the list
     * The number of strings in rowData must match the number of columns.
     * @param rowData Vector of strings, one for each column (in order of column creation)
     * @return The index of the newly added row, or -1 on failure
     */
    int addRow(const std::vector<std::string>& rowData);
    
    /**
     * Remove a row from the list
     * @param rowIndex Index of the row to remove (0-based)
     * @return true if row was removed successfully
     */
    bool removeRow(int rowIndex);
    
    /**
     * Remove all rows from the list
     */
    void clear();
    
    /**
     * Get the number of rows in the list
     */
    int getRowCount() const;
    
    /**
     * Get the number of columns in the list
     */
    int getColumnCount() const;
    
    /**
     * Set the data for a specific row
     * The number of strings in rowData must match the number of columns.
     * @param rowIndex Index of the row to update (0-based)
     * @param rowData Vector of strings, one for each column (in order of column creation)
     * @return true if row was updated successfully
     */
    bool setRowData(int rowIndex, const std::vector<std::string>& rowData);
    
    /**
     * Get the data for a specific row
     * @param rowIndex Index of the row to get (0-based)
     * @param columnIndex Index of the column to get (0-based)
     * @return The string data, or empty string if invalid
     */
    std::string getRowData(int rowIndex, int columnIndex) const;
    
    /**
     * Set the table selection callback
     * The callback will be called when the user selects a row
     * @param callback Function that receives the selected row index (0-based)
     */
    void setOnSelection(std::function<void(int rowIndex)> callback);
    
    /**
     * Get the currently selected row index
     * @return The index of the selected row (0-based), or -1 if no row is selected
     */
    int getSelectedRow() const;
    
    /**
     * Set the selected row
     * @param rowIndex Index of the row to select (0-based), or -1 to deselect all
     * @return true if selection was set successfully
     */
    bool setSelectedRow(int rowIndex);
    
    /**
     * Set table visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if table is visible
     */
    bool isVisible() const;
    
    /**
     * Set table enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if table is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add table to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove table from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if table is valid
     */
    bool isValid() const;
    
    // Non-copyable
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
    
    // Movable
    Table(Table&&) noexcept;
    Table& operator=(Table&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

