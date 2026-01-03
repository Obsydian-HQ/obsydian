/**
 * macOS FFI - C++ Implementation
 * 
 * C++ wrapper around the Objective-C++ bridge
 */

#include "macos_ffi.h"

#ifdef __APPLE__

#include "macos_window.h"
#include "macos_button.h"
#include "macos_textfield.h"
#include "macos_textview.h"
#include "macos_scrollview.h"
#include "macos_table.h"
#include "macos_list.h"
#include "macos_process.h"
#include "macos_process_list.h"
#include "macos_platform.h"
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>

namespace obsidian::ffi::macos {

Window::Window() : handle_(nullptr) {}

Window::~Window() {
    close();
}

bool Window::create(int width, int height, const std::string& title) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianWindowParams params;
    params.width = width;
    params.height = height;
    params.title = title.c_str();
    
    handle_ = obsidian_macos_create_window(params);
    return handle_ != nullptr;
}

void Window::show() {
    if (handle_) {
        obsidian_macos_show_window(handle_);
    }
}

void Window::close() {
    if (handle_) {
        obsidian_macos_destroy_window(handle_);
        handle_ = nullptr;
    }
}

bool Window::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_window_is_valid(handle_);
}

Window::Window(Window&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

bool initializePlatform() {
    return obsidian_macos_platform_init();
}

void runEventLoop() {
    obsidian_macos_platform_run();
}

void stopEventLoop() {
    obsidian_macos_platform_stop();
}

void cleanupPlatform() {
    obsidian_macos_platform_cleanup();
}

// Button implementation
Button::Button() : handle_(nullptr), callback_(nullptr) {}

Button::~Button() {
    if (handle_) {
        obsidian_macos_destroy_button(handle_);
        handle_ = nullptr;
    }
}

bool Button::create(const std::string& title, int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianButtonParams params;
    params.title = title.c_str();
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_button(params);
    if (handle_) {
        // Register callback if it was set before create()
        if (callback_) {
            obsidian_macos_button_set_on_click(handle_, onClickCallback, this);
        }
        return true;
    }
    return false;
}

void Button::setTitle(const std::string& title) {
    if (handle_) {
        obsidian_macos_button_set_title(handle_, title.c_str());
    }
}

std::string Button::getTitle() const {
    if (handle_) {
        const char* title = obsidian_macos_button_get_title(handle_);
        if (title) {
            return std::string(title);
        }
    }
    return std::string();
}

void Button::setOnClick(std::function<void()> callback) {
    callback_ = callback;
    // Register callback if button handle exists (created)
    // If handle doesn't exist yet, callback will be registered when create() is called
    if (handle_) {
        obsidian_macos_button_set_on_click(handle_, onClickCallback, this);
    }
}

void Button::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_button_set_visible(handle_, visible);
    }
}

bool Button::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_button_is_visible(handle_);
}

void Button::setEnabled(bool enabled) {
    if (handle_) {
        obsidian_macos_button_set_enabled(handle_, enabled);
    }
}

bool Button::isEnabled() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_button_is_enabled(handle_);
}

void Button::addToWindow(Window& window) {
    if (handle_ && window.isValid()) {
        void* windowHandle = window.getHandle();
        obsidian_macos_button_add_to_window(handle_, windowHandle);
    }
}

void Button::removeFromParent() {
    if (handle_) {
        obsidian_macos_button_remove_from_parent(handle_);
    }
}

bool Button::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_button_is_valid(handle_);
}

Button::Button(Button&& other) noexcept 
    : handle_(other.handle_), callback_(std::move(other.callback_)) {
    other.handle_ = nullptr;
    // Update callback userData to point to this instance
    if (handle_ && callback_) {
        obsidian_macos_button_set_on_click(handle_, onClickCallback, this);
    }
}

Button& Button::operator=(Button&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_button(handle_);
        }
        handle_ = other.handle_;
        callback_ = std::move(other.callback_);
        other.handle_ = nullptr;
        // Update callback userData to point to this instance
        if (handle_ && callback_) {
            obsidian_macos_button_set_on_click(handle_, onClickCallback, this);
        }
    }
    return *this;
}

void Button::onClickCallback(void* userData) {
    Button* button = static_cast<Button*>(userData);
    if (button && button->callback_) {
        button->callback_();
    }
}

// TextField implementation
TextField::TextField() : handle_(nullptr), callback_(nullptr) {}

TextField::~TextField() {
    if (handle_) {
        obsidian_macos_destroy_textfield(handle_);
        handle_ = nullptr;
    }
}

