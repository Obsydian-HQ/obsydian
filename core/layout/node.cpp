/**
 * Obsidian Layout Engine - Layout Node Implementation
 */

#include "node.h"
#include "engine.h"
#include <algorithm>
#include <iostream>

namespace obsidian::layout {

LayoutNode::LayoutNode() = default;
LayoutNode::~LayoutNode() {
    // Don't delete children - we don't own them
    if (parent_) {
        parent_->removeChild(this);
    }
}

void LayoutNode::addChild(LayoutNode* child) {
    if (!child) return;
    
    // Remove from old parent
    if (child->parent_) {
        child->parent_->removeChild(child);
    }
    
    children_.push_back(child);
    child->parent_ = this;
    markDirty();
}

void LayoutNode::insertChild(LayoutNode* child, size_t index) {
    if (!child) return;
    
    // Remove from old parent
    if (child->parent_) {
        child->parent_->removeChild(child);
    }
    
    index = std::min(index, children_.size());
    children_.insert(children_.begin() + index, child);
    child->parent_ = this;
    markDirty();
}

void LayoutNode::removeChild(LayoutNode* child) {
    if (!child) return;
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        child->parent_ = nullptr;
        markDirty();
    }
}

void LayoutNode::removeAllChildren() {
    for (auto* child : children_) {
        child->parent_ = nullptr;
    }
    children_.clear();
    markDirty();
}

LayoutNode* LayoutNode::getChild(size_t index) const {
    if (index >= children_.size()) return nullptr;
    return children_[index];
}

void LayoutNode::setMeasureFunc(MeasureFunc func) {
    measureFunc_ = std::move(func);
    markDirty();
}

void LayoutNode::markDirty() {
    isDirty_ = true;
    // Propagate to parent
    if (parent_) {
        parent_->markDirty();
    }
}

void LayoutNode::calculateLayout(float availableWidth, float availableHeight) {
    LayoutEngine::calculateLayout(this, availableWidth, availableHeight);
    isDirty_ = false;
}

Size LayoutNode::measure(float width, MeasureMode widthMode,
                         float height, MeasureMode heightMode) {
    if (measureFunc_) {
        return measureFunc_(width, widthMode, height, heightMode);
    }
    
    // Default: return 0x0 for nodes without measure function
    return {0.0f, 0.0f};
}

void LayoutNode::printTree(int indent) const {
    std::string prefix(indent * 2, ' ');
    
    std::cout << prefix << "Node: "
              << "pos=(" << layout_.left << "," << layout_.top << ") "
              << "size=(" << layout_.width << "x" << layout_.height << ")"
              << std::endl;
    
    for (const auto* child : children_) {
        child->printTree(indent + 1);
    }
}

} // namespace obsidian::layout
