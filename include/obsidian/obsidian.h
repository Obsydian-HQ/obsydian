/**
 * Obsidian Public API - Main Entry Point
 * 
 * This is the main public API header for the Obsidian framework.
 * Include this header to use Obsidian in your application.
 * 
 * Example usage:
 * 
 *     #include <obsidian/obsidian.h>
 *     using namespace obsidian;
 *     
 *     int main() {
 *         App app;
 *         app.initialize();
 *         Window window;
 *         window.create(800, 600, "Hello World");
 *         window.show();
 *         AppCallbacks callbacks;
 *         callbacks.onInit = []() { };
 *         app.run(callbacks);
 *         return 0;
 *     }
 */

#pragma once

// ============================================================================
// Core
// ============================================================================
#include "obsidian/app.h"
#include "obsidian/window.h"

// ============================================================================
// UI Components
// ============================================================================
#include "obsidian/ui/button.h"
#include "obsidian/ui/textfield.h"
#include "obsidian/ui/textview.h"
#include "obsidian/ui/scrollview.h"
#include "obsidian/ui/table.h"
#include "obsidian/ui/list.h"
#include "obsidian/ui/link.h"

// ============================================================================
// Layout Components
// ============================================================================
#include "obsidian/layout/vstack.h"
#include "obsidian/layout/hstack.h"
#include "obsidian/layout/zstack.h"
#include "obsidian/layout/spacer.h"
#include "obsidian/layout/splitview.h"
#include "obsidian/layout/sidebar.h"

// ============================================================================
// Navigation & Routing
// ============================================================================
#include "obsidian/navigation/router.h"
#include "obsidian/navigation/screen_container.h"
#include "obsidian/navigation/route_registry_helper.h"

// ============================================================================
// System APIs
// ============================================================================
#include "obsidian/system/process.h"
#include "obsidian/system/process_list.h"

/**
 * @namespace obsidian
 * 
 * The Obsidian namespace contains all public API classes and functions.
 * 
 * Main classes:
 * - obsidian::App - Application lifecycle management
 * - obsidian::Window - Window creation and management
 * - obsidian::AppCallbacks - Application lifecycle callbacks
 * 
 * UI Components:
 * - obsidian::Button - Clickable button
 * - obsidian::TextField - Text input field
 * - obsidian::TextView - Text display
 * - obsidian::ScrollView - Scrollable container
 * - obsidian::Table - Data table
 * - obsidian::List - List view
 * - obsidian::Link - Navigation link
 * 
 * Layout Components:
 * - obsidian::VStack - Vertical stack layout
 * - obsidian::HStack - Horizontal stack layout
 * - obsidian::ZStack - Z-axis stack layout
 * - obsidian::Spacer - Flexible space
 * - obsidian::SplitView - Split view container
 * - obsidian::Sidebar - Sidebar navigation
 * 
 * Navigation:
 * - obsidian::Router - File-based routing
 * - obsidian::ScreenContainer - Screen container for routing
 * - REGISTER_ROUTE macro - Route registration helper
 * 
 * System APIs:
 * - obsidian::Process - Process execution
 * - obsidian::ProcessList - Process management
 */

