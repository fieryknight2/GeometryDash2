/* Made by Matthew Brown on 6/19/2024 */

#include "MainMenuState.h"
#include "GeometryDash.h"

#include <cmath>

#include "SFML/Graphics/Font.hpp"
#include "simplelogger.hpp"

void MainMenuState::create()
{
    constexpr int buttonWidth = 200;
    constexpr int buttonHeight = 50;

    SL_LOG_DEBUG("Loading Mangabey font");
    if (!m_defaultFont.loadFromFile("assets/fonts/mangabey-regular.otf"))
    {
        SL_LOG_FATAL("Failed to load font");
    }

    // Create the button style
    static ButtonStyle mainMenuButtonStyle{sf::Color::White,
                                           sf::Color::Black,
                                           sf::Color::Black,
                                           sf::Color(170, 170, 170),
                                           sf::Color(70, 70, 70),
                                           2.0f,
                                           0,
                                           m_defaultFont,
                                           15.0f};

    // Create the menu
    const auto size = GeometryDash::getInstance().getWindow().getWindow().getSize();
    const auto center = sf::Vector2f(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);

    m_startButton.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, buttonHeight / 2.0f + buttonHeight * 1.25));
    m_startButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_startButton.setText("Start");
    m_startButton.setStyle(mainMenuButtonStyle);

    m_optionsButton.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, buttonHeight / 2.0f));
    m_optionsButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_optionsButton.setText("Options");
    m_optionsButton.setStyle(mainMenuButtonStyle);

    m_exitButton.setPosition(center + sf::Vector2f(-buttonWidth / 2.0f, buttonHeight * 1.25 - buttonHeight / 2.0f));
    m_exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_exitButton.setText("Quit");
    m_exitButton.setStyle(mainMenuButtonStyle);

    m_fpsCounter.setFont(m_defaultFont);
    m_fpsCounter.setCharacterSize(30);
    m_fpsCounter.setPosition(sf::Vector2f(5, 5));
    m_fpsCounter.setFillColor(sf::Color::Black);
    m_fpsCounter.setString("00");
}

void MainMenuState::update()
{
    // Update the menu
    m_avgFPS =
            static_cast<int>(std::floor(1000 / GeometryDash::getInstance().getDeltaTime().asSeconds())) + m_avgFPS / 2;
    if (m_updateCount > 0.25)
    {
        m_updateCount = 0.0;
        m_fpsCounter.setString("FPS: " + std::to_string(m_avgFPS));
    }
    else
    {
        m_updateCount += GeometryDash::getInstance().getDeltaTime().asSeconds();
    }

    m_startButton.update();
    m_optionsButton.update();
    m_exitButton.update();

    if (m_exitButton.isActive())
    {
        GeometryDash::getInstance().getWindow().close();
    }
}

void MainMenuState::render()
{
    // Render the menu
    m_startButton.render();
    m_optionsButton.render();
    m_exitButton.render();

    GeometryDash::getInstance().getWindow().getWindow().draw(m_fpsCounter);
}

void MainMenuState::destroy()
{
    // Destroy the menu
}
