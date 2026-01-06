/**
 * Obsidian Public API - File System
 * 
 * This is the public API header for file system operations.
 * Users should include <obsidian/obsidian.h> for the full API.
 * 
 * STATUS: Placeholder - Implementation pending
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace obsidian {

// Forward declarations
class FileWatcher;

/**
 * File information structure
 */
struct FileInfo {
    std::string path;           // Full path to the file
    std::string name;           // File name only
    std::string extension;      // File extension (without dot)
    uint64_t size;              // File size in bytes
    uint64_t createdAt;         // Creation timestamp
    uint64_t modifiedAt;        // Last modification timestamp
    bool isDirectory;           // Whether this is a directory
    bool isHidden;              // Whether the file is hidden
    bool isSymlink;             // Whether the file is a symbolic link
};

/**
 * File system change event types
 */
enum class FileSystemEvent {
    Created,
    Modified,
    Deleted,
    Renamed,
    MovedIn,
    MovedOut
};

/**
 * File system change notification
 */
struct FileSystemChange {
    FileSystemEvent event;
    std::string path;
    std::string oldPath;  // For rename events
};

/**
 * FileSystem class - Platform-agnostic file system operations
 * 
 * This class provides a clean, high-level interface for file system
 * operations across all platforms. Platform-specific details are hidden.
 * 
 * STATUS: Placeholder - Implementation pending
 */
class FileSystem {
public:
    // ========================================================================
    // File Operations
    // ========================================================================
    
    /**
     * Read file contents as string
     * @param path Path to the file
     * @return File contents, or empty string on error
     */
    static std::string readFile(const std::string& path);
    
    /**
     * Read file contents as binary data
     * @param path Path to the file
     * @return File contents as byte vector
     */
    static std::vector<uint8_t> readBinaryFile(const std::string& path);
    
    /**
     * Write string contents to file
     * @param path Path to the file
     * @param contents Contents to write
     * @return true on success
     */
    static bool writeFile(const std::string& path, const std::string& contents);
    
    /**
     * Write binary data to file
     * @param path Path to the file
     * @param data Binary data to write
     * @return true on success
     */
    static bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
    
    /**
     * Append string contents to file
     * @param path Path to the file
     * @param contents Contents to append
     * @return true on success
     */
    static bool appendFile(const std::string& path, const std::string& contents);
    
    /**
     * Delete a file
     * @param path Path to the file
     * @return true on success
     */
    static bool deleteFile(const std::string& path);
    
    /**
     * Copy a file
     * @param source Source file path
     * @param destination Destination file path
     * @param overwrite Whether to overwrite if destination exists
     * @return true on success
     */
    static bool copyFile(const std::string& source, const std::string& destination, bool overwrite = false);
    
    /**
     * Move a file
     * @param source Source file path
     * @param destination Destination file path
     * @return true on success
     */
    static bool moveFile(const std::string& source, const std::string& destination);
    
    // ========================================================================
    // Directory Operations
    // ========================================================================
    
    /**
     * List directory contents
     * @param path Path to the directory
     * @param includeHidden Whether to include hidden files
     * @return Vector of file information
     */
    static std::vector<FileInfo> listDirectory(const std::string& path, bool includeHidden = false);
    
    /**
     * Create a directory
     * @param path Path to the directory
     * @param recursive Whether to create parent directories
     * @return true on success
     */
    static bool createDirectory(const std::string& path, bool recursive = true);
    
    /**
     * Delete a directory
     * @param path Path to the directory
     * @param recursive Whether to delete contents recursively
     * @return true on success
     */
    static bool deleteDirectory(const std::string& path, bool recursive = false);
    
    // ========================================================================
    // Path Operations
    // ========================================================================
    
    /**
     * Check if a path exists
     * @param path Path to check
     * @return true if path exists
     */
    static bool exists(const std::string& path);
    
    /**
     * Check if a path is a file
     * @param path Path to check
     * @return true if path is a file
     */
    static bool isFile(const std::string& path);
    
    /**
     * Check if a path is a directory
     * @param path Path to check
     * @return true if path is a directory
     */
    static bool isDirectory(const std::string& path);
    
    /**
     * Get file information
     * @param path Path to the file
     * @return File information
     */
    static FileInfo getFileInfo(const std::string& path);
    
    /**
     * Get the current working directory
     * @return Current working directory path
     */
    static std::string getCurrentDirectory();
    
    /**
     * Get the home directory
     * @return Home directory path
     */
    static std::string getHomeDirectory();
    
    /**
     * Get the temporary directory
     * @return Temporary directory path
     */
    static std::string getTempDirectory();
    
    /**
     * Join path components
     * @param base Base path
     * @param component Path component to join
     * @return Joined path
     */
    static std::string joinPath(const std::string& base, const std::string& component);
    
    /**
     * Get the directory name from a path
     * @param path Full path
     * @return Directory portion of the path
     */
    static std::string getDirectoryName(const std::string& path);
    
    /**
     * Get the file name from a path
     * @param path Full path
     * @return File name portion of the path
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * Get the file extension from a path
     * @param path Full path
     * @return File extension (without dot)
     */
    static std::string getExtension(const std::string& path);
    
    // ========================================================================
    // File Watching
    // ========================================================================
    
    /**
     * Watch a path for changes
     * @param path Path to watch (file or directory)
     * @param callback Callback for change events
     * @param recursive Whether to watch subdirectories (for directories)
     * @return Watcher handle (unique_ptr to FileWatcher)
     */
    static std::unique_ptr<FileWatcher> watch(
        const std::string& path,
        std::function<void(const FileSystemChange&)> callback,
        bool recursive = true
    );
};

/**
 * FileWatcher class - File system change watcher handle
 * 
 * Returned by FileSystem::watch(). The watcher remains active until
 * this object is destroyed or stop() is called.
 */
class FileWatcher {
public:
    virtual ~FileWatcher() = default;
    
    /**
     * Stop watching for changes
     */
    virtual void stop() = 0;
    
    /**
     * Check if the watcher is active
     * @return true if still watching
     */
    virtual bool isActive() const = 0;
    
    /**
     * Get the path being watched
     * @return Watched path
     */
    virtual std::string getPath() const = 0;
};

} // namespace obsidian
