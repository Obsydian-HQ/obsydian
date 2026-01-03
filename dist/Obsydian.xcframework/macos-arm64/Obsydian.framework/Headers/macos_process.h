/**
 * macOS Process FFI - C Interface
 * 
 * Provides C interface for creating and managing NSTask instances
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
 * Opaque handle to a macOS process
 */
typedef void* ObsidianProcessHandle;

/**
 * Process creation parameters
 */
typedef struct {
    const char* command;  // Command to execute (e.g., "/bin/ls")
    const char** arguments;  // Array of argument strings (NULL-terminated)
    int argumentCount;  // Number of arguments
    const char* workingDirectory;  // Working directory (NULL for current)
} ObsidianProcessParams;

/**
 * Process output callback function type
 * Called when data is available on stdout or stderr
 * @param data The output data (null-terminated string)
 * @param isStderr true if this is stderr output, false if stdout
 * @param userData User-provided data pointer
 */
typedef void (*ObsidianProcessOutputCallback)(const char* data, bool isStderr, void* userData);

/**
 * Process termination callback function type
 * Called when the process terminates
 * @param exitCode The exit code of the process
 * @param userData User-provided data pointer
 */
typedef void (*ObsidianProcessTerminationCallback)(int exitCode, void* userData);

/**
 * Process error callback function type
 * Called when an error occurs (e.g., failed to launch)
 * @param errorMessage The error message
 * @param userData User-provided data pointer
 */
typedef void (*ObsidianProcessErrorCallback)(const char* errorMessage, void* userData);

/**
 * Create a new macOS process
 * Returns a handle to the process, or NULL on failure
 */
ObsidianProcessHandle obsidian_macos_create_process(ObsidianProcessParams params);

/**
 * Start the process
 * Returns true if the process was started successfully
 */
bool obsidian_macos_process_start(ObsidianProcessHandle handle);

/**
 * Set the stdout callback
 * The callback will be called when data is available on stdout
 */
void obsidian_macos_process_set_on_stdout(ObsidianProcessHandle handle,
                                           ObsidianProcessOutputCallback callback,
                                           void* userData);

/**
 * Set the stderr callback
 * The callback will be called when data is available on stderr
 */
void obsidian_macos_process_set_on_stderr(ObsidianProcessHandle handle,
                                          ObsidianProcessOutputCallback callback,
                                          void* userData);

/**
 * Set the termination callback
 * The callback will be called when the process terminates
 */
void obsidian_macos_process_set_on_termination(ObsidianProcessHandle handle,
                                               ObsidianProcessTerminationCallback callback,
                                               void* userData);

/**
 * Set the error callback
 * The callback will be called when an error occurs (e.g., failed to launch)
 */
void obsidian_macos_process_set_on_error(ObsidianProcessHandle handle,
                                         ObsidianProcessErrorCallback callback,
                                         void* userData);

/**
 * Terminate the process
 * Sends SIGTERM to the process
 */
void obsidian_macos_process_terminate(ObsidianProcessHandle handle);

/**
 * Check if the process is currently running
 */
bool obsidian_macos_process_is_running(ObsidianProcessHandle handle);

/**
 * Wait for the process to finish
 * Blocks until the process terminates
 * Returns the exit code of the process
 */
int obsidian_macos_process_wait_until_exit(ObsidianProcessHandle handle);

/**
 * Get the exit code of the process
 * Only valid after the process has terminated
 * Returns -1 if the process hasn't terminated yet
 */
int obsidian_macos_process_get_exit_code(ObsidianProcessHandle handle);

/**
 * Get the process identifier (PID) of the process
 * Only valid after the process has been started
 * Returns -1 if the process hasn't been started yet or is invalid
 */
int32_t obsidian_macos_process_get_pid(ObsidianProcessHandle handle);

/**
 * Check if process handle is valid
 */
bool obsidian_macos_process_is_valid(ObsidianProcessHandle handle);

/**
 * Close and destroy the process handle
 */
void obsidian_macos_destroy_process(ObsidianProcessHandle handle);

#ifdef __cplusplus
}
#endif

#endif // __APPLE__