bool TextField::create(const std::string& placeholder, int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianTextFieldParams params;
    params.placeholder = placeholder.c_str();
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_textfield(params);
    return handle_ != nullptr;
}

void TextField::setText(const std::string& text) {
    if (handle_) {
        obsidian_macos_textfield_set_text(handle_, text.c_str());
    }
}

std::string TextField::getText() const {
    if (handle_) {
        const char* text = obsidian_macos_textfield_get_text(handle_);
        if (text) {
            std::string result(text);
            // Free the strdup'd string from the C function
            free((void*)text);
            return result;
        }
    }
    return std::string();
}

void TextField::setPlaceholder(const std::string& placeholder) {
    if (handle_) {
        obsidian_macos_textfield_set_placeholder(handle_, placeholder.c_str());
    }
}

void TextField::setOnEnter(std::function<void()> callback) {
    callback_ = callback;
    if (handle_) {
        obsidian_macos_textfield_set_on_enter(handle_, onEnterCallback, this);
    }
}

void TextField::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_textfield_set_visible(handle_, visible);
    }
}

bool TextField::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textfield_is_visible(handle_);
}

void TextField::setEnabled(bool enabled) {
    if (handle_) {
        obsidian_macos_textfield_set_enabled(handle_, enabled);
    }
}

bool TextField::isEnabled() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textfield_is_enabled(handle_);
}

void TextField::addToWindow(Window& window) {
    if (handle_ && window.isValid()) {
        void* windowHandle = window.getHandle();
        obsidian_macos_textfield_add_to_window(handle_, windowHandle);
    }
}

void TextField::removeFromParent() {
    if (handle_) {
        obsidian_macos_textfield_remove_from_parent(handle_);
    }
}

bool TextField::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textfield_is_valid(handle_);
}

TextField::TextField(TextField&& other) noexcept 
    : handle_(other.handle_), callback_(std::move(other.callback_)) {
    other.handle_ = nullptr;
    // Update callback userData to point to this instance
    if (handle_ && callback_) {
        obsidian_macos_textfield_set_on_enter(handle_, onEnterCallback, this);
    }
}

TextField& TextField::operator=(TextField&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_textfield(handle_);
        }
        handle_ = other.handle_;
        callback_ = std::move(other.callback_);
        other.handle_ = nullptr;
        // Update callback userData to point to this instance
        if (handle_ && callback_) {
            obsidian_macos_textfield_set_on_enter(handle_, onEnterCallback, this);
        }
    }
    return *this;
}

void TextField::onEnterCallback(void* userData) {
    TextField* textField = static_cast<TextField*>(userData);
    if (textField && textField->callback_) {
        textField->callback_();
    }
}

// TextView implementation
TextView::TextView() : handle_(nullptr) {}

TextView::~TextView() {
    if (handle_) {
        obsidian_macos_destroy_textview(handle_);
        handle_ = nullptr;
    }
}

bool TextView::create(int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianTextViewParams params;
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_textview(params);
    return handle_ != nullptr;
}

void TextView::setString(const std::string& text) {
    if (handle_) {
        obsidian_macos_textview_set_string(handle_, text.c_str());
    }
}

std::string TextView::getString() const {
    if (handle_) {
        const char* text = obsidian_macos_textview_get_string(handle_);
        if (text) {
            return std::string(text);
        }
    }
    return std::string();
}

void TextView::setEditable(bool editable) {
    if (handle_) {
        obsidian_macos_textview_set_editable(handle_, editable);
    }
}

bool TextView::isEditable() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textview_is_editable(handle_);
}

void TextView::setSelectable(bool selectable) {
    if (handle_) {
        obsidian_macos_textview_set_selectable(handle_, selectable);
    }
}

bool TextView::isSelectable() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textview_is_selectable(handle_);
}

void TextView::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_textview_set_visible(handle_, visible);
    }
}

bool TextView::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textview_is_visible(handle_);
}

void TextView::setEnabled(bool enabled) {
    if (handle_) {
        obsidian_macos_textview_set_enabled(handle_, enabled);
    }
}

bool TextView::isEnabled() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textview_is_enabled(handle_);
}

void TextView::addToWindow(Window& window) {
    if (handle_ && window.isValid()) {
        void* windowHandle = window.getHandle();
        obsidian_macos_textview_add_to_window(handle_, windowHandle);
    }
}

