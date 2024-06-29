/* Created by Matthew Brown on 6/19/2024 */
#include "PlayState.h"
#include "GeometryDash.h"
#include "game/PauseState.h"
#include "game/SettingsState.h"
#include "simplelogger.hpp"

#include <chrono>
#include <cmath>
#include <format>
#include <random>

#include "AssetManager.h"

PlayState::PlayState()
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

#ifndef NDEBUG
    m_fpsCounter.setFont(m_defaultFont);
    m_fpsCounter.setCharacterSize(30);
    m_fpsCounter.setPosition(sf::Vector2f(5, 5));
    m_fpsCounter.setFillColor(sf::Color::Black);
    m_fpsCounter.setString("FPS: 0");

    m_collisionCounter.setFont(m_defaultFont);
    m_collisionCounter.setCharacterSize(30);
    m_collisionCounter.setPosition(sf::Vector2f(5, 40));
    m_collisionCounter.setFillColor(sf::Color::Black);
    m_collisionCounter.setString("Collisions: 0");

    m_processedCounter.setFont(m_defaultFont);
    m_processedCounter.setCharacterSize(30);
    m_processedCounter.setPosition(sf::Vector2f(5, 75));
    m_processedCounter.setFillColor(sf::Color::Black);
    m_processedCounter.setString("Rendered: 0");
#endif // NDEBUG

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

    m_settingsButton.setClickedCallback([this] { this->openSettings(); });
    m_pauseButton.setClickedCallback([this] { this->openPause(); });

    std::default_random_engine generator{
            static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())};
    std::uniform_real_distribution<> distribution(0.0f, 1.0f);
    m_dt = distribution(generator);
    m_hue = static_cast<float>(std::abs(std::sin(m_dt)));
    m_backgroundColor = fromHSL(m_hue, m_saturation + 0.1f, m_lightness);
    GeometryDash::getInstance().getWindow().setClearColor(m_backgroundColor);
}

void PlayState::update()
{
    m_pauseButton.update();
    m_settingsButton.update();

    if (m_topState and m_topState->getName() == std::string("Settings"))
    {
        m_topState->update();
        const auto temp = std::dynamic_pointer_cast<SettingsState>(m_topState);

        if (temp->continueGame())
        {
            m_topState = nullptr;
            m_isPaused = false;
        }
        if (temp->restartGame())
        {
            GeometryDash::getInstance().changeState(std::make_unique<PlayState>());
        }
        if (temp->toMenu())
        {
            GeometryDash::getInstance().changeState(std::make_unique<MainMenuState>());
        }
    }
    else if (m_topState)
    {
        m_topState->update();
    }

    if (m_isPaused == false)
    {
        // std::ranges::sort(m_gameObjects, [](const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject>
        // &b)
        //                   { return a->getId() < b->getId(); });
        m_arena.update();
        m_player.update(m_arena);

#ifndef NDEBUG
        m_collisionCounter.setString("Collisions: " + std::to_string(m_arena.getCollisionsThisFrame()));
        m_processedCounter.setString("Rendered: " + std::to_string(m_arena.getRenderedLastFrame()));
#endif // NDEBUG

        if (m_player.isDead() or
            m_player.getPosition().y >
                    static_cast<float>(GeometryDash::getInstance().getWindow().getWindow().getSize().y))
        {
            // Restart

            // Temp
            GeometryDash::getInstance().changeState(std::make_unique<PlayState>());
        }

        const float lerpSpeed =
                std::min(1.0f, m_cameraSmoothSpeed * GeometryDash::getInstance().getDeltaTime().asSeconds());
        m_cameraPos.y = std::lerp(m_cameraPos.y, -m_player.getPosition().y + m_cameraOffset.y, lerpSpeed);
    }

    // Color

    m_dt += GeometryDash::getInstance().getDeltaTime().asSeconds() / 10;

    if (m_dt > 200)
    {
        m_dt = 0;
    }
    m_hue = static_cast<float>(std::abs(std::sin(m_dt)));

    m_backgroundColor = fromHSL(m_hue, m_saturation + 0.1f, m_lightness);
    GeometryDash::getInstance().getWindow().setClearColor(m_backgroundColor);


#ifndef NDEBUG
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
#endif // NDEBUG
}

void PlayState::openSettings()
{
    if (m_topState)
    {
        m_topState = nullptr;
        m_isPaused = false;
        return;
    }


    m_isPaused = true;
    m_topState = std::make_shared<SettingsState>();
}

void PlayState::openPause()
{
    SL_LOG_DEBUG("Toggling pause");

    if (m_isPaused)
    {
        m_isPaused = false;

        if (m_topState)
        {
            m_topState = nullptr;
        }
    }
    else
    {
        m_isPaused = true;

        m_topState = std::make_shared<PauseState>();
    }
}

void PlayState::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape:
                openPause();

                break;
            default:
                break;
        }
    }

    m_player.handleEvent(event);

    if (m_topState)
    {
        m_topState->handleEvent(event);
    }
}

void PlayState::render()
{
    // Draw FPS counter on top to preserve z index
#ifndef NDEBUG
    if (GeometryDash::EnableDebug)
    {
        GeometryDash::getInstance().getWindow().getWindow().draw(m_fpsCounter);
        GeometryDash::getInstance().getWindow().getWindow().draw(m_collisionCounter);
        GeometryDash::getInstance().getWindow().getWindow().draw(m_processedCounter);
    }
#endif // NDEBUG

    m_arena.render(m_cameraPos, fromHSL(m_hue, m_saturation, m_lightness));
    // m_player.render(m_cameraPos);
    m_player.render(m_cameraPos);

    m_pauseButton.render();
    m_settingsButton.render();

    if (m_isPaused and m_topState)
    {
        m_topState->render();
    }
}

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

PlayState::~PlayState() { GeometryDash::getInstance().getWindow().setClearColor(sf::Color::White); }
