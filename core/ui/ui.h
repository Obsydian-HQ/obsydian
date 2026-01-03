/**
 * Obsidian UI - Cross-platform UI Abstraction
 * 
 * Provides unified UI components that map to native platform controls
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

namespace obsidian::ui {

/**
 * Base UI component
 */
class Component {
public:
    virtual ~Component() = default;
    virtual void render() = 0;
    virtual void setVisible(bool visible) = 0;
    virtual bool isVisible() const = 0;
};

/**
 * Button component
 */
class Button : public Component {
public:
    Button(const std::string& text);
    ~Button() override;
    
    void render() override;
    void setVisible(bool visible) override;
    bool isVisible() const override;
    
    void setText(const std::string& text);
    std::string getText() const;
    
    void setOnClick(std::function<void()> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Container for UI components
 */
class Container : public Component {
public:
    Container();
    ~Container() override;
    
    void render() override;
    void setVisible(bool visible) override;
    bool isVisible() const override;
    
    void addChild(std::shared_ptr<Component> child);
    void removeChild(std::shared_ptr<Component> child);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace obsidian::ui