void TextView::removeFromParent() {
    if (handle_) {
        obsidian_macos_textview_remove_from_parent(handle_);
    }
}

bool TextView::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_textview_is_valid(handle_);
}

void* TextView::getViewHandle() const {
    if (handle_) {
        return obsidian_macos_textview_get_view_handle(handle_);
    }
    return nullptr;
}

TextView::TextView(TextView&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

TextView& TextView::operator=(TextView&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_textview(handle_);
        }
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

// ScrollView implementation
ScrollView::ScrollView() : handle_(nullptr) {}

ScrollView::~ScrollView() {
    if (handle_) {
        obsidian_macos_destroy_scrollview(handle_);
        handle_ = nullptr;
    }
}

bool ScrollView::create(int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianScrollViewParams params;
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_scrollview(params);
    return handle_ != nullptr;
}

void ScrollView::setDocumentView(TextView& textView) {
    if (handle_ && textView.isValid()) {
        void* viewHandle = textView.getViewHandle();
        if (viewHandle) {
            obsidian_macos_scrollview_set_document_view(handle_, viewHandle);
        }
    }
}

void* ScrollView::getDocumentView() const {
    if (handle_) {
        return obsidian_macos_scrollview_get_document_view(handle_);
    }
    return nullptr;
}

void ScrollView::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_scrollview_set_visible(handle_, visible);
    }
}

bool ScrollView::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_scrollview_is_visible(handle_);
}

void ScrollView::addToWindow(Window& window) {
    if (handle_ && window.isValid()) {
        void* windowHandle = window.getHandle();
        obsidian_macos_scrollview_add_to_window(handle_, windowHandle);
    }
}

void ScrollView::removeFromParent() {
    if (handle_) {
        obsidian_macos_scrollview_remove_from_parent(handle_);
    }
}

bool ScrollView::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_scrollview_is_valid(handle_);
}

ScrollView::ScrollView(ScrollView&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

ScrollView& ScrollView::operator=(ScrollView&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_scrollview(handle_);
        }
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

// Table implementation
Table::Table() : handle_(nullptr), callback_(nullptr) {}

Table::~Table() {
    if (handle_) {
        obsidian_macos_destroy_table(handle_);
        handle_ = nullptr;
    }
}

bool Table::create(int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianTableParams params;
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_table(params);
    return handle_ != nullptr;
}

bool Table::addColumn(const std::string& identifier, const std::string& title, int width) {
    if (handle_) {
        return obsidian_macos_table_add_column(handle_, identifier.c_str(), title.c_str(), width);
    }
    return false;
}

int Table::addRow(const std::vector<std::string>& rowData) {
    if (!handle_ || rowData.empty()) {
        return -1;
    }
    
    // Convert vector to C array
    std::vector<const char*> cStrings;
    for (const auto& str : rowData) {
        cStrings.push_back(str.c_str());
    }
    
    return obsidian_macos_table_add_row(handle_, cStrings.data(), (int)rowData.size());
}

bool Table::removeRow(int rowIndex) {
    if (handle_) {
        return obsidian_macos_table_remove_row(handle_, rowIndex);
    }
    return false;
}

void Table::clear() {
    if (handle_) {
        obsidian_macos_table_clear(handle_);
    }
}

int Table::getRowCount() const {
    if (handle_) {
        return obsidian_macos_table_get_row_count(handle_);
    }
    return 0;
}

int Table::getColumnCount() const {
    if (handle_) {
        return obsidian_macos_table_get_column_count(handle_);
    }
    return 0;
}

bool Table::setRowData(int rowIndex, const std::vector<std::string>& rowData) {
    if (!handle_ || rowData.empty()) {
        return false;
    }
    
    // Convert vector to C array
    std::vector<const char*> cStrings;
    for (const auto& str : rowData) {
        cStrings.push_back(str.c_str());
    }
    
    return obsidian_macos_table_set_row_data(handle_, rowIndex, cStrings.data(), (int)rowData.size());
}

std::string Table::getRowData(int rowIndex, int columnIndex) const {
    if (handle_) {
        const char* data = obsidian_macos_table_get_row_data(handle_, rowIndex, columnIndex);
        if (data) {
            return std::string(data);
        }
    }
    return std::string();
}

void Table::setOnSelection(std::function<void(int rowIndex)> callback) {
    callback_ = callback;
    if (handle_) {
        obsidian_macos_table_set_on_selection(handle_, onSelectionCallback, this);
    }
}

int Table::getSelectedRow() const {
    if (handle_) {
        return obsidian_macos_table_get_selected_row(handle_);
    }
    return -1;
}

bool Table::setSelectedRow(int rowIndex) {
    if (handle_) {
        return obsidian_macos_table_set_selected_row(handle_, rowIndex);
    }
    return false;
}

void Table::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_table_set_visible(handle_, visible);
    }
}

