/**
 * Obsidian Public API - Process
 * 
 * This is the public API header for process execution.
 * Users should include <obsidian/obsidian.h> for the full API.
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace obsidian {

/**
 * Process class - Platform-agnostic process execution
 * 
 * This class provides a clean, high-level interface for executing
 * shell commands and capturing their output across all platforms.
 * Platform-specific details are hidden.
 */
class Process {
public:
    /**
     * Create a new process
     */
    Process();
    
    /**
     * Destructor - automatically terminates and cleans up the process
     */
    ~Process();
    
    /**
     * Create a process with the given command and arguments
     * @param command The command to execute (e.g., "/bin/ls" or "ls")
     * @param arguments Vector of argument strings (empty by default)
     * @param workingDirectory Working directory (empty string for current directory)
     * @return true if process was created successfully
     */
    bool create(const std::string& command, 
                const std::vector<std::string>& arguments = {},
                const std::string& workingDirectory = "");
    
    /**
     * Start the process
     * @return true if process was started successfully
     */
    bool start();
    
    /**
     * Set the stdout callback
     * The callback will be called when data is available on stdout
     * @param callback Function that receives stdout data as a string
     */
    void setOnStdout(std::function<void(const std::string&)> callback);
    
    /**
     * Set the stderr callback
     * The callback will be called when data is available on stderr
     * @param callback Function that receives stderr data as a string
     */
    void setOnStderr(std::function<void(const std::string&)> callback);
    
    /**
     * Set the termination callback
     * The callback will be called when the process terminates
     * @param callback Function that receives the exit code
     */
    void setOnTermination(std::function<void(int exitCode)> callback);
    
    /**
     * Set the error callback
     * The callback will be called when an error occurs (e.g., failed to launch)
     * @param callback Function that receives the error message
     */
    void setOnError(std::function<void(const std::string& errorMessage)> callback);
    
    /**
     * Terminate the process
     * Sends SIGTERM to the process
     */
    void terminate();
    
    /**
     * Check if the process is currently running
     * @return true if the process is running
     */
    bool isRunning() const;
    
    /**
     * Wait for the process to finish
     * Blocks until the process terminates
     * @return The exit code of the process, or -1 on error
     */
    int waitUntilExit();
    
    /**
     * Get the exit code of the process
     * Only valid after the process has terminated
     * @return The exit code, or -1 if the process hasn't terminated yet
     */
    int getExitCode() const;
    
    /**
     * Get the process identifier (PID) of the process
     * Only valid after the process has been started
     * @return The process ID, or -1 if the process hasn't been started yet or is invalid
     */
    int32_t getPid() const;
    
    /**
     * Check if process is valid
     * @return true if the process handle is valid
     */
    bool isValid() const;
    
    // Non-copyable
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    
    // Movable
    Process(Process&&) noexcept;
    Process& operator=(Process&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian

