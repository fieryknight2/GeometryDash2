/* Created by Matthew Brown on 6/19/2024 */

#include <format>

#include "GeometryDash.h"
#include "simplelogger.hpp"

GeometryDash GeometryDash::S_instance{};
bool GeometryDash::RenderCollisionShapes = false;
bool GeometryDash::EnableDebug = true;
bool GeometryDash::EnableVSync = true;
bool GeometryDash::Restart = false;

GeometryDash &GeometryDash::getInstance() { return S_instance; }

void GeometryDash::Reset()
{
    SL_LOG_INFO("Resetting GeometryDash");
    S_instance.getWindow().destroy();
}

void GeometryDash::changeState(const std::shared_ptr<State> &state) noexcept(false)
{
    // Verify
    if (m_state != state and state != nullptr)
    {
        SL_LOGF_DEBUG("Changing state from {} to {}", m_state->getName(), state->getName());
        // Shared pointer automatically deletes the old state
        m_state = state;
    }
}

void GeometryDash::LoadSettings() { EnableVSync = false; }

bool GeometryDash::run() noexcept(false)
{
    SL_LOG_INFO("Starting GeometryDash");
    SL_LOG_INFO("Creating Window");

    m_window.create(WindowSettings({1200, 800}, "Geometry Dash", EnableVSync, sf::Style::Titlebar | sf::Style::Close));

    SL_LOG_DEBUG("Creating Main Menu state");
    m_state = std::make_shared<MainMenuState>();

    m_window.getWindow().setFramerateLimit(120);

    while (m_state and m_window.isOpen())
    {
        m_deltaTime = m_clock.restart();

        // Event handling
        sf::Event event{};
        while (m_window.getWindow().pollEvent(event))
        {
            if (event.type == sf::Event::Closed and !m_state->handleCloseEvent())
            {
                m_window.close();
                break;
            }

            m_state->handleEvent(event);
        }

        m_state->update();
        if (m_state->quit()) // Should the program quit? (no need to render if so)
        {
            m_window.close();
            break;
        }

        // Render the screen
        m_window.clear();

        m_state->render();

        m_window.render();
    }

    SL_LOG_INFO("Exiting GeometryDash");
    SL_LOGF_DEBUG("Quitting from state {}", m_state ? m_state->getName() : "null");
    SL_LOG_DEBUG("Destroying Window");
    m_window.destroy();

    return Restart;
}
