/**
 * macOS Layout FFI - C++ Implementation
 * 
 * C++ wrapper around the Objective-C++ bridge
 */

#include "macos_layout_ffi.h"

#ifdef __APPLE__

#include "macos_layout.h"
#include <algorithm>
#include <vector>

namespace obsidian::ffi::macos {

LayoutConstraint::LayoutConstraint() 
    : handle_(nullptr)
    , active_(false)
{
}

LayoutConstraint::~LayoutConstraint() {
    destroy();
}

bool LayoutConstraint::create(const ObsidianLayoutConstraintParams& params) {
    if (handle_) {
        return false; // Already created
    }
    
    handle_ = obsidian_macos_create_constraint(params);
    if (!handle_) {
        return false;
    }
    
    active_ = false; // Constraints are created inactive
    return true;
}

void LayoutConstraint::activate() {
    if (!handle_ || active_) {
        return;
    }
    
    obsidian_macos_activate_constraints(&handle_, 1);
    active_ = true;
}

void LayoutConstraint::deactivate() {
    if (!handle_ || !active_) {
        return;
    }
    
    obsidian_macos_deactivate_constraints(&handle_, 1);
    active_ = false;
}

void LayoutConstraint::destroy() {
    if (handle_) {
        if (active_) {
            deactivate();
        }
        obsidian_macos_destroy_constraint(handle_);
        handle_ = nullptr;
        active_ = false;
    }
}

bool LayoutConstraint::isValid() const {
    if (!handle_) {
        return false;
    }
    return obsidian_macos_constraint_is_valid(handle_);
}

LayoutConstraint::LayoutConstraint(LayoutConstraint&& other) noexcept
    : handle_(other.handle_)
    , active_(other.active_)
{
    other.handle_ = nullptr;
    other.active_ = false;
}

LayoutConstraint& LayoutConstraint::operator=(LayoutConstraint&& other) noexcept {
    if (this != &other) {
        destroy();
        handle_ = other.handle_;
        active_ = other.active_;
        other.handle_ = nullptr;
        other.active_ = false;
    }
    return *this;
}

void activateConstraints(const std::vector<LayoutConstraint*>& constraints) {
    if (constraints.empty()) {
        return;
    }
    
    std::vector<ObsidianLayoutConstraintHandle> handles;
    handles.reserve(constraints.size());
    
    for (LayoutConstraint* constraint : constraints) {
        if (constraint && constraint->isValid()) {
            handles.push_back(constraint->getHandle());
        }
    }
    
    if (!handles.empty()) {
        obsidian_macos_activate_constraints(handles.data(), handles.size());
    }
}

void deactivateConstraints(const std::vector<LayoutConstraint*>& constraints) {
    if (constraints.empty()) {
        return;
    }
    
    std::vector<ObsidianLayoutConstraintHandle> handles;
    handles.reserve(constraints.size());
    
    for (LayoutConstraint* constraint : constraints) {
        if (constraint && constraint->isValid()) {
            handles.push_back(constraint->getHandle());
        }
    }
    
    if (!handles.empty()) {
        obsidian_macos_deactivate_constraints(handles.data(), handles.size());
    }
}

} // namespace obsidian::ffi::macos

#endif // __APPLE__
