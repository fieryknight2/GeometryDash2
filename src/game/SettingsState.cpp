/* Created by Matthew Brown on 6/23/2024 */
#include "game/SettingsState.h"

#include "AssetManager.h"
#include "GeometryDash.h"

#include <cmath>


SettingsState::SettingsState()
{
    const sf::Vector2f settingsPanelSize{320, 200};

    AssetManager::getInstance().loadFont("assets/fonts/mangabey-regular.otf", "mangabey");
    AssetManager::getInstance().loadTexture("assets/icons/buttons.png", "pause_settings");
    AssetManager::getInstance().loadTexture("assets/icons/MusicVolume.png", "musicVolume");
    AssetManager::getInstance().loadTexture("assets/icons/SFXVolume.png", "sfxVolume");
    m_defaultFont = AssetManager::getInstance().getFont("mangabey");

    const sf::Vector2f windowSize{GeometryDash::getInstance().getWindow().getWindow().getSize()};
    m_settingsPanel = Panel(sf::Vector2f(windowSize.x / 2, windowSize.y / 2) -
                                    sf::Vector2f(settingsPanelSize.x / 2, settingsPanelSize.y / 2),
                            settingsPanelSize, PanelStyle(sf::Color::White, sf::Color::Black, 2.0f));

    const ButtonStyle buttonStyle{sf::Color::White,
                                  sf::Color::Transparent,
                                  sf::Color::Black,
                                  sf::Color(170, 170, 170),
                                  sf::Color(70, 70, 70),
                                  2.0f,
                                  0,
                                  m_defaultFont,
                                  1.0f};
    sf::Vector2f startPos{m_settingsPanel.getPosition()};

    m_resumeButton = IconButton(startPos + sf::Vector2f(5, 5), sf::Vector2f(64, 64),
                                AssetManager::getInstance().getTexture("pause_settings"), buttonStyle);
    m_resumeButton.setFrame(0, sf::Vector2i(3, 1));
    m_restartButton = IconButton(startPos + sf::Vector2f(settingsPanelSize.x / 2 - 64.0f / 2, 5), sf::Vector2f(64, 64),
                                 AssetManager::getInstance().getTexture("pause_settings"), buttonStyle);
    m_restartButton.setFrame(2, sf::Vector2i(3, 1));
    m_quitButton = IconButton(startPos + sf::Vector2f(settingsPanelSize.x - 69, 5), sf::Vector2f(64, 64),
                              AssetManager::getInstance().getTexture("pause_settings"), buttonStyle);
    m_quitButton.setFrame(1, sf::Vector2i(3, 1));

    SliderStyle sliderStyle{10,
                            sf::Color::White,
                            1.0f,
                            10.0f,
                            sf::Color(170, 170, 170),
                            1.0f,
                            sf::Color::Black,
                            20.0f,
                            AssetManager::getInstance().getFont("mangabey"),
                            true,
                            true,
                            14.0f,
                            0.0f,
                            false};

    m_musicVolumeV.setTexture(AssetManager::getInstance().getTexture("musicVolume"));
    m_musicVolumeV.setPosition(startPos + sf::Vector2f(5, 90));

    m_sfxVolumeV.setTexture(AssetManager::getInstance().getTexture("sfxVolume"));
    m_sfxVolumeV.setPosition(startPos + sf::Vector2f(5, 145));

    m_musicVolumeSlider = Slider(startPos + sf::Vector2f(42, 90), 200, sliderStyle);
    m_musicVolumeSlider.setValue(50);
    m_musicVolumeSlider.setMinValue(0);
    m_musicVolumeSlider.setMaxValue(100);
    m_sfxVolumeSlider = Slider(startPos + sf::Vector2f(42, 145), 200, sliderStyle);
    m_sfxVolumeSlider.setValue(50);
    m_sfxVolumeSlider.setMinValue(0);
    m_sfxVolumeSlider.setMaxValue(100);
}


void SettingsState::update()
{
    m_resumeButton.update();
    m_restartButton.update();
    m_quitButton.update();

    if (m_resumeButton.wasPressed())
    {
        m_continueGame = true;
    }
    if (m_restartButton.wasPressed())
    {
        m_restartGame = true;
    }
    if (m_quitButton.wasPressed())
    {
        m_menu = true;
    }
}

int convert(const int percent)
{
    if (percent < 10)
    {
        return 0;
    }

    if (percent < 40)
        return 1;

    if (percent < 80)
        return 2;

    return 3;
}

void SettingsState::render()
{
    m_settingsPanel.render();

    m_resumeButton.render();
    m_restartButton.render();
    m_quitButton.render();

    const auto vSize = sf::Vector2i(AssetManager::getInstance().getTexture("musicVolume").getSize());
    const auto sSize = sf::Vector2i(AssetManager::getInstance().getTexture("sfxVolume").getSize());
    constexpr int vFrames = 4;
    constexpr int sFrames = 4;

    const int musicLoc =
            static_cast<int>(std::floor(m_musicVolumeSlider.getValue() / m_musicVolumeSlider.getMaxValue() * 100));
    const int sfxLoc =
            static_cast<int>(std::floor(m_sfxVolumeSlider.getValue() / m_sfxVolumeSlider.getMaxValue() * 100));


    m_musicVolumeV.setTextureRect(sf::IntRect(convert(musicLoc) * vSize.x / vFrames, 0, vSize.x / vFrames, vSize.y));
    m_sfxVolumeV.setTextureRect(sf::IntRect(convert(sfxLoc) * sSize.x / sFrames, 0, sSize.x / sFrames, sSize.y));

    m_musicVolumeV.setScale(sf::Vector2f(0.5, 0.5));
    m_sfxVolumeV.setScale(sf::Vector2f(0.5, 0.5));

    GeometryDash::getInstance().getWindow().getWindow().draw(m_musicVolumeV);
    GeometryDash::getInstance().getWindow().getWindow().draw(m_sfxVolumeV);

    m_musicVolumeSlider.render();
    m_sfxVolumeSlider.render();
}

void SettingsState::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        const sf::FloatRect rect{m_settingsPanel.getPosition(), m_settingsPanel.getSize()};
        if (!rect.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
        {
            m_quit = true;
            return;
        }
    }

    m_musicVolumeSlider.handleEvent(event);
    m_sfxVolumeSlider.handleEvent(event);
}
