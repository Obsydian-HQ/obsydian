/**
 * macOS FFI - C++ Interface
 * 
 * Provides C++ interface for macOS platform features
 * This is the interface that the core FFI layer uses
 */

#pragma once

#ifdef __APPLE__

#include <cstdint>
#include <memory>
#include <string>
#include <functional>

namespace obsidian::ffi::macos {

/**
 * Window handle (opaque pointer)
 */
class Window {
public:
    Window();
    ~Window();
    
    /**
     * Create a window with the given parameters
     */
    bool create(int width, int height, const std::string& title);
    
    /**
     * Show the window
     */
    void show();
    
    /**
     * Close the window
     */
    void close();
    
    /**
     * Check if window is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal window handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // Movable
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

private:
    void* handle_;  // Opaque handle to native window
};

/**
 * Button handle (opaque pointer)
 */
class Button {
public:
    Button();
    ~Button();
    
    /**
     * Create a button with the given parameters
     */
    bool create(const std::string& title, int x, int y, int width, int height);
    
    /**
     * Set the button title/text
     */
    void setTitle(const std::string& title);
    
    /**
     * Get the button title/text
     */
    std::string getTitle() const;
    
    /**
     * Set the button click callback
     * The callback will be called when the button is clicked
     */
    void setOnClick(std::function<void()> callback);
    
    /**
     * Set button visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if button is visible
     */
    bool isVisible() const;
    
    /**
     * Set button enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if button is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add button to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove button from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if button is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal button handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;
    
    // Movable
    Button(Button&&) noexcept;
    Button& operator=(Button&&) noexcept;

private:
    void* handle_;  // Opaque handle to native button
    std::function<void()> callback_;  // C++ callback
    static void onClickCallback(void* userData);  // C callback bridge
};

/**
 * TextField handle (opaque pointer)
 */
class TextField {
public:
    TextField();
    ~TextField();
    
    /**
     * Create a text field with the given parameters
     */
    bool create(const std::string& placeholder, int x, int y, int width, int height);
    
    /**
     * Set the text field text
     */
    void setText(const std::string& text);
    
    /**
     * Get the text field text
     */
    std::string getText() const;
    
    /**
     * Set the text field placeholder text
     */
    void setPlaceholder(const std::string& placeholder);
    
    /**
     * Set the text field enter key callback
     * The callback will be called when the user presses Enter/Return
     */
    void setOnEnter(std::function<void()> callback);
    
    /**
     * Set text field visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if text field is visible
     */
    bool isVisible() const;
    
    /**
     * Set text field enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if text field is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add text field to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove text field from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if text field is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal text field handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    TextField(const TextField&) = delete;
    TextField& operator=(const TextField&) = delete;
    
    // Movable
    TextField(TextField&&) noexcept;
    TextField& operator=(TextField&&) noexcept;

private:
    void* handle_;  // Opaque handle to native text field
    std::function<void()> callback_;  // C++ callback
    static void onEnterCallback(void* userData);  // C callback bridge
};

/**
 * TextView handle (opaque pointer)
 */
class TextView {
public:
    TextView();
    ~TextView();
    
    /**
     * Create a text view with the given parameters
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Set the text view text
     */
    void setString(const std::string& text);
    
    /**
     * Get the text view text
     */
    std::string getString() const;
    
    /**
     * Set text view editable state
     */
    void setEditable(bool editable);
    
    /**
     * Check if text view is editable
     */
    bool isEditable() const;
    
    /**
     * Set text view selectable state
     */
    void setSelectable(bool selectable);
    
    /**
     * Check if text view is selectable
     */
    bool isSelectable() const;
    
    /**
     * Set text view visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if text view is visible
     */
    bool isVisible() const;
    
    /**
     * Set text view enabled state
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if text view is enabled
     */
    bool isEnabled() const;
    
    /**
     * Add text view to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove text view from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if text view is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal text view handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    /**
     * Get the underlying view handle (for use with ScrollView.setDocumentView)
     */
    void* getViewHandle() const;
    
    // Non-copyable
    TextView(const TextView&) = delete;
    TextView& operator=(const TextView&) = delete;
    
    // Movable
    TextView(TextView&&) noexcept;
    TextView& operator=(TextView&&) noexcept;

private:
    void* handle_;  // Opaque handle to native text view
};

/**
 * ScrollView handle (opaque pointer)
 */
class ScrollView {
public:
    ScrollView();
    ~ScrollView();
    
