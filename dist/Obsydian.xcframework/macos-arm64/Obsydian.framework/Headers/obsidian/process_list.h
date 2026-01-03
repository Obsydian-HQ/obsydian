/**
 * Obsidian Public API - Process List
 * 
 * This is the public API header for process management.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace obsidian {

/**
 * Process information structure
 */
struct ProcessInfo {
    int32_t processId;  // Process ID (PID)
    std::string processName;  // Process name (e.g., "Finder")
    std::string bundleIdentifier;  // Bundle identifier (e.g., "com.apple.finder")
    bool isActive;  // Whether the process is currently active
};

/**
 * ProcessList class - Platform-agnostic process management
 * 
 * This class provides a clean, high-level interface for listing
 * and managing running processes across all platforms.
 * Platform-specific details are hidden.
 */
class ProcessList {
public:
    /**
     * Get all running processes
     * @return Vector of process information
     */
    static std::vector<ProcessInfo> getAllProcesses();
    
    /**
     * Kill a process by its process ID
     * @param processId The process ID to kill
     * @return true if the process was successfully killed
     */
    static bool killProcess(int32_t processId);
    
    /**
     * Get process information for a specific process ID
     * @param processId The process ID to query
     * @param processInfo Output parameter for process information
     * @return true if process information was successfully retrieved
     */
    static bool getProcessInfo(int32_t processId, ProcessInfo& processInfo);
};

} // namespace obsidian

