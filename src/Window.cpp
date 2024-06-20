/* Created by Matthew Brown on 6/19/2024 */

#include "Window.h"

void Window::create(const WindowSettings &settings)
{
    m_mode = sf::VideoMode(settings.size.x, settings.size.y);
    m_window.create(m_mode, settings.title, settings.flags);
}

void Window::close() { m_window.close(); }

void Window::clear() { m_window.clear(sf::Color::White); }

void Window::render() { m_window.display(); }

void Window::destroy()
{
    if (m_window.isOpen())
    {
        close();
    }
}
