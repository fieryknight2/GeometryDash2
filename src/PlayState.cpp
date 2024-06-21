/* Created by Matthew Brown on 6/19/2024 */
#include "PlayState.h"
#include "GeometryDash.h"
#include "simplelogger.hpp"

#include <algorithm>
#include <cmath>
#include <format>

void PlayState::create()
{
    SL_LOG_DEBUG("Loading Mangabey font");
    if (!m_defaultFont.loadFromFile("assets/fonts/mangabey-regular.otf"))
    {
        SL_LOG_FATAL("Failed to load font");
    }
    SL_LOG_DEBUG("Loading icons");
    if (!m_settingsTexture.loadFromFile("assets/icons/Settings.png"))
    {
        SL_LOG_FATAL("Failed to load settings icon");
    }
    if (!m_pauseTexture.loadFromFile("assets/icons/Pause.png"))
    {
        SL_LOG_FATAL("Failed to load pause icon");
    }
    m_settingsTexture.setSmooth(true);
    m_pauseTexture.setSmooth(true);

    const sf::Vector2f windowSize{GeometryDash::getInstance().getWindow().getWindow().getSize()};
    // Default button style
    const ButtonStyle buttonStyle{sf::Color::White,
                                  sf::Color::Transparent,
                                  sf::Color::Black,
                                  sf::Color(170, 170, 170),
                                  sf::Color(70, 70, 70),
                                  2.0f,
                                  0,
                                  m_defaultFont,
                                  1.0f};
    m_pauseButton = IconButton(sf::Vector2f(windowSize.x, 10) - sf::Vector2f(75, 0), sf::Vector2f(50, 50),
                               m_pauseTexture, buttonStyle);
    m_settingsButton = IconButton(sf::Vector2f(windowSize.x, 10) - sf::Vector2f(150, 0), sf::Vector2f(50, 50),
                                  m_settingsTexture, buttonStyle);

    m_fpsCounter.setFont(m_defaultFont);
    m_fpsCounter.setCharacterSize(30);
    m_fpsCounter.setPosition(sf::Vector2f(5, 5));
    m_fpsCounter.setFillColor(sf::Color::White);
    m_fpsCounter.setString("FPS: 0");

    if (!m_arena.loadFromFile("assets/map/tiled/level-1.tmx"))
    {
        SL_LOG_FATAL("Failed to load map");
    }

    m_arena.setViewportSize(sf::Vector2f(GeometryDash::getInstance().getWindow().getWindow().getSize()));
    m_arena.setScrollSpeed(sf::Vector2f(250, 0));

    GeometryDash::getInstance().getWindow().setClearColor(sf::Color::Black);
}

void PlayState::update()
{
    m_avgFPS = static_cast<int>((std::floor(1000 / GeometryDash::getInstance().getDeltaTime().asMilliseconds())) +
                                static_cast<float>(m_avgFPS)) /
               2;
    if (m_updateCount > 0.25)
    {
        m_updateCount = 0.0;
        m_fpsCounter.setString("FPS: " + std::to_string(m_avgFPS));
    }
    else
    {
        m_updateCount += GeometryDash::getInstance().getDeltaTime().asSeconds();
    }

    if (m_isPaused == false)
    {
        m_pauseButton.update();
        m_settingsButton.update();

        // std::ranges::sort(m_gameObjects, [](const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject>
        // &b)
        //                   { return a->getId() < b->getId(); });
        m_arena.update();
    }
    else
    {
        if (m_topState)
        {
            m_topState->update();
        }
    }
}

void PlayState::render()
{
    // Draw FPS counter on top to preserve z index
    GeometryDash::getInstance().getWindow().getWindow().draw(m_fpsCounter);

    m_arena.render();

    m_pauseButton.render();
    m_settingsButton.render();

    if (m_topState)
    {
        m_topState->render();
    }
}

void PlayState::destroy() {}

// std::shared_ptr<GameObject> PlayState::getGameObject(const uint64_t id)
// {
//     const auto value = std::ranges::find_if(m_gameObjects.begin(), m_gameObjects.end(),
//                                             [id](const std::shared_ptr<GameObject> &gc) { return gc->getId() == id;
//                                             });
//     if (value == m_gameObjects.end())
//     {
//         SL_LOG_ERROR(std::format("GameObject with id {} not found", id));
//     }
//
//     return *value;
// }
