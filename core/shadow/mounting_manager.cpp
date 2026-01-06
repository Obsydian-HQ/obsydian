/**
 * Obsidian Mounting Manager Implementation
 */

#include "mounting_manager.h"
#include <iostream>

namespace obsidian::shadow {

MountingManager& MountingManager::getInstance() {
    static MountingManager instance;
    return instance;
}

void MountingManager::setSetFrameFunc(SetFrameFunc func) {
    setFrameFunc_ = std::move(func);
}

void MountingManager::applyMutations(const MutationList& mutations) {
    for (const auto& mutation : mutations) {
        applyMutation(mutation);
    }
}

void MountingManager::applyMutation(const ViewMutation& mutation) {
    switch (mutation.type) {
        case MutationType::Create:
            // Native view creation is handled by UI components
            // This mutation is informational
            break;
            
        case MutationType::Delete:
            // Native view deletion is handled by UI components
            // This mutation is informational
            break;
            
        case MutationType::Insert:
            // Native view insertion is handled by UI components
            // This mutation is informational
            break;
            
        case MutationType::Remove:
            // Native view removal is handled by UI components
            // This mutation is informational
            break;
            
        case MutationType::Update:
            // Apply layout metrics to native view
            if (mutation.nativeView && setFrameFunc_) {
                const auto& metrics = mutation.layoutMetrics;
                setFrameFunc_(
                    mutation.nativeView,
                    metrics.x,
                    metrics.y,
                    metrics.width,
                    metrics.height
                );
            }
            break;
    }
}

} // namespace obsidian::shadow
