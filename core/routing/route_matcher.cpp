/**
 * Obsidian Core - Route Matcher Implementation
 */

#include "route_matcher.h"
#include <sstream>
#include <iomanip>
#include <cctype>

namespace obsidian::routing {

void RouteMatcher::parseUrl(const std::string& url, 
                            std::string& path, 
                            std::map<std::string, std::string>& query) {
    query.clear();
    
    // Find query string separator
    size_t queryPos = url.find('?');
    
    if (queryPos == std::string::npos) {
        // No query string
        path = url;
        return;
    }
    
    // Extract path
    path = url.substr(0, queryPos);
    
    // Extract and parse query string
    std::string queryString = url.substr(queryPos + 1);
    query = parseQueryString(queryString);
}

std::map<std::string, std::string> RouteMatcher::parseQueryString(const std::string& queryString) {
    std::map<std::string, std::string> result;
    
    if (queryString.empty()) {
        return result;
    }
    
    std::istringstream iss(queryString);
    std::string pair;
    
    while (std::getline(iss, pair, '&')) {
        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            std::string key = pair.substr(0, eqPos);
            std::string value = pair.substr(eqPos + 1);
            
            // URL decode
            key = urlDecode(key);
            value = urlDecode(value);
            
            result[key] = value;
        } else if (!pair.empty()) {
            // Key without value
            result[urlDecode(pair)] = "";
        }
    }
    
    return result;
}

std::string RouteMatcher::urlDecode(const std::string& encoded) {
    std::ostringstream decoded;
    
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.length()) {
            // Try to decode hex sequence
            std::string hex = encoded.substr(i + 1, 2);
            char* end;
            unsigned long value = std::strtoul(hex.c_str(), &end, 16);
            if (*end == '\0') {
                decoded << static_cast<char>(value);
                i += 2;
                continue;
            }
        } else if (encoded[i] == '+') {
            decoded << ' ';
            continue;
        }
        
        decoded << encoded[i];
    }
    
    return decoded.str();
}

std::string RouteMatcher::urlEncode(const std::string& decoded) {
    std::ostringstream encoded;
    encoded << std::hex << std::uppercase;
    
    for (unsigned char c : decoded) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else if (c == ' ') {
            encoded << '+';
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
    }
    
    return encoded.str();
}

} // namespace obsidian::routing
