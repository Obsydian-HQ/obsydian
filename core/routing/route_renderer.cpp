/**
 * Obsidian Core - Route Renderer Implementation
 */

#include "route_renderer.h"
#include "obsidian/navigation/router.h"
#include "obsidian/window.h"
#include "obsidian/navigation/screen_container.h"
#include <iostream>
#include <sstream>

namespace obsidian::routing {

class RouteRenderer::Impl {
public:
    std::map<std::string, RouteComponentFunction> routeComponents;
    std::map<std::string, LayoutComponentFunction> layoutComponents;
};

RouteRenderer::RouteRenderer() : pImpl(std::make_unique<Impl>()) {}

RouteRenderer::RouteRenderer(RouteRenderer&&) noexcept = default;
RouteRenderer& RouteRenderer::operator=(RouteRenderer&&) noexcept = default;

RouteRenderer::~RouteRenderer() = default;

void RouteRenderer::registerRouteComponent(const std::string& routePath, RouteComponentFunction componentFunction) {
    pImpl->routeComponents[routePath] = componentFunction;
}

void RouteRenderer::registerLayoutComponent(const std::string& routePath, LayoutComponentFunction layoutFunction) {
    pImpl->layoutComponents[routePath] = layoutFunction;
}

bool RouteRenderer::isRouteComponentRegistered(const std::string& routePath) const {
    return pImpl->routeComponents.find(routePath) != pImpl->routeComponents.end();
}

bool RouteRenderer::isLayoutComponentRegistered(const std::string& routePath) const {
    return pImpl->layoutComponents.find(routePath) != pImpl->layoutComponents.end();
}

void RouteRenderer::clear() {
    pImpl->routeComponents.clear();
    pImpl->layoutComponents.clear();
}

std::string RouteRenderer::buildPathFromNode(const RouteNode* routeNode) const {
    if (!routeNode) {
        return "/";
    }
    
    // Use the fullPath from the route node if available
    if (!routeNode->fullPath.empty()) {
        return routeNode->fullPath;
    }
    
    // Otherwise, build path from node hierarchy
    std::vector<std::string> segments;
    const RouteNode* current = routeNode;
    
    while (current && current->parent) {
        segments.insert(segments.begin(), current->path);
        current = current->parent;
    }
    
    std::ostringstream oss;
    oss << "/";
    for (size_t i = 0; i < segments.size(); ++i) {
        if (i > 0) {
            oss << "/";
        }
        oss << segments[i];
    }
    
    return oss.str();
}

bool RouteRenderer::renderRoute(const RouteNode* routeNode,
                                obsidian::Window& window,
                                obsidian::ScreenContainer* screenContainer,
                                obsidian::Router& router,
                                const std::map<std::string, std::string>& params,
                                const std::map<std::string, std::string>& query) {
    if (!routeNode) {
        std::cerr << "RouteRenderer: Cannot render null route node" << std::endl;
        return false;
    }
    
    // Get route path for screen lookup
    std::string routePath = buildPathFromNode(routeNode);
    std::cerr << "[RouteRenderer] Rendering route: " << routePath << std::endl;
    
    // Get or create screen for this route
    obsidian::Screen* screen = nullptr;
    if (screenContainer) {
        screen = screenContainer->getOrCreateScreen(routePath);
        std::cerr << "[RouteRenderer] Screen " << (screen ? "created/found" : "FAILED") << " for path: " << routePath << std::endl;
        if (screen) {
            // Clear previous content
            screen->clear();
            std::cerr << "[RouteRenderer] Screen content cleared" << std::endl;
        }
    } else {
        std::cerr << "[RouteRenderer] WARNING: No screenContainer!" << std::endl;
    }
    
    // Render into screen
    bool result = renderRouteWithLayouts(routeNode, window, screen, router, params, query, "");
    std::cerr << "[RouteRenderer] renderRouteWithLayouts returned: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    
    // Set screen as active (makes it visible)
    if (screenContainer && screen) {
        screenContainer->setActiveScreen(screen);
        std::cerr << "[RouteRenderer] Screen set as active" << std::endl;
    }
    
    return result;
}

bool RouteRenderer::renderRouteWithLayouts(const RouteNode* routeNode,
                                          obsidian::Window& window,
                                          obsidian::Screen* screen,
                                          obsidian::Router& router,
                                          const std::map<std::string, std::string>& params,
                                          const std::map<std::string, std::string>& query,
                                          const std::string& /* currentPath */) {
    if (!routeNode) {
        return false;
    }
    
    // Build current path for layout lookup
    std::string routePath = buildPathFromNode(routeNode);
    
    // Check if route component is registered
    auto routeIt = pImpl->routeComponents.find(routePath);
    if (routeIt == pImpl->routeComponents.end()) {
        std::cerr << "RouteRenderer: Route component not registered for path: " << routePath << std::endl;
        return false;
    }
    
    // Create RouteContext with screen for rendering
    obsidian::RouteContext ctx(window, screen, routePath, params, query, router);
    
    // Collect layouts in parent hierarchy (outermost to innermost)
    // First check if the route node itself has a layout
    std::vector<LayoutComponentFunction> layoutChain;
    const RouteNode* current = routeNode;
    
    // Check route node itself for layout (e.g., /products with both layout and route)
    if (current && current->hasLayout) {
        std::string layoutPath = buildPathFromNode(current);
        auto layoutIt = pImpl->layoutComponents.find(layoutPath);
        if (layoutIt != pImpl->layoutComponents.end()) {
            layoutChain.push_back(layoutIt->second);
        }
    }
    
    // Then walk up parent chain
    current = routeNode->parent;
    while (current) {
        if (current->hasLayout) {
            std::string layoutPath = buildPathFromNode(current);
            auto layoutIt = pImpl->layoutComponents.find(layoutPath);
            if (layoutIt != pImpl->layoutComponents.end()) {
                layoutChain.insert(layoutChain.begin(), layoutIt->second);
            }
        }
        current = current->parent;
    }
    
    // Render with layout nesting
    // If we have layouts, render them in order (outermost to innermost)
    // Each layout wraps the next layout or the route content
    std::cerr << "[RouteRenderer] Layout chain has " << layoutChain.size() << " layouts for path: " << routePath << std::endl;
    
    try {
        if (layoutChain.empty()) {
            // No layouts, just render the route component
            std::cerr << "[RouteRenderer] No layouts, rendering route directly" << std::endl;
            routeIt->second(ctx);
        } else {
            // Build nested rendering: outermost layout wraps next layout, which wraps route
            std::function<void()> renderContent = [&]() {
                std::cerr << "[RouteRenderer] Rendering route content for: " << routePath << std::endl;
                routeIt->second(ctx);
                std::cerr << "[RouteRenderer] Route content rendered" << std::endl;
            };
            
            // Wrap renderContent with each layout (from innermost to outermost)
            for (auto it = layoutChain.rbegin(); it != layoutChain.rend(); ++it) {
                auto layoutFunc = *it;
                auto currentRender = renderContent;
                renderContent = [&, layoutFunc, currentRender]() {
                    std::cerr << "[RouteRenderer] Entering layout function" << std::endl;
                    layoutFunc(ctx, currentRender);
                    std::cerr << "[RouteRenderer] Exited layout function" << std::endl;
                };
            }
            
            // Execute the nested rendering chain
            std::cerr << "[RouteRenderer] Executing nested rendering chain" << std::endl;
            renderContent();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "RouteRenderer: Exception while rendering route " << routePath << ": " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "RouteRenderer: Unknown exception while rendering route " << routePath << std::endl;
        return false;
    }
}

} // namespace obsidian::routing
