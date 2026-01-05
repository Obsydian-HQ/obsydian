/**
 * Integration tests for Navigation History
 */

#include "core/routing/navigation_history.h"
#include <gtest/gtest.h>
#include <map>
#include <string>

using namespace obsidian::routing;

class NavigationHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        history = std::make_unique<NavigationHistory>();
    }
    
    void TearDown() override {
        history.reset();
    }
    
    std::unique_ptr<NavigationHistory> history;
};

TEST_F(NavigationHistoryTest, InitialState) {
    ASSERT_EQ(history->size(), 0) << "History should be empty initially";
    ASSERT_FALSE(history->canGoBack()) << "Cannot go back when empty";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward when empty";
    ASSERT_EQ(history->getCurrentIndex(), -1) << "Current index should be -1 when empty";
    ASSERT_EQ(history->getCurrent(), nullptr) << "Current entry should be nullptr when empty";
}

TEST_F(NavigationHistoryTest, PushSingleEntry) {
    history->push("/about");
    
    ASSERT_EQ(history->size(), 1) << "History should have 1 entry";
    ASSERT_FALSE(history->canGoBack()) << "Cannot go back from first entry";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward from first entry";
    ASSERT_EQ(history->getCurrentIndex(), 0) << "Current index should be 0";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/about") << "Current path should be correct";
}

TEST_F(NavigationHistoryTest, PushMultipleEntries) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    
    ASSERT_EQ(history->size(), 3) << "History should have 3 entries";
    ASSERT_TRUE(history->canGoBack()) << "Should be able to go back";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward at end";
    ASSERT_EQ(history->getCurrentIndex(), 2) << "Current index should be 2";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/contact") << "Current path should be /contact";
}

TEST_F(NavigationHistoryTest, GoBack) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    
    const HistoryEntry* back = history->goBack();
    
    ASSERT_NE(back, nullptr) << "goBack should return an entry";
    ASSERT_EQ(back->path, "/about") << "Should navigate back to /about";
    ASSERT_EQ(history->getCurrentIndex(), 1) << "Current index should be 1";
    ASSERT_TRUE(history->canGoBack()) << "Should still be able to go back";
    ASSERT_TRUE(history->canGoForward()) << "Should be able to go forward";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/about") << "Current path should be /about";
}

TEST_F(NavigationHistoryTest, GoForward) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    history->goBack();
    
    const HistoryEntry* forward = history->goForward();
    
    ASSERT_NE(forward, nullptr) << "goForward should return an entry";
    ASSERT_EQ(forward->path, "/contact") << "Should navigate forward to /contact";
    ASSERT_EQ(history->getCurrentIndex(), 2) << "Current index should be 2";
    ASSERT_TRUE(history->canGoBack()) << "Should be able to go back";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward at end";
}

TEST_F(NavigationHistoryTest, GoBackMultipleTimes) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    history->push("/products");
    
    history->goBack();
    history->goBack();
    
    ASSERT_EQ(history->getCurrentIndex(), 1) << "Current index should be 1";
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/about") << "Current path should be /about";
}

TEST_F(NavigationHistoryTest, GoBackThenPush) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    history->goBack();
    
    // Push new entry - should truncate forward history
    history->push("/products");
    
    ASSERT_EQ(history->size(), 3) << "History should have 3 entries";
    ASSERT_EQ(history->getCurrentIndex(), 2) << "Current index should be 2";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward after push";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/products") << "Current path should be /products";
}

TEST_F(NavigationHistoryTest, Replace) {
    history->push("/");
    history->push("/about");
    
    history->replace("/contact");
    
    ASSERT_EQ(history->size(), 2) << "History size should not change";
    ASSERT_EQ(history->getCurrentIndex(), 1) << "Current index should remain 1";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/contact") << "Current path should be replaced";
}

TEST_F(NavigationHistoryTest, ReplaceFirstEntry) {
    history->push("/");
    
    history->replace("/about");
    
    ASSERT_EQ(history->size(), 1) << "History size should remain 1";
    ASSERT_EQ(history->getCurrentIndex(), 0) << "Current index should be 0";
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/about") << "Current path should be replaced";
}

TEST_F(NavigationHistoryTest, PushWithParams) {
    std::map<std::string, std::string> params;
    params["id"] = "123";
    
    history->push("/users/123", params);
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->path, "/users/123") << "Path should be correct";
    ASSERT_EQ(current->params.size(), 1) << "Should have 1 parameter";
    ASSERT_EQ(current->params.at("id"), "123") << "Parameter should be correct";
}

TEST_F(NavigationHistoryTest, PushWithQuery) {
    std::map<std::string, std::string> query;
    query["sort"] = "name";
    query["order"] = "asc";
    
    history->push("/users", {}, query);
    
    const HistoryEntry* current = history->getCurrent();
    ASSERT_NE(current, nullptr) << "Current entry should not be nullptr";
    ASSERT_EQ(current->query.size(), 2) << "Should have 2 query parameters";
    ASSERT_EQ(current->query.at("sort"), "name") << "Query parameter should be correct";
    ASSERT_EQ(current->query.at("order"), "asc") << "Query parameter should be correct";
}

TEST_F(NavigationHistoryTest, Clear) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    
    history->clear();
    
    ASSERT_EQ(history->size(), 0) << "History should be empty";
    ASSERT_FALSE(history->canGoBack()) << "Cannot go back when empty";
    ASSERT_FALSE(history->canGoForward()) << "Cannot go forward when empty";
    ASSERT_EQ(history->getCurrentIndex(), -1) << "Current index should be -1";
    ASSERT_EQ(history->getCurrent(), nullptr) << "Current entry should be nullptr";
}

TEST_F(NavigationHistoryTest, GetAllEntries) {
    history->push("/");
    history->push("/about");
    history->push("/contact");
    history->goBack();
    
    auto entries = history->getAllEntries();
    
    ASSERT_EQ(entries.size(), 3) << "Should return all 3 entries";
    ASSERT_EQ(entries[0].path, "/") << "First entry should be /";
    ASSERT_EQ(entries[1].path, "/about") << "Second entry should be /about";
    ASSERT_EQ(entries[2].path, "/contact") << "Third entry should be /contact";
}

TEST_F(NavigationHistoryTest, CannotGoBackFromFirst) {
    history->push("/");
    
    const HistoryEntry* back = history->goBack();
    
    ASSERT_EQ(back, nullptr) << "goBack should return nullptr from first entry";
    ASSERT_EQ(history->getCurrentIndex(), 0) << "Index should remain 0";
}

TEST_F(NavigationHistoryTest, CannotGoForwardFromLast) {
    history->push("/");
    history->push("/about");
    
    const HistoryEntry* forward = history->goForward();
    
    ASSERT_EQ(forward, nullptr) << "goForward should return nullptr from last entry";
    ASSERT_EQ(history->getCurrentIndex(), 1) << "Index should remain 1";
}
