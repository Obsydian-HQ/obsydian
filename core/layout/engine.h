/**
 * Obsidian Layout Engine - Core Algorithm
 * 
 * The layout engine computes positions and sizes for all nodes
 * in a layout tree based on their styles.
 * 
 * Inspired by Yoga but simplified for our needs.
 */

#pragma once

#include "node.h"

namespace obsidian::layout {

/**
 * Layout Engine
 * 
 * Computes layout for a tree of LayoutNodes.
 * This is the main entry point for layout calculation.
 */
class LayoutEngine {
public:
    /**
     * Calculate layout for a node tree
     * 
     * @param root The root node of the tree
     * @param availableWidth Available width for the root
     * @param availableHeight Available height for the root
     */
    static void calculateLayout(LayoutNode* root, 
                                float availableWidth, 
                                float availableHeight);
    
    /**
     * Apply computed layout to native views
     * 
     * After calculateLayout, call this to apply the results
     * to the native view hierarchy.
     * 
     * @param root The root node (must have calculateLayout called first)
     * @param setFrameFunc Function to set frame on native view
     */
    using SetFrameFunc = void(*)(void* nativeView, 
                                  float x, float y, 
                                  float width, float height);
    
    static void applyLayout(LayoutNode* root, SetFrameFunc setFrameFunc);
    
private:
    // Internal layout algorithm
    static void layoutNode(LayoutNode* node, 
                          float availableWidth, MeasureMode widthMode,
                          float availableHeight, MeasureMode heightMode);
    
    // Layout for flex containers
    static void layoutFlexContainer(LayoutNode* node,
                                    float availableWidth, MeasureMode widthMode,
                                    float availableHeight, MeasureMode heightMode);
    
    // Layout for absolute positioned nodes
    static void layoutAbsoluteChildren(LayoutNode* node);
    
    // Resolve size constraints
    static float resolveWidth(LayoutNode* node, float parentWidth);
    static float resolveHeight(LayoutNode* node, float parentHeight);
    
    // Apply layout recursively to native views
    static void applyLayoutRecursive(LayoutNode* node, 
                                     SetFrameFunc setFrameFunc,
                                     float parentAbsX, 
                                     float parentAbsY);
};

} // namespace obsidian::layout
