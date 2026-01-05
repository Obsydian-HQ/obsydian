/**
 * Layout Alignment Utilities
 * 
 * Provides alignment enumeration and utilities for layout system
 */

#pragma once

namespace obsidian::layout {

/**
 * Alignment enumeration
 * Supports horizontal, vertical, and combined alignments
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
