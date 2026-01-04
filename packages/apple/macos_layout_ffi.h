/**
 * macOS Layout FFI - C++ Interface
 * 
 * Provides C++ interface for macOS layout constraints
 * This is the interface that the layout system uses
 */

#pragma once

#ifdef __APPLE__

#include <memory>
#include <vector>
#include "macos_layout.h"

namespace obsidian::ffi::macos {

/**
 * Layout constraint handle (RAII wrapper)
 */
class LayoutConstraint {
public:
    LayoutConstraint();
    ~LayoutConstraint();
    
    /**
     * Create a constraint with the given parameters
     * @return true if constraint was created successfully
     */
    bool create(const ObsidianLayoutConstraintParams& params);
    
    /**
     * Activate this constraint
     */
    void activate();
    
    /**
     * Deactivate this constraint
     */
    void deactivate();
    
    /**
     * Destroy the constraint
     */
    void destroy();
    
    /**
     * Check if constraint is valid
     */
    bool isValid() const;
    
    /**
     * Get the internal constraint handle (for internal use)
     */
    ObsidianLayoutConstraintHandle getHandle() const { return handle_; }
    
    // Non-copyable
    LayoutConstraint(const LayoutConstraint&) = delete;
    LayoutConstraint& operator=(const LayoutConstraint&) = delete;
    
    // Movable
    LayoutConstraint(LayoutConstraint&&) noexcept;
    LayoutConstraint& operator=(LayoutConstraint&&) noexcept;

private:
    ObsidianLayoutConstraintHandle handle_;  // Opaque handle to native constraint
    bool active_;  // Track activation state
};

/**
 * Batch activate constraints
 */
void activateConstraints(const std::vector<LayoutConstraint*>& constraints);

/**
 * Batch deactivate constraints
 */
void deactivateConstraints(const std::vector<LayoutConstraint*>& constraints);

} // namespace obsidian::ffi::macos

#endif // __APPLE__