    /**
     * Create a scroll view with the given parameters
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Set the document view for the scroll view
     * The document view is the view that will be scrolled
     */
    void setDocumentView(TextView& textView);
    
    /**
     * Get the document view from the scroll view
     * Returns nullptr if no document view is set
     */
    void* getDocumentView() const;
    
    /**
     * Set scroll view visibility
     */
    void setVisible(bool visible);
    
    /**
     * Check if scroll view is visible
     */
    bool isVisible() const;
    
    /**
     * Add scroll view to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove scroll view from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if scroll view is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal scroll view handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    ScrollView(const ScrollView&) = delete;
    ScrollView& operator=(const ScrollView&) = delete;
    
    // Movable
    ScrollView(ScrollView&&) noexcept;
    ScrollView& operator=(ScrollView&&) noexcept;

private:
    void* handle_;  // Opaque handle to native scroll view
};

/**
 * Table handle (opaque pointer)
 */
class Table {
public:
    Table();
    ~Table();
    
    /**
     * Create a table with the given parameters
     */
    bool create(int x, int y, int width, int height);
    
    /**
     * Add a column to the table
     * @param identifier Unique identifier for the column
     * @param title Display title for the column header
     * @param width Initial width of the column in pixels
     * @return true if column was added successfully
     */
    bool addColumn(const std::string& identifier, const std::string& title, int width);
    
    /**
     * Add a row to the list
     * @param rowData Vector of strings, one for each column (in order of column creation)
     * @return The index of the newly added row, or -1 on failure
     */
    int addRow(const std::vector<std::string>& rowData);
    
    /**
     * Remove a row from the table
     * @param rowIndex Index of the row to remove (0-based)
     * @return true if row was removed successfully
     */
    bool removeRow(int rowIndex);
    
    /**
     * Remove all rows from the table
     */
    void clear();
    
    /**
     * Get the number of rows in the table
     */
    int getRowCount() const;
    
    /**
     * Get the number of columns in the table
     */
    int getColumnCount() const;
    
    /**
     * Set the data for a specific row
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
     * The callback will be called when a row is selected
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
    
    /**
     * Get the internal table handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
    
    // Movable
    Table(Table&&) noexcept;
    Table& operator=(Table&&) noexcept;

private:
    void* handle_;  // Opaque handle to native list
    std::function<void(int)> callback_;  // C++ callback
    static void onSelectionCallback(int rowIndex, void* userData);  // C callback bridge
};

/**
 * List handle (opaque pointer)
 */
class List {
public:
    List();
    ~List();
    
    /**
     * Create a list with the given parameters
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
     */
    void addToWindow(Window& window);
    
    /**
     * Remove list from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if list is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal list handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    List(const List&) = delete;
    List& operator=(const List&) = delete;
    
    // Movable
    List(List&&) noexcept;
    List& operator=(List&&) noexcept;

private:
    void* handle_;  // Opaque handle to native list
    std::function<void(int)> callback_;  // C++ callback
    static void onSelectionCallback(int itemIndex, void* userData);  // C callback bridge
};

/**
 * SplitView handle (opaque pointer)
 * Provides split view layout with collapse/expand and resizing capabilities
 */
class SplitView {
public:
    SplitView();
    ~SplitView();
    
    /**
     * Create a SplitView with the given parameters
     * @param position Split position (leading or trailing)
     * @param initialPrimaryPaneWidth Initial primary pane width in points (default: 200.0)
     * @param minPrimaryPaneWidth Minimum primary pane width in points (default: 150.0)
     * @param maxPrimaryPaneWidth Maximum primary pane width in points (default: 400.0)
     */
    bool create(int position, double initialPrimaryPaneWidth = 200.0, 
                double minPrimaryPaneWidth = 150.0, double maxPrimaryPaneWidth = 400.0);
    
    /**
     * Set the primary pane width (in points)
     */
    void setPrimaryPaneWidth(double width);
    
    /**
     * Get the current primary pane width (in points)
     */
    double getPrimaryPaneWidth() const;
    
    /**
     * Set minimum primary pane width (in points)
     */
    void setMinimumPrimaryPaneWidth(double width);
    
    /**
     * Set maximum primary pane width (in points)
     */
    void setMaximumPrimaryPaneWidth(double width);
    
    /**
     * Collapse the primary pane
     */
    void collapsePrimaryPane();
    
    /**
     * Expand the primary pane
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
     * Set the primary pane content view
     * @param viewHandle NSView handle to use as primary pane content
     */
    void setPrimaryPaneView(void* viewHandle);
    
