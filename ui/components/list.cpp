#include "obsidian/ui/list.h"
#include "obsidian/window.h"

#ifdef __APPLE__
#include "macos_ffi.h"
#include "macos_list.h"
#endif

namespace obsidian {

class List::Impl {
public:
#ifdef __APPLE__
    obsidian::ffi::macos::List macosList;
#endif
    bool valid = false;
};

List::List() : pImpl(std::make_unique<Impl>()) {}

List::~List() {
    if (pImpl->valid) {
        removeFromParent();
    }
}

bool List::create(int x, int y, int width, int height) {
    if (pImpl->valid) {
        return false; // Already created
    }
    
#ifdef __APPLE__
    if (pImpl->macosList.create(x, y, width, height)) {
        pImpl->valid = true;
        return true;
    }
#endif
    
    return false;
}

int List::addItem(const std::string& itemText) {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.addItem(itemText);
#endif
    
    return -1;
}

bool List::removeItem(int itemIndex) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.removeItem(itemIndex);
#endif
    
    return false;
}

void List::clear() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosList.clear();
#endif
}

int List::getItemCount() const {
    if (!pImpl->valid) {
        return 0;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.getItemCount();
#endif
    
    return 0;
}

std::string List::getItem(int itemIndex) const {
    if (!pImpl->valid) {
        return "";
    }
    
#ifdef __APPLE__
    return pImpl->macosList.getItem(itemIndex);
#endif
    
    return "";
}

bool List::setItem(int itemIndex, const std::string& itemText) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.setItem(itemIndex, itemText);
#endif
    
    return false;
}

void List::setOnSelection(std::function<void(int itemIndex)> callback) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosList.setOnSelection(callback);
#endif
}

int List::getSelectedIndex() const {
    if (!pImpl->valid) {
        return -1;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.getSelectedIndex();
#endif
    
    return -1;
}

bool List::setSelectedIndex(int itemIndex) {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.setSelectedIndex(itemIndex);
#endif
    
    return false;
}

void List::setVisible(bool visible) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosList.setVisible(visible);
#endif
}

bool List::isVisible() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.isVisible();
#endif
    
    return false;
}

void List::setEnabled(bool enabled) {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosList.setEnabled(enabled);
#endif
}

bool List::isEnabled() const {
    if (!pImpl->valid) {
        return false;
    }
    
#ifdef __APPLE__
    return pImpl->macosList.isEnabled();
#endif
    
    return false;
}

void List::addToWindow(Window& window) {
    if (!pImpl->valid || !window.isValid()) {
        return;
    }
    
#ifdef __APPLE__
    // Get the native handles from both list and window
    void* listHandle = pImpl->macosList.getHandle();
    void* windowHandle = window.getNativeHandle();
    
    if (!listHandle || !windowHandle) {
        return;
    }
    
    // Call the C FFI function directly
    obsidian_macos_list_add_to_window(listHandle, windowHandle);
#endif
}

void List::removeFromParent() {
    if (!pImpl->valid) {
        return;
    }
    
#ifdef __APPLE__
    pImpl->macosList.removeFromParent();
    // Note: List remains valid after removal - it's just not attached to a parent
#endif
}

bool List::isValid() const {
    return pImpl->valid;
}

void* List::getNativeViewHandle() const {
    if (!pImpl || !pImpl->valid) {
        return nullptr;
    }
    
#ifdef __APPLE__
    void* listHandle = pImpl->macosList.getHandle();
    if (listHandle) {
        return obsidian_macos_list_get_view(listHandle);
    }
#endif
    return nullptr;
}

} // namespace obsidian

