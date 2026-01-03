/**
 * Obsidian Public API - Application
 * 
 * This is the public API header for application lifecycle management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace obsidian {

/**
 * Application lifecycle callbacks
 */
struct AppCallbacks {
    std::function<void()> onInit;
    std::function<void()> onUpdate;
    std::function<void()> onShutdown;
};

/**
 * Application class - Main entry point for Obsidian applications
 * 
 * This class manages the application lifecycle, platform initialization,
 * and the main event loop. It provides a clean, high-level API that
 * hides platform-specific details.
 */
class App {
public:
    /**
     * Create a new application instance
     */
    App();
    
    /**
     * Destructor - automatically shuts down the application
     */
    ~App();
    
    /**
     * Initialize the application and platform
     * @return true if initialization was successful
     */
    bool initialize();
    
    /**
     * Run the application with the given callbacks
     * This will start the main event loop and block until the app exits.
     * @param callbacks Lifecycle callbacks
     */
    void run(const AppCallbacks& callbacks);
    
    /**
     * Shutdown the application
     */
    void shutdown();
    
    /**
     * Enable Hot Module Reload (HMR)
     * @param enabled Whether to enable HMR
     */
    void enableHMR(bool enabled);
    
    /**
     * Check if HMR is enabled
     */
    bool isHMREnabled() const;
    
    // Non-copyable
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    
    // Movable
    App(App&&) noexcept;
    App& operator=(App&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};


} // namespace obsidian