    /**
     * Set the secondary pane content view
     * @param viewHandle NSView handle to use as secondary pane content
     */
    void setSecondaryPaneView(void* viewHandle);
    
    /**
     * Add SplitView to a window's content view
     */
    void addToWindow(Window& window);
    
    /**
     * Remove SplitView from its parent view
     */
    void removeFromParent();
    
    /**
     * Check if SplitView is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal SplitView handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    SplitView(const SplitView&) = delete;
    SplitView& operator=(const SplitView&) = delete;
    
    // Movable
    SplitView(SplitView&&) noexcept;
    SplitView& operator=(SplitView&&) noexcept;

private:
    void* handle_;  // Opaque handle to native SplitView
};

/**
 * ViewController handle (opaque pointer)
 * Wraps NSViewController for managing view hierarchies
 * Required for NSSplitViewController and NSSplitViewItem composition
 */
class ViewController {
public:
    ViewController();
    ~ViewController();
    
    /**
     * Create a ViewController
     */
    bool create();
    
    /**
     * Set the view for the view controller
     * @param viewHandle NSView handle (from VStack, HStack, List, etc.)
     */
    void setView(void* viewHandle);
    
    /**
     * Get the view from the view controller
     * @return NSView handle, or nullptr if not set
     */
    void* getView() const;
    
    /**
     * Check if view controller is valid
     */
    bool isValid() const;
    
    /**
     * Configure the view controller for use as a sidebar.
     * This wraps the current view in an NSVisualEffectView with the sidebar
     * material to get the native macOS sidebar appearance (translucent, vibrancy).
     * 
     * IMPORTANT: Call this AFTER setting the view via setView().
     */
    void configureForSidebar();
    
    /**
     * Set the preferred content size for the view controller.
     * This is CRITICAL for NSSplitViewController to properly size its child items.
     * NSSplitView uses the preferredContentSize of child view controllers to determine
     * the layout of split view items.
     * 
     * @param width Preferred width (use 0 for unconstrained)
     * @param height Preferred height (use 0 for unconstrained)
     */
    void setPreferredContentSize(double width, double height);
    
    /**
     * Get the preferred content size for the view controller.
     * @param outWidth Pointer to receive width (can be nullptr)
     * @param outHeight Pointer to receive height (can be nullptr)
     */
    void getPreferredContentSize(double* outWidth, double* outHeight) const;
    
    /**
     * Get the internal view controller handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    ViewController(const ViewController&) = delete;
    ViewController& operator=(const ViewController&) = delete;
    
    // Movable
    ViewController(ViewController&&) noexcept;
    ViewController& operator=(ViewController&&) noexcept;

private:
    void* handle_;  // Opaque handle to native view controller
};

/**
 * SplitViewController handle (opaque pointer)
 * Wraps NSSplitViewController for managing split view items
 */
class SplitViewController {
public:
    SplitViewController();
    ~SplitViewController();
    
    /**
     * Create a SplitViewController
     */
    bool create();
    
    /**
     * Get the view from the split view controller
     * @return NSView handle, or nullptr if not set
     */
    void* getView() const;
    
    /**
     * Add a split view item to the split view controller
     * @param splitViewItemHandle NSSplitViewItem handle
     */
    void addSplitViewItem(void* splitViewItemHandle);
    
    /**
     * Remove a split view item from the split view controller
     * @param splitViewItemHandle NSSplitViewItem handle to remove
     */
    void removeSplitViewItem(void* splitViewItemHandle);
    
    /**
     * Check if split view controller is valid
     */
    bool isValid() const;
    
    /**
     * Set the NSSplitView's frame size.
     * CRITICAL: This MUST be called BEFORE adding any split view items.
     * Real macOS apps (like Watt editor) set the split view frame size before adding items.
     * @param width Desired width
     * @param height Desired height
     */
    void setViewFrameSize(double width, double height);
    
    /**
     * Get the internal split view controller handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    SplitViewController(const SplitViewController&) = delete;
    SplitViewController& operator=(const SplitViewController&) = delete;
    
    // Movable
    SplitViewController(SplitViewController&&) noexcept;
    SplitViewController& operator=(SplitViewController&&) noexcept;

private:
    void* handle_;  // Opaque handle to native split view controller
};

/**
 * SplitViewItem handle (opaque pointer)
 * Wraps NSSplitViewItem for managing individual panes in a split view
 */
class SplitViewItem {
public:
    SplitViewItem();
    ~SplitViewItem();
    
