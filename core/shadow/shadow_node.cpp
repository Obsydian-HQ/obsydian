/**
 * Obsidian Shadow Tree - Shadow Node Implementation
 */

#include "shadow_node.h"
#include <algorithm>

namespace obsidian::shadow {

ShadowNode::ShadowNode(ShadowTag tag, ComponentType type)
    : tag_(tag)
    , componentType_(type)
    , layoutNode_(std::make_unique<layout::LayoutNode>())
{
    // Configure layout node based on component type
    auto& style = layoutNode_->getStyle();
    
    switch (type) {
        case ComponentType::VStack:
        case ComponentType::Root:
            style.flexDirection = layout::FlexDirection::Column;
            break;
            
        case ComponentType::HStack:
            style.flexDirection = layout::FlexDirection::Row;
            break;
            
        case ComponentType::ZStack:
            style.flexDirection = layout::FlexDirection::Column;
            style.positionType = layout::PositionType::Absolute;
            break;
            
        case ComponentType::Spacer:
            // Spacer should grow to fill available space
            style.flexGrow = 1.0f;
            break;
            
        default:
            // Default for leaf nodes
            break;
    }
}

ShadowNode::~ShadowNode() {
    // Remove from parent
    if (parent_) {
        parent_->removeChild(this);
    }
    
    // Clear children's parent reference
    for (auto* child : children_) {
        child->parent_ = nullptr;
    }
}

void ShadowNode::setLayoutMetrics(const LayoutMetrics& metrics) {
    previousLayoutMetrics_ = layoutMetrics_;
    layoutMetrics_ = metrics;
    layoutMetricsChanged_ = (layoutMetrics_ != previousLayoutMetrics_);
}

void ShadowNode::addChild(ShadowNode* child) {
    if (!child || child->parent_ == this) {
        return;
    }
    
    // Remove from previous parent
    if (child->parent_) {
        child->parent_->removeChild(child);
    }
    
    // Add to this node
    children_.push_back(child);
    child->parent_ = this;
    
    // Add to layout tree
    layoutNode_->addChild(child->layoutNode_.get());
    
    markDirty();
}

void ShadowNode::insertChild(ShadowNode* child, size_t index) {
    if (!child || child->parent_ == this) {
        return;
    }
    
    // Remove from previous parent
    if (child->parent_) {
        child->parent_->removeChild(child);
    }
    
    // Insert at position
    if (index >= children_.size()) {
        children_.push_back(child);
    } else {
        children_.insert(children_.begin() + index, child);
    }
    child->parent_ = this;
    
    // Insert into layout tree
    layoutNode_->insertChild(child->layoutNode_.get(), index);
    
    markDirty();
}

void ShadowNode::removeChild(ShadowNode* child) {
    if (!child) {
        return;
    }
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
        child->parent_ = nullptr;
        
        // Remove from layout tree
        layoutNode_->removeChild(child->layoutNode_.get());
        
        markDirty();
    }
}

void ShadowNode::removeAllChildren() {
    for (auto* child : children_) {
        child->parent_ = nullptr;
    }
    children_.clear();
    layoutNode_->removeAllChildren();
    markDirty();
}

size_t ShadowNode::getChildCount() const {
    return children_.size();
}

ShadowNode* ShadowNode::getChild(size_t index) const {
    if (index >= children_.size()) {
        return nullptr;
    }
    return children_[index];
}

void ShadowNode::markDirty() {
    if (isDirty_) {
        return;  // Already dirty
    }
    
    isDirty_ = true;
    layoutNode_->markDirty();
    
    // Propagate to parent
    if (parent_) {
        parent_->markDirty();
    }
}

void ShadowNode::updateFromLayoutResult() {
    const auto& result = layoutNode_->getLayout();
    
    LayoutMetrics newMetrics;
    newMetrics.x = result.left;
    newMetrics.y = result.top;
    newMetrics.width = result.width;
    newMetrics.height = result.height;
    newMetrics.paddingLeft = result.paddingLeft;
    newMetrics.paddingTop = result.paddingTop;
    newMetrics.paddingRight = result.paddingRight;
    newMetrics.paddingBottom = result.paddingBottom;
    
    setLayoutMetrics(newMetrics);
    clearDirty();
    
    // Update children
    for (auto* child : children_) {
        child->updateFromLayoutResult();
    }
}

} // namespace obsidian::shadow
