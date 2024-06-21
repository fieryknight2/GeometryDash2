/*
 * PlayState.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <memory>
#include "State.h"
#include "game/Arena.h"
#include "gui/Button.h"

class PlayState final : public State
{
public:
    std::string getName() override { return "Play"; }

    void create() override;
    void update() override;
    void render() override;
    void destroy() override;

    Arena &getArena() { return m_arena; }
    // Player &getPlayer() { return m_player; }

private:
    sf::Text m_fpsCounter;
    sf::Font m_defaultFont;

    bool m_isPaused = false;
    std::unique_ptr<State> m_topState = nullptr;

    IconButton m_pauseButton;
    IconButton m_settingsButton;
    sf::Texture m_settingsTexture;
    sf::Texture m_pauseTexture;

    Arena m_arena;
    // Player m_player;

    double m_updateCount = 1.0;
    int m_avgFPS = 0;
};
