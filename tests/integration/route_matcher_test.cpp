/**
 * Integration tests for Route Matcher
 */

#include "core/routing/route_matcher.h"
#include <gtest/gtest.h>
#include <map>
#include <string>

using namespace obsidian::routing;

class RouteMatcherTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(RouteMatcherTest, ParseUrlWithoutQuery) {
    std::string path;
    std::map<std::string, std::string> query;
    
    RouteMatcher::parseUrl("/about", path, query);
    
    ASSERT_EQ(path, "/about") << "Path should be extracted correctly";
    ASSERT_TRUE(query.empty()) << "Query should be empty when no query string";
}

TEST_F(RouteMatcherTest, ParseUrlWithQuery) {
    std::string path;
    std::map<std::string, std::string> query;
    
    RouteMatcher::parseUrl("/users?sort=name&order=asc", path, query);
    
    ASSERT_EQ(path, "/users") << "Path should be extracted correctly";
    ASSERT_EQ(query.size(), 2) << "Should have 2 query parameters";
    ASSERT_EQ(query["sort"], "name") << "sort parameter should be correct";
    ASSERT_EQ(query["order"], "asc") << "order parameter should be correct";
}

TEST_F(RouteMatcherTest, ParseUrlWithSingleQueryParam) {
    std::string path;
    std::map<std::string, std::string> query;
    
    RouteMatcher::parseUrl("/search?q=test", path, query);
    
    ASSERT_EQ(path, "/search") << "Path should be extracted correctly";
    ASSERT_EQ(query.size(), 1) << "Should have 1 query parameter";
    ASSERT_EQ(query["q"], "test") << "q parameter should be correct";
}

TEST_F(RouteMatcherTest, ParseUrlWithEmptyQueryValue) {
    std::string path;
    std::map<std::string, std::string> query;
    
    RouteMatcher::parseUrl("/filter?category=&sort=name", path, query);
    
    ASSERT_EQ(path, "/filter") << "Path should be extracted correctly";
    ASSERT_EQ(query.size(), 2) << "Should have 2 query parameters";
    ASSERT_EQ(query["category"], "") << "Empty query value should be preserved";
    ASSERT_EQ(query["sort"], "name") << "sort parameter should be correct";
}

TEST_F(RouteMatcherTest, ParseQueryString) {
    auto query = RouteMatcher::parseQueryString("sort=name&order=asc&limit=10");
    
    ASSERT_EQ(query.size(), 3) << "Should have 3 parameters";
    ASSERT_EQ(query["sort"], "name") << "sort should be correct";
    ASSERT_EQ(query["order"], "asc") << "order should be correct";
    ASSERT_EQ(query["limit"], "10") << "limit should be correct";
}

TEST_F(RouteMatcherTest, ParseQueryStringEmpty) {
    auto query = RouteMatcher::parseQueryString("");
    
    ASSERT_TRUE(query.empty()) << "Empty query string should return empty map";
}

TEST_F(RouteMatcherTest, ParseQueryStringSingleParam) {
    auto query = RouteMatcher::parseQueryString("key=value");
    
    ASSERT_EQ(query.size(), 1) << "Should have 1 parameter";
    ASSERT_EQ(query["key"], "value") << "Parameter should be correct";
}

TEST_F(RouteMatcherTest, UrlDecode) {
    std::string encoded = "hello%20world";
    std::string decoded = RouteMatcher::urlDecode(encoded);
    
    ASSERT_EQ(decoded, "hello world") << "Should decode space correctly";
}

TEST_F(RouteMatcherTest, UrlDecodeSpecialChars) {
    std::string encoded = "test%2Bvalue%3D123";
    std::string decoded = RouteMatcher::urlDecode(encoded);
    
    ASSERT_EQ(decoded, "test+value=123") << "Should decode special characters";
}

TEST_F(RouteMatcherTest, UrlDecodeNoEncoding) {
    std::string plain = "hello world";
    std::string decoded = RouteMatcher::urlDecode(plain);
    
    ASSERT_EQ(decoded, plain) << "Plain text should remain unchanged";
}

TEST_F(RouteMatcherTest, UrlEncode) {
    std::string plain = "hello world";
    std::string encoded = RouteMatcher::urlEncode(plain);
    
    ASSERT_EQ(encoded, "hello%20world") << "Should encode space correctly";
}

TEST_F(RouteMatcherTest, UrlEncodeSpecialChars) {
    std::string plain = "test+value=123";
    std::string encoded = RouteMatcher::urlEncode(plain);
    
    // Should encode + and = characters
    ASSERT_NE(encoded, plain) << "Special characters should be encoded";
    ASSERT_TRUE(encoded.find("+") == std::string::npos || encoded.find("%2B") != std::string::npos) 
        << "Plus sign should be encoded";
}

TEST_F(RouteMatcherTest, UrlEncodeDecodeRoundTrip) {
    std::string original = "hello world! test+value=123";
    std::string encoded = RouteMatcher::urlEncode(original);
    std::string decoded = RouteMatcher::urlDecode(encoded);
    
    ASSERT_EQ(decoded, original) << "Round trip should preserve original string";
}

TEST_F(RouteMatcherTest, ParseUrlWithEncodedQuery) {
    std::string path;
    std::map<std::string, std::string> query;
    
    RouteMatcher::parseUrl("/search?q=hello%20world", path, query);
    
    ASSERT_EQ(path, "/search") << "Path should be extracted correctly";
    ASSERT_EQ(query.size(), 1) << "Should have 1 query parameter";
    // Query values should be decoded
    ASSERT_EQ(query["q"], "hello world") << "Query value should be decoded";
}
