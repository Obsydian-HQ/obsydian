/**
 * ViewMutation - Describes mutations to the native view tree
 * 
 * Following React Native's ShadowViewMutation pattern:
 * - Create: Create a new native view
 * - Delete: Delete a native view (return to pool)
 * - Insert: Insert a child into a parent at an index
 * - Remove: Remove a child from a parent
 * - Update: Update a view's props/layout
 */

#pragma once

#include "component_view_registry.h"
#include <vector>
#include <string>

namespace obsidian::mounting {

/**
 * Layout metrics computed by the layout engine
 * Applied to native views during Update mutations
 */
struct LayoutMetrics {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    
    bool operator==(const LayoutMetrics& other) const {
        return x == other.x && y == other.y && 
               width == other.width && height == other.height;
    }
    
    bool operator!=(const LayoutMetrics& other) const {
        return !(*this == other);
    }
};

/**
 * View state that can be updated
 */
struct ViewState {
    bool isHidden = false;
    float opacity = 1.0f;
    // Add more state as needed
};

/**
 * Describes a single mutation to the native view tree
 */
struct ViewMutation {
    enum class Type : uint8_t {
        Create = 1,
        Delete = 2,
        Insert = 4,
        Remove = 8,
        Update = 16,
    };
    
    Type type;
    
    // For Create/Delete: the view being created/deleted
    Tag tag = 0;
    ComponentHandle componentHandle;
    
    // For Insert/Remove: parent and index
    Tag parentTag = 0;
    int32_t index = -1;
    
    // For Update: old and new metrics/state
    LayoutMetrics oldLayoutMetrics;
    LayoutMetrics newLayoutMetrics;
    ViewState oldState;
    ViewState newState;
    
    // Static factory methods (following React Native pattern)
    
    static ViewMutation CreateMutation(Tag tag, const ComponentHandle& handle) {
        ViewMutation m;
        m.type = Type::Create;
        m.tag = tag;
        m.componentHandle = handle;
        return m;
    }
    
    static ViewMutation DeleteMutation(Tag tag, const ComponentHandle& handle) {
        ViewMutation m;
        m.type = Type::Delete;
        m.tag = tag;
        m.componentHandle = handle;
        return m;
    }
    
    static ViewMutation InsertMutation(Tag parentTag, Tag childTag, int32_t index) {
        ViewMutation m;
        m.type = Type::Insert;
        m.parentTag = parentTag;
        m.tag = childTag;
        m.index = index;
        return m;
    }
    
    static ViewMutation RemoveMutation(Tag parentTag, Tag childTag, int32_t index) {
        ViewMutation m;
        m.type = Type::Remove;
        m.parentTag = parentTag;
        m.tag = childTag;
        m.index = index;
        return m;
    }
    
    static ViewMutation UpdateMutation(Tag tag, 
                                       const LayoutMetrics& oldMetrics,
                                       const LayoutMetrics& newMetrics) {
        ViewMutation m;
        m.type = Type::Update;
        m.tag = tag;
        m.oldLayoutMetrics = oldMetrics;
        m.newLayoutMetrics = newMetrics;
        return m;
    }
    
    static ViewMutation UpdateMutation(Tag tag,
                                       const LayoutMetrics& oldMetrics,
                                       const LayoutMetrics& newMetrics,
                                       const ViewState& oldState,
                                       const ViewState& newState) {
        ViewMutation m;
        m.type = Type::Update;
        m.tag = tag;
        m.oldLayoutMetrics = oldMetrics;
        m.newLayoutMetrics = newMetrics;
        m.oldState = oldState;
        m.newState = newState;
        return m;
    }
};

using ViewMutationList = std::vector<ViewMutation>;

/**
 * Debug string for mutation type
 */
inline const char* getMutationTypeName(ViewMutation::Type type) {
    switch (type) {
        case ViewMutation::Type::Create: return "Create";
        case ViewMutation::Type::Delete: return "Delete";
        case ViewMutation::Type::Insert: return "Insert";
        case ViewMutation::Type::Remove: return "Remove";
        case ViewMutation::Type::Update: return "Update";
        default: return "Unknown";
    }
}

} // namespace obsidian::mounting