bool Table::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_table_is_visible(handle_);
}

void Table::setEnabled(bool enabled) {
    if (handle_) {
        obsidian_macos_table_set_enabled(handle_, enabled);
    }
}

bool Table::isEnabled() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_table_is_enabled(handle_);
}

void Table::addToWindow(Window& window) {
    if (handle_ && window.isValid()) {
        void* windowHandle = window.getHandle();
        obsidian_macos_table_add_to_window(handle_, windowHandle);
    }
}

void Table::removeFromParent() {
    if (handle_) {
        obsidian_macos_table_remove_from_parent(handle_);
    }
}

bool Table::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_table_is_valid(handle_);
}

Table::Table(Table&& other) noexcept : handle_(other.handle_), callback_(std::move(other.callback_)) {
    other.handle_ = nullptr;
    // Update callback userData to point to this instance
    if (handle_ && callback_) {
        obsidian_macos_table_set_on_selection(handle_, onSelectionCallback, this);
    }
}

Table& Table::operator=(Table&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_table(handle_);
        }
        handle_ = other.handle_;
        callback_ = std::move(other.callback_);
        other.handle_ = nullptr;
        // Update callback userData to point to this instance
        if (handle_ && callback_) {
            obsidian_macos_table_set_on_selection(handle_, onSelectionCallback, this);
        }
    }
    return *this;
}

void Table::onSelectionCallback(int rowIndex, void* userData) {
    Table* table = static_cast<Table*>(userData);
    if (table && table->callback_) {
        table->callback_(rowIndex);
    }
}

// List implementation
List::List() : handle_(nullptr), callback_(nullptr) {}

List::~List() {
    if (handle_) {
        obsidian_macos_destroy_list(handle_);
        handle_ = nullptr;
    }
}

bool List::create(int x, int y, int width, int height) {
    if (handle_) {
        return false; // Already created
    }
    
    ObsidianListParams params;
    params.x = x;
    params.y = y;
    params.width = width;
    params.height = height;
    
    handle_ = obsidian_macos_create_list(params);
    if (!handle_) {
        return false;
    }
    
    return true;
}

int List::addItem(const std::string& itemText) {
    if (!handle_ || itemText.empty()) {
        return -1;
    }
    return obsidian_macos_list_add_item(handle_, itemText.c_str());
}

bool List::removeItem(int itemIndex) {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_list_remove_item(handle_, itemIndex);
}

void List::clear() {
    if (handle_) {
        obsidian_macos_list_clear(handle_);
    }
}

int List::getItemCount() const {
    if (!handle_) {
        return 0;
    }
    return obsidian_macos_list_get_item_count(handle_);
}

std::string List::getItem(int itemIndex) const {
    if (!handle_) {
        return "";
    }
    const char* item = obsidian_macos_list_get_item(handle_, itemIndex);
    if (!item) {
        return "";
    }
    return std::string(item);
}

bool List::setItem(int itemIndex, const std::string& itemText) {
    if (!handle_ || itemText.empty()) {
        return false;
    }
    return obsidian_macos_list_set_item(handle_, itemIndex, itemText.c_str());
}

void List::setOnSelection(std::function<void(int itemIndex)> callback) {
    callback_ = callback;
    if (handle_) {
        obsidian_macos_list_set_on_selection(handle_, onSelectionCallback, this);
    }
}

int List::getSelectedIndex() const {
    if (!handle_) {
        return -1;
    }
    return obsidian_macos_list_get_selected_index(handle_);
}

bool List::setSelectedIndex(int itemIndex) {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_list_set_selected_index(handle_, itemIndex);
}

void List::setVisible(bool visible) {
    if (handle_) {
        obsidian_macos_list_set_visible(handle_, visible);
    }
}

bool List::isVisible() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_list_is_visible(handle_);
}

void List::setEnabled(bool enabled) {
    if (handle_) {
        obsidian_macos_list_set_enabled(handle_, enabled);
    }
}

bool List::isEnabled() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_list_is_enabled(handle_);
}

void List::addToWindow(Window& window) {
    if (!handle_) {
        return;
    }
    
    void* windowHandle = window.getHandle();
    if (windowHandle) {
        obsidian_macos_list_add_to_window(handle_, windowHandle);
    }
}

