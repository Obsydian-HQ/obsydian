/**
 * Obsidian Runtime - Core Header
 * 
 * This is the main entry point for Obsidian applications.
 * Provides the runtime environment, HMR support, and core abstractions.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <functional>

// Forward declare AppCallbacks - will be defined by public API
namespace obsidian {
    struct AppCallbacks;

/**
 * Main Obsidian runtime class
 */
class Runtime {
public:
    Runtime();
    ~Runtime();

    /**
     * Initialize the runtime
     */
    bool initialize();

    /**
     * Run the application with the given callbacks
     */
    void run(const AppCallbacks& callbacks);

    /**
     * Shutdown the runtime
     */
    void shutdown();

    /**
     * Enable Hot Module Reload (HMR)
     */
    void enableHMR(bool enabled);

    /**
     * Check if HMR is enabled
     */
    bool isHMREnabled() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Main entry point for Obsidian applications
 */
int run(int argc, char* argv[], const AppCallbacks& callbacks);

} // namespace obsidian