    /**
     * Create a sidebar split view item with a view controller
     * This provides native macOS sidebar behavior (material background, collapse button)
     * @param viewControllerHandle NSViewController handle
     */
    bool createSidebarWithViewController(void* viewControllerHandle);
    
    /**
     * Create a content list split view item with a view controller
     * @param viewControllerHandle NSViewController handle
     */
    bool createContentListWithViewController(void* viewControllerHandle);
    
    /**
     * Create an inspector split view item with a view controller
     * @param viewControllerHandle NSViewController handle
     */
    bool createInspectorWithViewController(void* viewControllerHandle);
    
    /**
     * Set minimum thickness for the split view item
     */
    void setMinimumThickness(double thickness);
    
    /**
     * Get minimum thickness for the split view item
     */
    double getMinimumThickness() const;
    
    /**
     * Set maximum thickness for the split view item
     */
    void setMaximumThickness(double thickness);
    
    /**
     * Get maximum thickness for the split view item
     */
    double getMaximumThickness() const;
    
    /**
     * Set whether the split view item is collapsed
     */
    void setCollapsed(bool collapsed);
    
    /**
     * Get whether the split view item is collapsed
     */
    bool getCollapsed() const;
    
    /**
     * Set whether the split view item allows full-height layout (macOS 11+)
     * When true, the sidebar extends into the title bar area.
     */
    void setAllowsFullHeightLayout(bool allowed);
    
    /**
     * Get whether the split view item allows full-height layout
     */
    bool getAllowsFullHeightLayout() const;
    
    /**
     * Set whether the split view item can collapse
     */
    void setCanCollapse(bool canCollapse);
    
    /**
     * Get whether the split view item can collapse
     */
    bool getCanCollapse() const;
    
    /**
     * Set the holding priority for the split view item
     * Higher priority items resist being resized.
     */
    void setHoldingPriority(float priority);
    
    /**
     * Get the holding priority for the split view item
     */
    float getHoldingPriority() const;
    
    /**
     * Check if split view item is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal split view item handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    // Non-copyable
    SplitViewItem(const SplitViewItem&) = delete;
    SplitViewItem& operator=(const SplitViewItem&) = delete;
    
    // Movable
    SplitViewItem(SplitViewItem&&) noexcept;
    SplitViewItem& operator=(SplitViewItem&&) noexcept;

private:
    void* handle_;  // Opaque handle to native split view item
};

/**
 * Toolbar display mode enumeration
 */
enum class ToolbarDisplayMode {
    Default = 0,
    IconAndLabel = 1,
    IconOnly = 2,
    LabelOnly = 3
};

/**
 * Toolbar style enumeration (macOS 11+)
 */
enum class ToolbarStyle {
    Automatic = 0,
    Expanded = 1,
    Preference = 2,
    Unified = 3,
    UnifiedCompact = 4
};

/**
 * Toolbar handle (opaque pointer)
 * Wraps NSToolbar for managing window toolbars
 * 
 * REQUIRED for native sidebar collapse button functionality.
 * The collapse button is provided by NSToolbarToggleSidebarItemIdentifier.
 */
class Toolbar {
public:
    Toolbar();
    ~Toolbar();
    
    /**
     * Create a toolbar with the given identifier
     * @param identifier Unique identifier for the toolbar
     */
    bool create(const std::string& identifier = "ObsidianMainToolbar");
    
    /**
     * Set the toolbar's display mode
     */
    void setDisplayMode(ToolbarDisplayMode mode);
    
    /**
     * Get the toolbar's display mode
     */
    ToolbarDisplayMode getDisplayMode() const;
    
    /**
     * Insert the sidebar toggle button at the specified index
     * This adds NSToolbarToggleSidebarItemIdentifier which provides
     * the native collapse/expand button for sidebars.
     * @param index Index where to insert the item
     */
    void insertSidebarToggleItem(int index = 0);
    
    /**
     * Insert the sidebar tracking separator at the specified index (macOS 11+)
     * This provides proper separator tracking when the sidebar is resized.
     * @param index Index where to insert the separator
     */
    void insertSidebarTrackingSeparator(int index = 1);
    
    /**
     * Insert a flexible space item at the specified index
     * @param index Index where to insert the item
     */
    void insertFlexibleSpace(int index);
    
    /**
     * Get the number of items in the toolbar
     */
    int getItemCount() const;
    
