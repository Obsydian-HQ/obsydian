/**
 * Obsidian Route Registry Helper Implementation
 * 
 * This file provides the definitions for the pending route/layout registration queues.
 * Having these in a .cpp file (not inline in header) ensures there's a single copy
 * of the static storage across the framework and any apps that link against it.
 */

#include "obsidian/route_registry_helper.h"

namespace obsidian {

// Pending route registrations - single definition in the framework
std::vector<PendingRouteRegistration>& getPendingRoutes() {
    static std::vector<PendingRouteRegistration> pendingRoutes;
    return pendingRoutes;
}

// Pending layout registrations - single definition in the framework
std::vector<PendingLayoutRegistration>& getPendingLayouts() {
    static std::vector<PendingLayoutRegistration> pendingLayouts;
    return pendingLayouts;
}

} // namespace obsidian
