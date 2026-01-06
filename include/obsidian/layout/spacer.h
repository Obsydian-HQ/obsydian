/**
 * Obsidian Public API - Spacer
 * 
 * Flexible spacing component that expands to fill available space
 * in layout containers (VStack, HStack). Supports min/max size constraints.
 */

#pragma once

#include <cstdint>
#include <memory>

namespace obsidian {

// Forward declarations
class Window;

/**
 * Spacer class - Flexible spacing component
 * 
 * Creates a flexible view that expands to fill available space in layout containers.
 * Uses NSLayoutConstraint with low content hugging and compression resistance priorities
 * to allow flexible sizing. Supports optional min/max size constraints.
 */
class Spacer {
public:
    /**
     * Create a new Spacer
     */
    Spacer();
    
    /**
     * Destructor - automatically removes and destroys the Spacer
     */
    ~Spacer();
    
    /**
     * Create a spacer with optional minimum and maximum sizes
     * @param minWidth Minimum width in pixels (0.0 for no minimum)
     * @param minHeight Minimum height in pixels (0.0 for no minimum)
     * @param maxWidth Maximum width in pixels (0.0 for no maximum, use very large value)
     * @param maxHeight Maximum height in pixels (0.0 for no maximum, use very large value)
     * @return true if spacer was created successfully
     */
    bool create(double minWidth = 0.0, double minHeight = 0.0, 
                double maxWidth = 0.0, double maxHeight = 0.0);
    
    /**
     * Set minimum width constraint
     * @param minWidth Minimum width in pixels (0.0 to remove constraint)
     */
    void setMinWidth(double minWidth);
    
    /**
     * Get minimum width
     */
    double getMinWidth() const;
    
    /**
     * Set minimum height constraint
     * @param minHeight Minimum height in pixels (0.0 to remove constraint)
     */
    void setMinHeight(double minHeight);
    
    /**
     * Get minimum height
     */
    double getMinHeight() const;
    
    /**
     * Set maximum width constraint
     * @param maxWidth Maximum width in pixels (0.0 to remove constraint)
     */
    void setMaxWidth(double maxWidth);
    
    /**
     * Get maximum width
     */
    double getMaxWidth() const;
    
    /**
     * Set maximum height constraint
     * @param maxHeight Maximum height in pixels (0.0 to remove constraint)
     */
    void setMaxHeight(double maxHeight);
    
    /**
     * Get maximum height
     */
    double getMaxHeight() const;
    
    /**
     * Check if spacer is valid
     */
    bool isValid() const;
    
    /**
     * Get the native view handle (for internal use by layout system)
     * @return Platform-specific native view handle, or nullptr if invalid
     */
    void* getNativeViewHandle() const;
    
    // Non-copyable
    Spacer(const Spacer&) = delete;
    Spacer& operator=(const Spacer&) = delete;
    
    // Movable
    Spacer(Spacer&&) noexcept;
    Spacer& operator=(Spacer&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian
