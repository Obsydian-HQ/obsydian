/**
 * Obsidian Public API - Network
 * 
 * This is the public API header for network operations.
 * Users should include <obsidian/obsidian.h> for the full API.
 * 
 * STATUS: Placeholder - Implementation pending
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace obsidian {

// Forward declarations
class HttpRequest;
class HttpResponse;
class WebSocket;
class TcpSocket;

/**
 * HTTP method types
 */
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
};

/**
 * HTTP headers type
 */
using HttpHeaders = std::unordered_map<std::string, std::string>;

/**
 * HTTP response structure
 */
struct HttpResponseData {
    int statusCode;
    std::string statusMessage;
    HttpHeaders headers;
    std::string body;
    std::vector<uint8_t> binaryBody;
};

/**
 * WebSocket message types
 */
enum class WebSocketMessageType {
    Text,
    Binary,
    Ping,
    Pong
};

/**
 * WebSocket connection state
 */
enum class WebSocketState {
    Connecting,
    Open,
    Closing,
    Closed
};

/**
 * Network class - Platform-agnostic network operations
 * 
 * This class provides a clean, high-level interface for HTTP requests
 * and WebSocket connections across all platforms.
 * 
 * STATUS: Placeholder - Implementation pending
 */
class Network {
public:
    // ========================================================================
    // HTTP Operations
    // ========================================================================
    
    /**
     * Perform a synchronous HTTP GET request
     * @param url Request URL
     * @param headers Optional HTTP headers
     * @return HTTP response
     */
    static HttpResponseData get(
        const std::string& url,
        const HttpHeaders& headers = {}
    );
    
    /**
     * Perform a synchronous HTTP POST request
     * @param url Request URL
     * @param body Request body
     * @param headers Optional HTTP headers
     * @return HTTP response
     */
    static HttpResponseData post(
        const std::string& url,
        const std::string& body,
        const HttpHeaders& headers = {}
    );
    
    /**
     * Perform a synchronous HTTP PUT request
     * @param url Request URL
     * @param body Request body
     * @param headers Optional HTTP headers
     * @return HTTP response
     */
    static HttpResponseData put(
        const std::string& url,
        const std::string& body,
        const HttpHeaders& headers = {}
    );
    
    /**
     * Perform a synchronous HTTP DELETE request
     * @param url Request URL
     * @param headers Optional HTTP headers
     * @return HTTP response
     */
    static HttpResponseData del(
        const std::string& url,
        const HttpHeaders& headers = {}
    );
    
    /**
     * Create an async HTTP request builder
     * @param method HTTP method
     * @param url Request URL
     * @return Request builder
     */
    static std::unique_ptr<HttpRequest> request(HttpMethod method, const std::string& url);
    
    // ========================================================================
    // WebSocket Operations
    // ========================================================================
    
    /**
     * Create a WebSocket connection
     * @param url WebSocket URL (ws:// or wss://)
     * @param headers Optional HTTP headers for upgrade request
     * @return WebSocket connection handle
     */
    static std::unique_ptr<WebSocket> webSocket(
        const std::string& url,
        const HttpHeaders& headers = {}
    );
    
    // ========================================================================
    // TCP Socket Operations
    // ========================================================================
    
    /**
     * Create a TCP socket connection
     * @param host Host address
     * @param port Port number
     * @return TCP socket handle
     */
    static std::unique_ptr<TcpSocket> tcpConnect(
        const std::string& host,
        uint16_t port
    );
};

/**
 * HttpRequest class - Async HTTP request builder
 * 
 * Use Network::request() to create an instance.
 */
class HttpRequest {
public:
    virtual ~HttpRequest() = default;
    
    /**
     * Set request header
     * @param name Header name
     * @param value Header value
     * @return Reference to this for chaining
     */
    virtual HttpRequest& header(const std::string& name, const std::string& value) = 0;
    
    /**
     * Set request body
     * @param body Request body string
     * @return Reference to this for chaining
     */
    virtual HttpRequest& body(const std::string& body) = 0;
    
