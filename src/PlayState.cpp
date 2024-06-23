/* Created by Matthew Brown on 6/19/2024 */
#include "PlayState.h"
#include "GeometryDash.h"
#include "game/PauseState.h"
#include "simplelogger.hpp"

#include <cmath>
#include <format>

#include "AssetManager.h"

void PlayState::create()
{
    ArenaItem::resetIds();

    AssetManager::getInstance().loadTexture("assets/player.png", "player");
    AssetManager::getInstance().loadTexture("assets/icons/Settings.png", "settings");
    AssetManager::getInstance().loadTexture("assets/icons/Pause.png", "pause");
    AssetManager::getInstance().loadFont("assets/fonts/mangabey-regular.otf", "mangabey");
    AssetManager::getInstance().loadLevel("assets/map/tiled/level-1.tmx", "level-1");

    m_defaultFont = AssetManager::getInstance().getFont("mangabey");
    m_playerTexture = AssetManager::getInstance().getTexture("player");
    m_settingsTexture = AssetManager::getInstance().getTexture("settings");
    m_pauseTexture = AssetManager::getInstance().getTexture("pause");

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
    m_fpsCounter.setFillColor(sf::Color::Black);
    m_fpsCounter.setString("FPS: 0");

    m_arena = AssetManager::getInstance().getLevel("level-1");

    m_arena.resetPos();
    m_arena.setViewportSize(sf::Vector2f(GeometryDash::getInstance().getWindow().getWindow().getSize()));
    m_arena.setScrollSpeed(sf::Vector2f(250, 0));

    m_player = Player(m_playerTexture, sf::Vector2f(150, 300), sf::Vector2f(32, 32),
                      PlayerAnimator(sf::Vector2f(64, 64), sf::Vector2i(), 0, 0, 10, sf::Vector2i(0, 0)));

    GeometryDash::getInstance().getWindow().setClearColor(sf::Color::White);

    m_cameraPos = sf::Vector2f(0, 0);
    m_cameraOffset =
            sf::Vector2f(0, static_cast<float>(GeometryDash::getInstance().getWindow().getWindow().getSize().y) / 2);
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

    m_pauseButton.update();
    m_settingsButton.update();

    if (m_pauseButton.wasPressed())
    {
        SL_LOG_DEBUG("Toggling pause");
        m_isPaused = !m_isPaused;

        if (m_topState)
        {
            m_topState->destroy();
        }
        else
        {
            m_topState = std::make_unique<PauseState>();
            m_topState->create();
        }
    }

    if (m_isPaused == false)
    {

        // std::ranges::sort(m_gameObjects, [](const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject>
        // &b)
        //                   { return a->getId() < b->getId(); });
        m_arena.update();
        m_player.update(m_arena);

        if (m_player.isDead() or
            m_player.getPosition().y >
                    static_cast<float>(GeometryDash::getInstance().getWindow().getWindow().getSize().y))
        {
            // Restart

            // Temp
            GeometryDash::getInstance().changeState(std::make_unique<PlayState>());
        }
    }

    // TODO: Smooth camera movement
    const float lerpSpeed =
            std::min(1.0f, m_cameraSmoothSpeed * GeometryDash::getInstance().getDeltaTime().asSeconds());
    // m_cameraPos.x = std::lerp(m_cameraPos.x, m_player.getPosition().x - m_cameraOffset.x, lerpSpeed);
    // m_cameraPos.y = std::lerp(m_cameraPos.y, m_player.getPosition().y - m_cameraOffset.y, lerpSpeed);
    // m_cameraPos.y = m_player.getPosition().y - m_cameraOffset.y;
    m_cameraPos.y = std::lerp(m_cameraPos.y, -m_player.getPosition().y + m_cameraOffset.y, lerpSpeed);
}

void PlayState::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape:
                SL_LOG_DEBUG("Toggling pause");
                m_isPaused = !m_isPaused;
                if (m_topState)
                {
                    m_topState->destroy();
                }
                else
                {
                    m_topState = std::make_unique<PauseState>();
                    m_topState->create();
                }

                break;
            default:
                break;
        }
    }

    m_player.handleEvent(event);
}

void PlayState::render()
{
    // Draw FPS counter on top to preserve z index
    GeometryDash::getInstance().getWindow().getWindow().draw(m_fpsCounter);

    m_arena.render(m_cameraPos);
    // m_player.render(m_cameraPos);
    m_player.render(m_cameraPos);

    m_pauseButton.render();
    m_settingsButton.render();

    if (m_isPaused and m_topState)
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