void List::removeFromParent() {
    if (handle_) {
        obsidian_macos_list_remove_from_parent(handle_);
    }
}

bool List::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_list_is_valid(handle_);
}

List::List(List&& other) noexcept : handle_(other.handle_), callback_(std::move(other.callback_)) {
    other.handle_ = nullptr;
    // Update callback userData to point to this instance
    if (handle_ && callback_) {
        obsidian_macos_list_set_on_selection(handle_, onSelectionCallback, this);
    }
}

List& List::operator=(List&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_list(handle_);
        }
        handle_ = other.handle_;
        callback_ = std::move(other.callback_);
        other.handle_ = nullptr;
        // Update callback userData to point to this instance
        if (handle_ && callback_) {
            obsidian_macos_list_set_on_selection(handle_, onSelectionCallback, this);
        }
    }
    return *this;
}

void List::onSelectionCallback(int itemIndex, void* userData) {
    List* list = static_cast<List*>(userData);
    if (list && list->callback_) {
        list->callback_(itemIndex);
    }
}

// Process implementation
Process::Process() : handle_(nullptr) {}

Process::~Process() {
    if (handle_) {
        obsidian_macos_destroy_process(handle_);
        handle_ = nullptr;
    }
}

bool Process::create(const std::string& command, 
                    const std::vector<std::string>& arguments,
                    const std::string& workingDirectory) {
    if (handle_) {
        return false; // Already created
    }
    
    if (command.empty()) {
        return false;
    }
    
    ObsidianProcessParams params;
    params.command = command.c_str();
    params.argumentCount = static_cast<int>(arguments.size());
    
    // Convert vector<string> to const char** array
    std::vector<const char*> argPtrs;
    for (const auto& arg : arguments) {
        argPtrs.push_back(arg.c_str());
    }
    params.arguments = argPtrs.empty() ? nullptr : argPtrs.data();
    
    params.workingDirectory = workingDirectory.empty() ? nullptr : workingDirectory.c_str();
    
    handle_ = obsidian_macos_create_process(params);
    if (handle_) {
        if (stdoutCallback_) {
            obsidian_macos_process_set_on_stdout(handle_, onStdoutCallback, this);
        }
        if (stderrCallback_) {
            obsidian_macos_process_set_on_stderr(handle_, onStderrCallback, this);
        }
        if (terminationCallback_) {
            obsidian_macos_process_set_on_termination(handle_, onTerminationCallback, this);
        }
        if (errorCallback_) {
            obsidian_macos_process_set_on_error(handle_, onErrorCallback, this);
        }
        return true;
    }
    return false;
}

bool Process::start() {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_process_start(handle_);
}

void Process::setOnStdout(std::function<void(const std::string&)> callback) {
    stdoutCallback_ = callback;
    if (handle_) {
        obsidian_macos_process_set_on_stdout(handle_, onStdoutCallback, this);
    }
}

void Process::setOnStderr(std::function<void(const std::string&)> callback) {
    stderrCallback_ = callback;
    if (handle_) {
        obsidian_macos_process_set_on_stderr(handle_, onStderrCallback, this);
    }
}

void Process::setOnTermination(std::function<void(int)> callback) {
    terminationCallback_ = callback;
    if (handle_) {
        obsidian_macos_process_set_on_termination(handle_, onTerminationCallback, this);
    }
}

void Process::setOnError(std::function<void(const std::string&)> callback) {
    errorCallback_ = callback;
    if (handle_) {
        obsidian_macos_process_set_on_error(handle_, onErrorCallback, this);
    }
}

void Process::terminate() {
    if (handle_) {
        obsidian_macos_process_terminate(handle_);
    }
}

bool Process::isRunning() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_process_is_running(handle_);
}

int Process::waitUntilExit() {
    if (!handle_) {
        return -1;
    }
    return obsidian_macos_process_wait_until_exit(handle_);
}

int Process::getExitCode() const {
    if (!handle_) {
        return -1;
    }
    return obsidian_macos_process_get_exit_code(handle_);
}

int32_t Process::getPid() const {
    if (!handle_) {
        return -1;
    }
    return obsidian_macos_process_get_pid(handle_);
}

bool Process::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_process_is_valid(handle_);
}