    /**
     * Set binary request body
     * @param data Binary data
     * @return Reference to this for chaining
     */
    virtual HttpRequest& body(const std::vector<uint8_t>& data) = 0;
    
    /**
     * Set request timeout
     * @param timeoutMs Timeout in milliseconds
     * @return Reference to this for chaining
     */
    virtual HttpRequest& timeout(int timeoutMs) = 0;
    
    /**
     * Send the request asynchronously
     * @param callback Callback for response
     */
    virtual void send(std::function<void(const HttpResponseData&)> callback) = 0;
    
    /**
     * Send the request and get response synchronously
     * @return HTTP response
     */
    virtual HttpResponseData sendSync() = 0;
    
    /**
     * Cancel the request
     */
    virtual void cancel() = 0;
};

/**
 * WebSocket class - WebSocket connection handle
 * 
 * Use Network::webSocket() to create an instance.
 */
class WebSocket {
public:
    virtual ~WebSocket() = default;
    
    /**
     * Set callback for when connection opens
     * @param callback Callback function
     */
    virtual void onOpen(std::function<void()> callback) = 0;
    
    /**
     * Set callback for receiving messages
     * @param callback Callback receiving message type and data
     */
    virtual void onMessage(std::function<void(WebSocketMessageType, const std::string&)> callback) = 0;
    
    /**
     * Set callback for binary messages
     * @param callback Callback receiving binary data
     */
    virtual void onBinaryMessage(std::function<void(const std::vector<uint8_t>&)> callback) = 0;
    
    /**
     * Set callback for when connection closes
     * @param callback Callback receiving close code and reason
     */
    virtual void onClose(std::function<void(int code, const std::string& reason)> callback) = 0;
    
    /**
     * Set callback for errors
     * @param callback Callback receiving error message
     */
    virtual void onError(std::function<void(const std::string&)> callback) = 0;
    
    /**
     * Connect to the WebSocket server
     */
    virtual void connect() = 0;
    
    /**
     * Send a text message
     * @param message Message to send
     */
    virtual void send(const std::string& message) = 0;
    
    /**
     * Send binary data
     * @param data Binary data to send
     */
    virtual void send(const std::vector<uint8_t>& data) = 0;
    
    /**
     * Close the connection
     * @param code Close code (default: 1000 for normal closure)
     * @param reason Close reason
     */
    virtual void close(int code = 1000, const std::string& reason = "") = 0;
    
    /**
     * Get current connection state
     * @return Connection state
     */
    virtual WebSocketState getState() const = 0;
};

/**
 * TcpSocket class - TCP socket connection handle
 * 
 * Use Network::tcpConnect() to create an instance.
 */
class TcpSocket {
public:
    virtual ~TcpSocket() = default;
    
    /**
     * Set callback for when connection is established
     * @param callback Callback function
     */
    virtual void onConnect(std::function<void()> callback) = 0;
    
    /**
     * Set callback for receiving data
     * @param callback Callback receiving data
     */
    virtual void onData(std::function<void(const std::vector<uint8_t>&)> callback) = 0;
    
    /**
     * Set callback for when connection closes
     * @param callback Callback function
     */
    virtual void onClose(std::function<void()> callback) = 0;
    
    /**
     * Set callback for errors
     * @param callback Callback receiving error message
     */
    virtual void onError(std::function<void(const std::string&)> callback) = 0;
    
    /**
     * Connect to the server
     */
    virtual void connect() = 0;
    
    /**
     * Send data
     * @param data Data to send
     */
    virtual void send(const std::vector<uint8_t>& data) = 0;
    
    /**
     * Send string data
     * @param data String to send
     */
    virtual void send(const std::string& data) = 0;
    
    /**
     * Close the connection
     */
    virtual void close() = 0;
    
    /**
     * Check if connected
     * @return true if connected
     */
    virtual bool isConnected() const = 0;
};

} // namespace obsidian
