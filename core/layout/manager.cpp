/**
 * Obsidian Layout Manager Implementation
 */

#include "manager.h"

namespace obsidian::layout {

LayoutManager& LayoutManager::getInstance() {
    static LayoutManager instance;
    return instance;
}

void LayoutManager::setNativeSetFrameFunc(NativeSetFrameFunc func) {
    setFrameFunc_ = func;
}

void LayoutManager::calculateAndApply(LayoutNode* root, float width, float height) {
    if (!root) return;
    
    // Calculate layout
    LayoutEngine::calculateLayout(root, width, height);
    
    // Apply to native views
    applyToNativeViews(root);
}

void LayoutManager::applyToNativeViews(LayoutNode* root) {
    if (!root || !setFrameFunc_) return;
    
    // Apply starting from root (position is relative to parent)
    applyRecursive(root, 0.0f, 0.0f);
}

void LayoutManager::applyRecursive(LayoutNode* node, float parentAbsX, float parentAbsY) {
    const LayoutResult& layout = node->getLayout();
    
    // Apply frame to native view if it exists
    if (node->getNativeView() && setFrameFunc_) {
        setFrameFunc_(node->getNativeView(), 
                      layout.left, layout.top,
                      layout.width, layout.height);
    }
    
    // Calculate absolute position for children
    float absX = parentAbsX + layout.left;
    float absY = parentAbsY + layout.top;
    
    // Recurse to children
    for (auto* child : node->getChildren()) {
        applyRecursive(child, absX, absY);
    }
}

} // namespace obsidian::layout
