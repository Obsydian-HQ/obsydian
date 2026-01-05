/**
 * Obsidian Core - Route Scanner Implementation
 */

#include "route_scanner.h"
#include <algorithm>
#include <regex>
#include <iostream>
#include <sstream>

namespace obsidian::routing {

class RouteScanner::Impl {
public:
    std::vector<RouteFile> routeFiles;
    std::string appDirectory;
    bool valid = false;
};

RouteScanner::RouteScanner() : pImpl(std::make_unique<Impl>()) {}

RouteScanner::RouteScanner(RouteScanner&&) noexcept = default;
RouteScanner& RouteScanner::operator=(RouteScanner&&) noexcept = default;

RouteScanner::~RouteScanner() = default;

bool RouteScanner::scan(const std::string& appDirectory) {
    pImpl->routeFiles.clear();
    pImpl->appDirectory = appDirectory;
    pImpl->valid = false;
    
    std::filesystem::path appPath(appDirectory);
    
    // Check if app directory exists
    if (!std::filesystem::exists(appPath)) {
        std::cerr << "RouteScanner: App directory does not exist: " << appDirectory << std::endl;
        return false;
    }
    
    if (!std::filesystem::is_directory(appPath)) {
        std::cerr << "RouteScanner: App directory is not a directory: " << appDirectory << std::endl;
        return false;
    }
    
    // Start recursive scanning from app/ directory
    scanDirectory(appPath, std::filesystem::path(""));
    
    pImpl->valid = true;
    return true;
}

void RouteScanner::scanDirectory(const std::filesystem::path& directory, const std::filesystem::path& relativePath) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                // Recursively scan subdirectories
                std::filesystem::path newRelativePath = relativePath / entry.path().filename();
                scanDirectory(entry.path(), newRelativePath);
            } else if (entry.is_regular_file()) {
                // Check if it's a .cpp file
                std::string fileName = entry.path().filename().string();
                if (fileName.ends_with(".cpp")) {
                    RouteFile routeFile;
                    if (parseFileName(fileName, routeFile)) {
                        routeFile.filePath = entry.path();
                        routeFile.relativePath = relativePath;
                        routeFile.routePath = buildRoutePath(relativePath, routeFile);
                        pImpl->routeFiles.push_back(routeFile);
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "RouteScanner: Error scanning directory: " << e.what() << std::endl;
    }
}

bool RouteScanner::parseFileName(const std::string& fileName, RouteFile& routeFile) {
    routeFile.fileName = fileName;
    
    // Remove .cpp extension
    std::string baseName = fileName;
    if (baseName.ends_with(".cpp")) {
        baseName = baseName.substr(0, baseName.length() - 4);
    }
    
    // Check for special file types
    if (baseName == "_layout") {
        routeFile.type = RouteFileType::Layout;
        return true;
    }
    
    if (baseName == "index") {
        routeFile.type = RouteFileType::Index;
        return true;
    }
    
    // Check for catch-all route: [...param].cpp
    std::regex catchAllRegex(R"(\[\.\.\.([^\]]+)\])");
    std::smatch catchAllMatch;
    if (std::regex_match(baseName, catchAllMatch, catchAllRegex)) {
        routeFile.type = RouteFileType::CatchAll;
        routeFile.dynamicParamName = catchAllMatch[1].str();
        routeFile.isCatchAll = true;
        return true;
    }
    
    // Check for dynamic route: [param].cpp
    std::regex dynamicRegex(R"(\[([^\]]+)\])");
    std::smatch dynamicMatch;
    if (std::regex_match(baseName, dynamicMatch, dynamicRegex)) {
        routeFile.type = RouteFileType::Dynamic;
        routeFile.dynamicParamName = dynamicMatch[1].str();
        return true;
    }
    
    // Regular static route
    routeFile.type = RouteFileType::Static;
    return true;
}

std::string RouteScanner::buildRoutePath(const std::filesystem::path& relativePath, const RouteFile& routeFile) {
    std::string routePath = "/";
    
    // Build path from directory structure, converting dynamic segments
    if (!relativePath.empty()) {
        std::string pathStr = relativePath.string();
        // Replace backslashes with forward slashes (Windows compatibility)
        std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
        // Remove leading slash if present
        if (pathStr.front() == '/') {
            pathStr = pathStr.substr(1);
        }
        
        // Split path and convert dynamic segments
        std::istringstream iss(pathStr);
        std::string segment;
        while (std::getline(iss, segment, '/')) {
            if (!segment.empty()) {
                // Check if segment is a dynamic route pattern [param]
                std::regex dynamicRegex(R"(\[([^\]]+)\])");
                std::smatch match;
                if (std::regex_match(segment, match, dynamicRegex)) {
                    // Convert [param] to :param
                    routePath += ":" + match[1].str() + "/";
                } else {
                    // Regular segment
                    routePath += segment + "/";
                }
            }
        }
    }
    
    // Add route segment based on file type
    switch (routeFile.type) {
        case RouteFileType::Layout:
            // Layout files don't add to route path
            if (routePath == "/") {
                routePath = "/";
            } else {
                // Remove trailing slash for layout
                routePath.pop_back();
            }
            break;
            
        case RouteFileType::Index:
            // Index files map to the directory path
            if (routePath != "/") {
                routePath.pop_back(); // Remove trailing slash
            }
            break;
            
        case RouteFileType::Static:
            // Static routes: about.cpp -> /about
            routePath += routeFile.fileName.substr(0, routeFile.fileName.length() - 4); // Remove .cpp
            break;
            
        case RouteFileType::Dynamic:
            // Dynamic routes: [id].cpp -> /:id
            routePath += ":" + routeFile.dynamicParamName;
            break;
            
        case RouteFileType::CatchAll:
            // Catch-all routes: [...slug].cpp -> /*
            routePath += "*";
            break;
    }
    
    return routePath;
}

std::string RouteScanner::extractDynamicParamName(const std::string& fileName) {
    std::regex dynamicRegex(R"(\[([^\]]+)\])");
    std::smatch match;
    if (std::regex_search(fileName, match, dynamicRegex)) {
        return match[1].str();
    }
    return "";
}

const std::vector<RouteFile>& RouteScanner::getRouteFiles() const {
    return pImpl->routeFiles;
}

std::vector<RouteFile> RouteScanner::getRouteFilesForDirectory(const std::string& directoryPath) const {
    std::vector<RouteFile> result;
    std::filesystem::path targetPath(directoryPath);
    
    for (const auto& routeFile : pImpl->routeFiles) {
        if (routeFile.relativePath == targetPath) {
            result.push_back(routeFile);
        }
    }
    
    return result;
}

bool RouteScanner::isValid() const {
    return pImpl->valid;
}

std::string RouteScanner::getAppDirectory() const {
    return pImpl->appDirectory;
}

} // namespace obsidian::routing
