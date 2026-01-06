/**
 * macOS Process List FFI - C Interface
 * 
 * Provides C interface for listing and managing running processes
 * from C++ code. This header is C-compatible and can be included from C++.
 */

#pragma once

#ifdef __APPLE__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Process information structure
 */
typedef struct {
    int32_t processId;  // Process ID (PID)
    const char* processName;  // Process name (e.g., "Finder")
    const char* bundleIdentifier;  // Bundle identifier (e.g., "com.apple.finder")
    bool isActive;  // Whether the process is currently active
} ObsidianProcessInfo;

/**
 * Process list structure
 * Contains an array of process information
 */
typedef struct {
    ObsidianProcessInfo* processes;  // Array of process information
    int count;  // Number of processes in the array
} ObsidianProcessList;

/**
 * Get all running processes
 * Returns a process list that must be freed with obsidian_macos_process_list_free()
 * Returns NULL on failure
 */
ObsidianProcessList* obsidian_macos_get_all_processes();

/**
 * Free a process list returned by obsidian_macos_get_all_processes()
 */
void obsidian_macos_process_list_free(ObsidianProcessList* processList);

/**
 * Kill a process by its process ID
 * @param processId The process ID to kill
 * @return true if the process was successfully killed
 */
bool obsidian_macos_kill_process(int32_t processId);

/**
 * Get process information for a specific process ID
 * @param processId The process ID to query
 * @param processInfo Output parameter for process information
 * @return true if process information was successfully retrieved
 */
bool obsidian_macos_get_process_info(int32_t processId, ObsidianProcessInfo* processInfo);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

