/*
 * Window.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <SFML/Graphics.hpp>
#include <utility>

struct WindowSettings
{
    sf::Vector2i size;
    std::string title;
    bool vsync = true;
    uint32_t flags = sf::Style::Default;

    WindowSettings() = default;
    WindowSettings(const sf::Vector2i &size, std::string title, const bool vsync,
                   const uint32_t flags = sf::Style::Default) :
        size(size), title(std::move(title)), vsync(vsync), flags(flags)
    {
    }
};

class Window
{
public:
    Window() = default;
    ~Window() = default;

    void create(const WindowSettings &settings);

    void clear();
    void render();

    void close();
    void destroy();

    sf::RenderWindow &getWindow() { return m_window; }
    bool isOpen() const { return m_window.isOpen(); }

    sf::Color getClearColor() const { return m_clearColor; }
    void setClearColor(const sf::Color &color) { m_clearColor = color; }

private:
    sf::Color m_clearColor{0, 0, 0};

    sf::RenderWindow m_window;
    sf::VideoMode m_mode;
};
