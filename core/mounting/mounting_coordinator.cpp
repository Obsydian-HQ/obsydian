/**
 * MountingCoordinator Implementation
 * 
 * Follows React Native's RCTMountingManager pattern
 */

#include "mounting_coordinator.h"
#include <iostream>

namespace obsidian::mounting {

MountingCoordinator& MountingCoordinator::getInstance() {
    static MountingCoordinator instance;
    return instance;
}

void MountingCoordinator::setPlatformDelegate(MountingPlatformDelegate delegate) {
    delegate_ = std::move(delegate);
    hasDelegate_ = true;
}

void MountingCoordinator::performMutations(const ViewMutationList& mutations) {
    for (const auto& mutation : mutations) {
        performMutation(mutation);
    }
}

void MountingCoordinator::performMutation(const ViewMutation& mutation) {
    switch (mutation.type) {
        case ViewMutation::Type::Create:
            handleCreateMutation(mutation);
            break;
        case ViewMutation::Type::Delete:
            handleDeleteMutation(mutation);
            break;
        case ViewMutation::Type::Insert:
            handleInsertMutation(mutation);
            break;
        case ViewMutation::Type::Remove:
            handleRemoveMutation(mutation);
            break;
        case ViewMutation::Type::Update:
            handleUpdateMutation(mutation);
            break;
    }
}

void MountingCoordinator::handleCreateMutation(const ViewMutation& mutation) {
    auto& registry = ComponentViewRegistry::getInstance();
    
    // Dequeue (create or reuse) a view with this tag
    auto descriptor = registry.dequeueComponentView(mutation.componentHandle, mutation.tag);
    
    if (!descriptor.view) {
        std::cerr << "[MountingCoordinator] Failed to create view for tag: " 
                  << mutation.tag << " handle: " << mutation.componentHandle << std::endl;
    }
}

void MountingCoordinator::handleDeleteMutation(const ViewMutation& mutation) {
    auto& registry = ComponentViewRegistry::getInstance();
    
    // Enqueue (return to pool) the view
    registry.enqueueComponentView(mutation.componentHandle, mutation.tag);
}

void MountingCoordinator::handleInsertMutation(const ViewMutation& mutation) {
    if (!hasDelegate_ || !delegate_.mountChildView) {
        std::cerr << "[MountingCoordinator] No delegate for Insert mutation" << std::endl;
        return;
    }
    
    auto& registry = ComponentViewRegistry::getInstance();
    
    void* parentView = registry.findNativeView(mutation.parentTag);
    void* childView = registry.findNativeView(mutation.tag);
    
    if (!parentView || !childView) {
        std::cerr << "[MountingCoordinator] Insert failed - parent or child not found. "
                  << "parentTag: " << mutation.parentTag 
                  << " childTag: " << mutation.tag << std::endl;
        return;
    }
    
    delegate_.mountChildView(parentView, childView, mutation.index);
}

void MountingCoordinator::handleRemoveMutation(const ViewMutation& mutation) {
    if (!hasDelegate_ || !delegate_.unmountChildView) {
        std::cerr << "[MountingCoordinator] No delegate for Remove mutation" << std::endl;
        return;
    }
    
    auto& registry = ComponentViewRegistry::getInstance();
    
    void* parentView = registry.findNativeView(mutation.parentTag);
    void* childView = registry.findNativeView(mutation.tag);
    
    if (!parentView || !childView) {
        std::cerr << "[MountingCoordinator] Remove failed - parent or child not found. "
                  << "parentTag: " << mutation.parentTag 
                  << " childTag: " << mutation.tag << std::endl;
        return;
    }
    
    delegate_.unmountChildView(parentView, childView, mutation.index);
}

void MountingCoordinator::handleUpdateMutation(const ViewMutation& mutation) {
    auto& registry = ComponentViewRegistry::getInstance();
    void* view = registry.findNativeView(mutation.tag);
    
    if (!view) {
        std::cerr << "[MountingCoordinator] Update failed - view not found for tag: " 
                  << mutation.tag << std::endl;
        return;
    }
    
    // Update layout metrics if changed
    if (hasDelegate_ && delegate_.updateLayoutMetrics) {
        if (mutation.oldLayoutMetrics != mutation.newLayoutMetrics) {
            delegate_.updateLayoutMetrics(view, mutation.newLayoutMetrics);
        }
    }
    
    // Update view state if changed (for future use)
    // if (hasDelegate_ && delegate_.updateViewState) {
    //     delegate_.updateViewState(view, mutation.newState);
    // }
}

} // namespace obsidian::mounting
