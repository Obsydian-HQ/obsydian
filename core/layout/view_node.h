/**
 * Obsidian Layout Engine - View Node
 * 
 * A LayoutNode that is associated with a native view.
 * Provides convenience methods for UI component integration.
 */

#pragma once

#include "node.h"
#include "engine.h"

namespace obsidian::layout {

/**
 * ViewNode - A LayoutNode that knows about its native view
 * 
 * This is used by UI components (VStack, HStack, etc.) to 
 * integrate with the layout engine.
 */
class ViewNode : public LayoutNode {
public:
    ViewNode() = default;
    
    /**
     * Create a ViewNode for a container (VStack/HStack/ZStack)
     */
    static ViewNode* createContainer(FlexDirection direction, void* nativeView);
    
    /**
     * Create a ViewNode for a spacer
     */
    static ViewNode* createSpacer(void* nativeView);
    
    /**
     * Create a ViewNode for a leaf (Text, Button, etc.)
     * Requires a measure function
     */
    static ViewNode* createLeaf(void* nativeView, MeasureFunc measureFunc);
    
    /**
     * Configure as VStack (Column)
     */
    void configureAsVStack(float spacing, float paddingTop, float paddingBottom, 
                           float paddingLeading, float paddingTrailing);
    
    /**
     * Configure as HStack (Row)
     */
    void configureAsHStack(float spacing, float paddingTop, float paddingBottom,
                           float paddingLeading, float paddingTrailing);
    
    /**
     * Configure as Spacer (flexGrow = 1)
     */
    void configureAsSpacer(float minWidth = 0, float minHeight = 0);
    
    /**
     * Calculate layout and apply to native views
     * Call this on the root node
     */
    void layoutAndApply(float width, float height);
    
    /**
     * Set a callback for when frame needs to be set on native view
     */
    using SetFrameCallback = void(*)(void* view, float x, float y, float w, float h);
    static void setSetFrameCallback(SetFrameCallback callback);

private:
    static SetFrameCallback setFrameCallback_;
};

} // namespace obsidian::layout