Process::Process(Process&& other) noexcept 
    : handle_(other.handle_),
      stdoutCallback_(std::move(other.stdoutCallback_)),
      stderrCallback_(std::move(other.stderrCallback_)),
      terminationCallback_(std::move(other.terminationCallback_)),
      errorCallback_(std::move(other.errorCallback_)) {
    other.handle_ = nullptr;
    // Update callback userData to point to this instance
    if (handle_) {
        if (stdoutCallback_) {
            obsidian_macos_process_set_on_stdout(handle_, onStdoutCallback, this);
        }
        if (stderrCallback_) {
            obsidian_macos_process_set_on_stderr(handle_, onStderrCallback, this);
        }
        if (terminationCallback_) {
            obsidian_macos_process_set_on_termination(handle_, onTerminationCallback, this);
        }
        if (errorCallback_) {
            obsidian_macos_process_set_on_error(handle_, onErrorCallback, this);
        }
    }
}

Process& Process::operator=(Process&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            obsidian_macos_destroy_process(handle_);
        }
        handle_ = other.handle_;
        stdoutCallback_ = std::move(other.stdoutCallback_);
        stderrCallback_ = std::move(other.stderrCallback_);
        terminationCallback_ = std::move(other.terminationCallback_);
        errorCallback_ = std::move(other.errorCallback_);
        other.handle_ = nullptr;
        // Update callback userData to point to this instance
        if (handle_) {
            if (stdoutCallback_) {
                obsidian_macos_process_set_on_stdout(handle_, onStdoutCallback, this);
            }
            if (stderrCallback_) {
                obsidian_macos_process_set_on_stderr(handle_, onStderrCallback, this);
            }
            if (terminationCallback_) {
                obsidian_macos_process_set_on_termination(handle_, onTerminationCallback, this);
            }
            if (errorCallback_) {
                obsidian_macos_process_set_on_error(handle_, onErrorCallback, this);
            }
        }
    }
    return *this;
}

void Process::onStdoutCallback(const char* data, bool isStderr, void* userData) {
    Process* process = static_cast<Process*>(userData);
    if (process && process->stdoutCallback_ && !isStderr) {
        process->stdoutCallback_(std::string(data));
    }
}

void Process::onStderrCallback(const char* data, bool isStderr, void* userData) {
    Process* process = static_cast<Process*>(userData);
    if (process && process->stderrCallback_ && isStderr) {
        process->stderrCallback_(std::string(data));
    }
}

void Process::onTerminationCallback(int exitCode, void* userData) {
    Process* process = static_cast<Process*>(userData);
    if (process && process->terminationCallback_) {
        process->terminationCallback_(exitCode);
    }
}

void Process::onErrorCallback(const char* errorMessage, void* userData) {
    Process* process = static_cast<Process*>(userData);
    if (process && process->errorCallback_) {
        process->errorCallback_(std::string(errorMessage));
    }
}

// ProcessList implementation
std::vector<ProcessInfo> ProcessList::getAllProcesses() {
    std::vector<ProcessInfo> result;
    
    ObsidianProcessList* processList = obsidian_macos_get_all_processes();
    if (!processList) {
        return result;
    }
    
    result.reserve(processList->count);
    
    for (int i = 0; i < processList->count; i++) {
        ProcessInfo info;
        info.processId = processList->processes[i].processId;
        if (processList->processes[i].processName) {
            info.processName = std::string(processList->processes[i].processName);
        }
        if (processList->processes[i].bundleIdentifier) {
            info.bundleIdentifier = std::string(processList->processes[i].bundleIdentifier);
        }
        info.isActive = processList->processes[i].isActive;
        result.push_back(info);
    }
    
    obsidian_macos_process_list_free(processList);
    return result;
}

bool ProcessList::killProcess(int32_t processId) {
    return obsidian_macos_kill_process(processId);
}

bool ProcessList::getProcessInfo(int32_t processId, ProcessInfo& processInfo) {
    ObsidianProcessInfo info;
    if (!obsidian_macos_get_process_info(processId, &info)) {
        return false;
    }
    
    processInfo.processId = info.processId;
    if (info.processName) {
        processInfo.processName = std::string(info.processName);
    }
    if (info.bundleIdentifier) {
        processInfo.bundleIdentifier = std::string(info.bundleIdentifier);
    }
    processInfo.isActive = info.isActive;
    
    // Free the allocated strings
    if (info.processName) {
        free((void*)info.processName);
    }
    if (info.bundleIdentifier) {
        free((void*)info.bundleIdentifier);
    }
    
    return true;
}

} // namespace obsidian::ffi::macos

#endif // __APPLE__

