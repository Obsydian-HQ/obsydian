/**
 * Obsidian Core - Navigation History Implementation
 */

#include "navigation_history.h"
#include <algorithm>

namespace obsidian::routing {

class NavigationHistory::Impl {
public:
    std::vector<HistoryEntry> entries;
    int currentIndex = -1;
    
    void updateIndices() {
        for (size_t i = 0; i < entries.size(); ++i) {
            entries[i].index = static_cast<int>(i);
        }
    }
};

NavigationHistory::NavigationHistory() : pImpl(std::make_unique<Impl>()) {}

NavigationHistory::NavigationHistory(NavigationHistory&&) noexcept = default;
NavigationHistory& NavigationHistory::operator=(NavigationHistory&&) noexcept = default;

NavigationHistory::~NavigationHistory() = default;

void NavigationHistory::push(const std::string& path,
                             const std::map<std::string, std::string>& params,
                             const std::map<std::string, std::string>& query) {
    // If we're not at the end of history, remove all entries after current
    if (pImpl->currentIndex >= 0 && 
        pImpl->currentIndex < static_cast<int>(pImpl->entries.size()) - 1) {
        pImpl->entries.erase(
            pImpl->entries.begin() + pImpl->currentIndex + 1,
            pImpl->entries.end()
        );
    }
    
    // Create new entry
    HistoryEntry entry(path);
    entry.params = params;
    entry.query = query;
    
    // Add to history
    pImpl->entries.push_back(entry);
    pImpl->currentIndex = static_cast<int>(pImpl->entries.size()) - 1;
    pImpl->updateIndices();
}

void NavigationHistory::replace(const std::string& path,
                                const std::map<std::string, std::string>& params,
                                const std::map<std::string, std::string>& query) {
    if (pImpl->currentIndex >= 0 && 
        pImpl->currentIndex < static_cast<int>(pImpl->entries.size())) {
        // Replace current entry
        pImpl->entries[pImpl->currentIndex].path = path;
        pImpl->entries[pImpl->currentIndex].params = params;
        pImpl->entries[pImpl->currentIndex].query = query;
    } else {
        // No current entry, just push
        push(path, params, query);
    }
}

const HistoryEntry* NavigationHistory::goBack() {
    if (!canGoBack()) {
        return nullptr;
    }
    
    pImpl->currentIndex--;
    return &pImpl->entries[pImpl->currentIndex];
}

const HistoryEntry* NavigationHistory::goForward() {
    if (!canGoForward()) {
        return nullptr;
    }
    
    pImpl->currentIndex++;
    return &pImpl->entries[pImpl->currentIndex];
}

const HistoryEntry* NavigationHistory::getCurrent() const {
    if (pImpl->currentIndex >= 0 && 
        pImpl->currentIndex < static_cast<int>(pImpl->entries.size())) {
        return &pImpl->entries[pImpl->currentIndex];
    }
    return nullptr;
}

bool NavigationHistory::canGoBack() const {
    return pImpl->currentIndex > 0;
}

bool NavigationHistory::canGoForward() const {
    return pImpl->currentIndex >= 0 && 
           pImpl->currentIndex < static_cast<int>(pImpl->entries.size()) - 1;
}

void NavigationHistory::clear() {
    pImpl->entries.clear();
    pImpl->currentIndex = -1;
}

size_t NavigationHistory::size() const {
    return pImpl->entries.size();
}

int NavigationHistory::getCurrentIndex() const {
    return pImpl->currentIndex;
}

std::vector<HistoryEntry> NavigationHistory::getAllEntries() const {
    return pImpl->entries;
}

} // namespace obsidian::routing
