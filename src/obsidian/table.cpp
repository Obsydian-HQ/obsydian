/**
 * Obsidian Public API - Table Implementation
 * 
 * This file implements the public Table API by wrapping the internal
 * platform-specific implementations.
 */

#include "obsidian/table.h"
#include "obsidian/window.h"

// Include internal headers (not exposed to users)
#ifdef __APPLE__
// Access through dependency: //packages/apple:apple_ffi exposes macos_ffi.h
#include "macos_ffi.h"
#include "macos_table.h"  // For C FFI function
#endif

namespace obsidian {

class Table::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::Table macosTable;
#endif
    bool valid = false;
};

Table::Table() : pImpl(std::make_unique<Impl>()) {}

Table::~Table() {
    // RAII: Automatically clean up resources
    // The FFI Table destructor will handle proper cleanup (remove from parent + destroy)
    // pImpl will be destroyed automatically, which will destroy the FFI Table
    // The FFI Table destructor calls obsidian_macos_destroy_table which handles all cleanup
    if (pImpl && pImpl->valid) {
        // Mark as invalid to prevent any further operations
        pImpl->valid = false;
    }
}

bool Table::create(int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (!pImpl->macosTable.create(x, y, width, height)) {
        return false;
    }
    pImpl->valid = true;
    return true;
#else
    // Other platforms not yet implemented
    return false;
#endif
}

bool Table::addColumn(const std::string& identifier, const std::string& title, int width) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.addColumn(identifier, title, width);
#else
    return false;
#endif
}

int Table::addRow(const std::vector<std::string>& rowData) {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.addRow(rowData);
#else
    return -1;
#endif
}

bool Table::removeRow(int rowIndex) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.removeRow(rowIndex);
#else
    return false;
#endif
}

void Table::clear() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTable.clear();
#endif
}

int Table::getRowCount() const {
    if (!pImpl->valid) {
        return 0;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.getRowCount();
#else
    return 0;
#endif
}

int Table::getColumnCount() const {
    if (!pImpl->valid) {
        return 0;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.getColumnCount();
#else
    return 0;
#endif
}

bool Table::setRowData(int rowIndex, const std::vector<std::string>& rowData) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.setRowData(rowIndex, rowData);
#else
    return false;
#endif
}

std::string Table::getRowData(int rowIndex, int columnIndex) const {
    if (!pImpl->valid) {
        return std::string();
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.getRowData(rowIndex, columnIndex);
#else
    return std::string();
#endif
}

void Table::setOnSelection(std::function<void(int rowIndex)> callback) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTable.setOnSelection(callback);
#endif
}

int Table::getSelectedRow() const {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.getSelectedRow();
#else
    return -1;
#endif
}

bool Table::setSelectedRow(int rowIndex) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.setSelectedRow(rowIndex);
#else
    return false;
#endif
}

void Table::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTable.setVisible(visible);
#endif
}

bool Table::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.isVisible();
#else
    return false;
#endif
}

void Table::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTable.setEnabled(enabled);
#endif
}

bool Table::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosTable.isEnabled();
#else
    return false;
#endif
}

void Table::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both list and window
    void* listHandle = pImpl->macosTable.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!listHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_table_add_to_window(listHandle, windowHandle);
#endif
}

void Table::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosTable.removeFromParent();
    // Note: Table remains valid after removal - it's just not attached to a parent
    // The list will be destroyed when the destructor runs
#endif
}

bool Table::isValid() const {
    return pImpl->valid;
}

Table::Table(Table&& other) noexcept = default;
Table& Table::operator=(Table&& other) noexcept = default;

} // namespace obsidian

