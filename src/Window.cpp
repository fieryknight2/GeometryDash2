/* Created by Matthew Brown on 6/19/2024 */

#include "Window.h"
#include "AssetManager.h"

void Window::create(const WindowSettings &settings)
{
    // m_mode = sf::VideoMode::getDesktopMode();
    m_mode = sf::VideoMode(settings.size.x, settings.size.y);
    m_window.create(m_mode, settings.title, settings.flags);
    m_window.setVerticalSyncEnabled(true);
}

void Window::close()
{
    AssetManager::getInstance().clean();
    m_window.close();
}

void Window::clear() { m_window.clear(m_clearColor); }

void Window::render() { m_window.display(); }

void Window::destroy()
{
    if (m_window.isOpen())
    {
        close();
    }
}