    /**
     * Check if toolbar is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal toolbar handle (for internal use)
     */
    void* getHandle() const { return handle_; }
    
    /**
     * Get the actual NSToolbar* handle (for internal use by Window)
     */
    void* getNSToolbarHandle() const;
    
    // Non-copyable
    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;
    
    // Movable
    Toolbar(Toolbar&&) noexcept;
    Toolbar& operator=(Toolbar&&) noexcept;

private:
    void* handle_;  // Opaque handle to native toolbar
};

/**
 * Process handle (opaque pointer)
 */
class Process {
public:
    Process();
    ~Process();
    
    /**
     * Create a process with the given command and arguments
     * @param command The command to execute (e.g., "/bin/ls")
     * @param arguments Vector of argument strings
     * @param workingDirectory Working directory (empty string for current)
     * @return true if process was created successfully
     */
    bool create(const std::string& command, 
                const std::vector<std::string>& arguments = {},
                const std::string& workingDirectory = "");
    
    /**
     * Start the process
     * @return true if process was started successfully
     */
    bool start();
    
    /**
     * Set the stdout callback
     * The callback will be called when data is available on stdout
     */
    void setOnStdout(std::function<void(const std::string&)> callback);
    
    /**
     * Set the stderr callback
     * The callback will be called when data is available on stderr
     */
    void setOnStderr(std::function<void(const std::string&)> callback);
    
    /**
     * Set the termination callback
     * The callback will be called when the process terminates
     */
    void setOnTermination(std::function<void(int exitCode)> callback);
    
    /**
     * Set the error callback
     * The callback will be called when an error occurs (e.g., failed to launch)
     */
    void setOnError(std::function<void(const std::string& errorMessage)> callback);
    
    /**
     * Terminate the process
     */
    void terminate();
    
    /**
     * Check if the process is currently running
     */
    bool isRunning() const;
    
    /**
     * Wait for the process to finish
     * Blocks until the process terminates
     * @return The exit code of the process
     */
    int waitUntilExit();
    
    /**
     * Get the exit code of the process
     * Only valid after the process has terminated
     * @return The exit code, or -1 if the process hasn't terminated yet
     */
    int getExitCode() const;
    
    /**
     * Get the process identifier (PID) of the process
     * Only valid after the process has been started
     * @return The process ID, or -1 if the process hasn't been started yet or is invalid
     */
    int32_t getPid() const;
    
    /**
     * Check if process is valid
     */
    bool isValid() const;
    
    // Non-copyable
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    
    // Movable
    Process(Process&&) noexcept;
    Process& operator=(Process&&) noexcept;

private:
    void* handle_;  // Opaque handle to native process
    std::function<void(const std::string&)> stdoutCallback_;
    std::function<void(const std::string&)> stderrCallback_;
    std::function<void(int)> terminationCallback_;
    std::function<void(const std::string&)> errorCallback_;
    static void onStdoutCallback(const char* data, bool isStderr, void* userData);
    static void onStderrCallback(const char* data, bool isStderr, void* userData);
    static void onTerminationCallback(int exitCode, void* userData);
    static void onErrorCallback(const char* errorMessage, void* userData);
};

/**
 * Process information structure
 */
struct ProcessInfo {
    int32_t processId;  // Process ID (PID)
    std::string processName;  // Process name (e.g., "Finder")
    std::string bundleIdentifier;  // Bundle identifier (e.g., "com.apple.finder")
    bool isActive;  // Whether the process is currently active
};

/**
 * Process list management
 */
class ProcessList {
public:
    /**
     * Get all running processes
     * @return Vector of process information
     */
    static std::vector<ProcessInfo> getAllProcesses();
    
    /**
     * Kill a process by its process ID
     * @param processId The process ID to kill
     * @return true if the process was successfully killed
     */
    static bool killProcess(int32_t processId);
    
    /**
     * Get process information for a specific process ID
     * @param processId The process ID to query
     * @param processInfo Output parameter for process information
     * @return true if process information was successfully retrieved
     */
    static bool getProcessInfo(int32_t processId, ProcessInfo& processInfo);
};

/**
 * Initialize macOS platform
 */
bool initializePlatform();

/**
 * Run the macOS event loop
 */
void runEventLoop();

/**
 * Stop the macOS event loop
 */
void stopEventLoop();

/**
 * Cleanup macOS platform
 */
void cleanupPlatform();

} // namespace obsidian::ffi::macos

#endif // __APPLE__

