/**
 * Obsidian Layout Engine - Style Implementation
 */

#include "style.h"

namespace obsidian::layout {

// Edge to index mapping helpers
static int edgeToIndex(Edge edge, int defaultIdx) {
    switch (edge) {
        case Edge::Left: return 0;
        case Edge::Top: return 1;
        case Edge::Right: return 2;
        case Edge::Bottom: return 3;
        case Edge::Start: return 0;  // Simplified: Start = Left
        case Edge::End: return 2;    // Simplified: End = Right
        default: return defaultIdx;
    }
}

void Style::setMargin(Edge edge, LayoutValue value) {
    if (edge == Edge::All) {
        margin[0] = margin[1] = margin[2] = margin[3] = value;
    } else if (edge == Edge::Horizontal) {
        margin[0] = margin[2] = value;
    } else if (edge == Edge::Vertical) {
        margin[1] = margin[3] = value;
    } else {
        int idx = edgeToIndex(edge, -1);
        if (idx >= 0 && idx < 4) {
            margin[idx] = value;
        }
    }
}

void Style::setPadding(Edge edge, LayoutValue value) {
    if (edge == Edge::All) {
        padding[0] = padding[1] = padding[2] = padding[3] = value;
    } else if (edge == Edge::Horizontal) {
        padding[0] = padding[2] = value;
    } else if (edge == Edge::Vertical) {
        padding[1] = padding[3] = value;
    } else {
        int idx = edgeToIndex(edge, -1);
        if (idx >= 0 && idx < 4) {
            padding[idx] = value;
        }
    }
}

void Style::setPosition(Edge edge, LayoutValue value) {
    int idx = edgeToIndex(edge, -1);
    if (idx >= 0 && idx < 4) {
        position[idx] = value;
    }
}

LayoutValue Style::getMargin(Edge edge) const {
    if (edge == Edge::All || edge == Edge::Horizontal || edge == Edge::Vertical) {
        return margin[0];  // Return first as representative
    }
    int idx = edgeToIndex(edge, 0);
    return margin[idx];
}

LayoutValue Style::getPadding(Edge edge) const {
    if (edge == Edge::All || edge == Edge::Horizontal || edge == Edge::Vertical) {
        return padding[0];
    }
    int idx = edgeToIndex(edge, 0);
    return padding[idx];
}

LayoutValue Style::getPosition(Edge edge) const {
    int idx = edgeToIndex(edge, 0);
    return position[idx];
}

} // namespace obsidian::layout
