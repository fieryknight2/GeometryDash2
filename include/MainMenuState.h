/*
 * MainMenuState.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include "State.h"
#include "gui/Button.h"

class MainMenuState final : public State
{
public:
    std::string getName() override { return "MainMenu"; }

    void create() override;
    void update() override;
    void render() override;
    void destroy() override;

private:
    Button m_startButton;
    Button m_optionsButton;
    Button m_exitButton;

    sf::Text m_fpsCounter;
    sf::Font m_defaultFont;

    double m_updateCount = 1.0; // Allow it to update immediately
    int m_avgFPS = 0;
};
