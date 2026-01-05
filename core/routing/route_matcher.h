/**
 * Obsidian Core - Route Matcher
 * 
 * Utility for parsing URLs and extracting query parameters.
 * Route matching itself is handled by RouteRegistry.
 */

#pragma once

#include <string>
#include <map>

namespace obsidian::routing {

/**
 * Route Matcher
 * 
 * Utility class for parsing URLs and extracting path and query parameters.
 */
class RouteMatcher {
public:
    /**
     * Parse a URL and extract path and query parameters
     * @param url Full URL (e.g., "/users/123?sort=name&order=asc")
     * @param path Output path without query string (e.g., "/users/123")
     * @param query Output map of query parameters
     */
    static void parseUrl(const std::string& url, 
                        std::string& path, 
                        std::map<std::string, std::string>& query);
    
    /**
     * Parse query string into a map
     * @param queryString Query string (e.g., "sort=name&order=asc")
     * @return Map of query parameters
     */
    static std::map<std::string, std::string> parseQueryString(const std::string& queryString);
    
    /**
     * URL decode a string
     * @param encoded Encoded string
     * @return Decoded string
     */
    static std::string urlDecode(const std::string& encoded);
    
    /**
     * URL encode a string
     * @param decoded Decoded string
     * @return Encoded string
     */
    static std::string urlEncode(const std::string& decoded);
};

} // namespace obsidian::routing
