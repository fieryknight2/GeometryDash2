/*
 * SettingsState.h
 * @author Matthew Brown
 * @date 6/23/2024
 */
#pragma once

#include "SFML/Graphics/Text.hpp"
#include "State.h"
#include "gui/Button.h"
#include "gui/Panel.h"
#include "gui/Slider.h"

class SettingsState final : public State
{
public:
    std::string getName() override { return "Settings"; }

    void create() override;
    void update() override;
    void render() override;
    void destroy() override;

    bool continueGame() const { return m_continueGame; }
    bool restartGame() const { return m_restartGame; }
    bool toMenu() const { return m_menu; }

    void handleEvent(const sf::Event &event) override;

private:
    sf::Font m_defaultFont;

    Panel m_settingsPanel;
    IconButton m_resumeButton;
    IconButton m_restartButton;
    IconButton m_quitButton;

    sf::Sprite m_musicVolumeV;
    sf::Sprite m_sfxVolumeV;
    Slider m_musicVolumeSlider;
    Slider m_sfxVolumeSlider;

    bool m_continueGame = false;
    bool m_restartGame = false;
    bool m_menu = false;
};
