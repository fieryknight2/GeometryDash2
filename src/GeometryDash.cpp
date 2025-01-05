/* Created by Matthew Brown on 6/19/2024 */

#include <filesystem>
#include <format>

#include "GeometryDash.h"
#include "simplelogger.hpp"

#include "tinyxml2.h"

GeometryDash GeometryDash::S_instance{};
bool GeometryDash::RenderCollisionShapes = false;
bool GeometryDash::EnableVSync = true;
bool GeometryDash::Restart = false;

#ifndef NDEBUG
bool GeometryDash::EnableDebug = true;
#else
// Change the default debug state to false in release builds
bool GeometryDash::EnableDebug = false;
#endif // NDEBUG

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

void GeometryDash::LoadSettings()
{
    if (!std::filesystem::exists("settings.xml"))
        return; // No settings file, nothing to load

    SL_LOG_INFO("Loading settings");

    tinyxml2::XMLDocument doc;
    if (const tinyxml2::XMLError error = doc.LoadFile("settings.xml"); error != tinyxml2::XML_SUCCESS)
    {
        SL_LOGF_ERROR("Failed to load settings.xml: {}", doc.ErrorIDToName(error));
        return;
    }

    const tinyxml2::XMLElement *root = doc.FirstChildElement("settings");
    if (root == nullptr)
    {
        SL_LOG_ERROR("Failed to find root element in settings.xml");
        return;
    }

    EnableVSync = root->BoolAttribute("EnableVSync");
    EnableDebug = root->BoolAttribute("EnableDebug");
    RenderCollisionShapes = root->BoolAttribute("EnableCollisionShapes");

    SL_LOGF_DEBUG("Settings loaded: EnableVSync={}, EnableDebug={}, EnableCollisionShapes={}", EnableVSync, EnableDebug,
                  RenderCollisionShapes);
}

void GeometryDash::SaveSettings()
{
    SL_LOG_INFO("Saving settings");
    tinyxml2::XMLDocument doc;

    // Create the root element
    tinyxml2::XMLElement *root = doc.NewElement("settings");
    doc.InsertFirstChild(root);

    root->SetAttribute("EnableVSync", EnableVSync);
    root->SetAttribute("EnableDebug", EnableDebug);
    root->SetAttribute("EnableCollisionShapes", RenderCollisionShapes);

    // Actually save the settings
    if (const tinyxml2::XMLError error = doc.SaveFile("settings.xml"); error != tinyxml2::XML_SUCCESS)
        SL_LOGF_ERROR("Failed to save settings.xml: {}", doc.ErrorIDToName(error));
}

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
