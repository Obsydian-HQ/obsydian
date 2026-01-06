/**
 * Obsidian Core - Route Scanner
 * 
 * Scans the app/ directory at runtime to discover route files and build
 * a route tree structure. Supports file-based routing conventions:
 * - _layout.cpp: Layout component
 * - index.cpp: Index route
 * - [param].cpp: Dynamic route segment
 * - [...catchall].cpp: Catch-all route
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace obsidian::routing {

/**
 * Route file type enumeration
 */
enum class RouteFileType {
    Layout,      // _layout.cpp
    Index,       // index.cpp
    Static,      // Regular .cpp file (e.g., about.cpp)
    Dynamic,     // [param].cpp
    CatchAll     // [...catchall].cpp
};

/**
 * Route file information
 */
struct RouteFile {
    std::filesystem::path filePath;      // Full path to the file
    std::filesystem::path relativePath; // Path relative to app/ directory
    std::string routePath;                // Route path (e.g., "/about", "/users/:id")
    std::string fileName;                // File name without extension
    RouteFileType type;                  // Type of route file
    std::string dynamicParamName;       // Parameter name for dynamic routes (e.g., "id" from [id].cpp)
    bool isCatchAll;                    // True if catch-all route
    
    RouteFile() : type(RouteFileType::Static), isCatchAll(false) {}
};

/**
 * Route Scanner
 * 
 * Recursively scans the app/ directory to discover route files
 * and parse their route information.
 */
class RouteScanner {
public:
    RouteScanner();
    ~RouteScanner();
    
    /**
     * Scan the app directory for route files
     * @param appDirectory Path to the app/ directory
     * @return true if scanning was successful
     */
    bool scan(const std::string& appDirectory);
    
    /**
     * Get all discovered route files
     * @return Vector of route file information
     */
    const std::vector<RouteFile>& getRouteFiles() const;
    
    /**
     * Get route files for a specific directory
     * @param directoryPath Path relative to app/ directory
     * @return Vector of route files in that directory
     */
    std::vector<RouteFile> getRouteFilesForDirectory(const std::string& directoryPath) const;
    
    /**
     * Check if scanning was successful
     */
    bool isValid() const;
    
    /**
     * Get the app directory path that was scanned
     */
    std::string getAppDirectory() const;
    
    // Non-copyable
    RouteScanner(const RouteScanner&) = delete;
    RouteScanner& operator=(const RouteScanner&) = delete;
    
    // Movable
    RouteScanner(RouteScanner&&) noexcept;
    RouteScanner& operator=(RouteScanner&&) noexcept;

private:
    /**
     * Recursively scan a directory for route files
     * @param directory Directory to scan
     * @param relativePath Current relative path from app/ directory
     */
    void scanDirectory(const std::filesystem::path& directory, const std::filesystem::path& relativePath);
    
    /**
     * Parse a file name to determine route type and information
     * @param fileName File name (e.g., "about.cpp", "[id].cpp", "_layout.cpp")
     * @param routeFile Output route file information
     * @return true if file is a valid route file
     */
    bool parseFileName(const std::string& fileName, RouteFile& routeFile);
    
    /**
     * Build route path from relative file path
     * @param relativePath Relative path from app/ directory
     * @param routeFile Route file information
     * @return Route path (e.g., "/about", "/users/:id")
     */
    std::string buildRoutePath(const std::filesystem::path& relativePath, const RouteFile& routeFile);
    
    /**
     * Extract dynamic parameter name from file name
     * @param fileName File name (e.g., "[id].cpp", "[userId].cpp")
     * @return Parameter name (e.g., "id", "userId")
     */
    std::string extractDynamicParamName(const std::string& fileName);
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian::routing
