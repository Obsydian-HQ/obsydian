/**
 * Obsidian Core - Navigation History
 * 
 * Manages navigation history stack for back/forward navigation.
 * Similar to browser history API.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace obsidian::routing {

/**
 * History entry representing a navigation state
 */
struct HistoryEntry {
    std::string path;                              // Route path
    std::map<std::string, std::string> params;    // Route parameters
    std::map<std::string, std::string> query;    // Query parameters
    int index;                                     // Index in history stack
    
    HistoryEntry() : index(-1) {}
    HistoryEntry(const std::string& p) : path(p), index(-1) {}
};

/**
 * Navigation History
 * 
 * Manages a stack of navigation entries with support for
 * back/forward navigation, similar to browser history.
 */
class NavigationHistory {
public:
    NavigationHistory();
    ~NavigationHistory();
    
    /**
     * Push a new entry onto the history stack
     * @param path Route path
     * @param params Route parameters
     * @param query Query parameters
     */
    void push(const std::string& path, 
              const std::map<std::string, std::string>& params = {},
              const std::map<std::string, std::string>& query = {});
    
    /**
     * Replace the current entry (doesn't add to history)
     * @param path Route path
     * @param params Route parameters
     * @param query Query parameters
     */
    void replace(const std::string& path,
                 const std::map<std::string, std::string>& params = {},
                 const std::map<std::string, std::string>& query = {});
    
    /**
     * Navigate back in history
     * @return History entry if back navigation is possible, nullptr otherwise
     */
    const HistoryEntry* goBack();
    
    /**
     * Navigate forward in history
     * @return History entry if forward navigation is possible, nullptr otherwise
     */
    const HistoryEntry* goForward();
    
    /**
     * Get the current history entry
     * @return Current entry, or nullptr if history is empty
     */
    const HistoryEntry* getCurrent() const;
    
    /**
     * Check if back navigation is possible
     */
    bool canGoBack() const;
    
    /**
     * Check if forward navigation is possible
     */
    bool canGoForward() const;
    
    /**
     * Clear all history
     */
    void clear();
    
    /**
     * Get the number of entries in history
     */
    size_t size() const;
    
    /**
     * Get the current index in history
     */
    int getCurrentIndex() const;
    
    /**
     * Get all history entries (for debugging)
     */
    std::vector<HistoryEntry> getAllEntries() const;
    
    // Non-copyable
    NavigationHistory(const NavigationHistory&) = delete;
    NavigationHistory& operator=(const NavigationHistory&) = delete;
    
    // Movable
    NavigationHistory(NavigationHistory&&) noexcept;
    NavigationHistory& operator=(NavigationHistory&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian::routing
