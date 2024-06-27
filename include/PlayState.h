/*
 * PlayState.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <memory>
#include "State.h"
#include "game/Arena.h"
#include "game/Player.h"
#include "gui/Button.h"

class PlayState final : public State
{
public:
    PlayState();
    ~PlayState() override;
    std::string getName() override { return "Play"; }

    void update() override;
    void render() override;
    void handleEvent(const sf::Event &event) override;

    Arena &getArena() { return m_arena; }
    // Player &getPlayer() { return m_player; }

private:
    sf::Text m_fpsCounter;
    sf::Font m_defaultFont;

    bool m_isPaused = false;
    std::shared_ptr<State> m_topState = nullptr;

    IconButton m_pauseButton;
    IconButton m_settingsButton;
    sf::Texture m_settingsTexture;
    sf::Texture m_pauseTexture;
    sf::Texture m_playerTexture;

    sf::Vector2f m_cameraPos;
    sf::Vector2f m_cameraOffset;
    float m_cameraSmoothSpeed = 1;

    Arena m_arena;
    Player m_player{};

    double m_updateCount = 1.0;
    int m_avgFPS = 0;

    void openSettings();
    void openPause();

    float m_hue;
    sf::Color m_backgroundColor{0, 0, 200};
    double m_dt = 0;

    const float m_lightness = 0.8f;
    const float m_saturation = 0.7f;
};
