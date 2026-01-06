/**
 * Layout Alignment Types
 * 
 * Basic alignment enumerations for the layout system.
 * This file is kept minimal - alignment concepts are universal.
 */

#pragma once

namespace obsidian::layout {

/**
 * Alignment enumeration
 * Used for positioning children within containers
 */
enum class Alignment {
    // Horizontal
    Leading,    // Left (LTR) / Right (RTL)
    Center,     // Center horizontally
    Trailing,   // Right (LTR) / Left (RTL)
    
    // Vertical
    Top,        // Top edge
    Middle,     // Center vertically
    Bottom,     // Bottom edge
    
    // Combined (for ZStack and complex layouts)
    TopLeading,
    TopCenter,
    TopTrailing,
    MiddleLeading,
    MiddleCenter,
    MiddleTrailing,
    BottomLeading,
    BottomCenter,
    BottomTrailing
};

} // namespace obsidian::layout
