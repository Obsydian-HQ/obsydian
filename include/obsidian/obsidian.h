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

// Include all public API headers
#include "obsidian/app.h"
#include "obsidian/window.h"
#include "obsidian/button.h"
#include "obsidian/textfield.h"
#include "obsidian/textview.h"
#include "obsidian/scrollview.h"
#include "obsidian/table.h"
#include "obsidian/list.h"
#include "obsidian/process.h"
#include "obsidian/process_list.h"
#include "obsidian/vstack.h"
#include "obsidian/hstack.h"
#include "obsidian/zstack.h"
#include "obsidian/spacer.h"
#include "obsidian/splitview.h"
#include "obsidian/sidebar.h"

/**
 * @namespace obsidian
 * 
 * The Obsidian namespace contains all public API classes and functions.
 * 
 * Main classes:
 * - obsidian::App - Application lifecycle management
 * - obsidian::Window - Window creation and management
 * - obsidian::AppCallbacks - Application lifecycle callbacks
 */

