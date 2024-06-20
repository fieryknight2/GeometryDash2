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
    uint32_t flags = sf::Style::Default;

    WindowSettings() = default;
    WindowSettings(const sf::Vector2i &size, std::string title, const uint32_t flags = sf::Style::Default) :
        size(size), title(std::move(title)), flags(flags)
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

private:
    sf::RenderWindow m_window;
    sf::VideoMode m_mode;
};
