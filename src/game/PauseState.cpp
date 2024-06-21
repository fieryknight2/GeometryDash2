/* Created by Matthew Brown on 6/19/2024 */
#include "game/PauseState.h"
#include "GeometryDash.h"
#include "simplelogger.hpp"

#include <format>

void PauseState::create()
{
    SL_LOG_DEBUG("Loading Mangabey font");
    if (!m_defaultFont.loadFromFile("assets/fonts/mangabey-regular.otf"))
    {
        SL_LOG_FATAL("Failed to load font");
    }

    m_pauseText.setFont(m_defaultFont);
    m_pauseText.setCharacterSize(30);
    m_pauseText.setFillColor(sf::Color::Black);
    m_pauseText.setString("Paused");
    m_pauseText.setPosition(
            sf::Vector2f(static_cast<float>(GeometryDash::getInstance().getWindow().getWindow().getSize().x) / 2 -
                                 m_pauseText.getGlobalBounds().width / 2,
                         static_cast<float>(GeometryDash::getInstance().getWindow().getWindow().getSize().y) / 2 -
                                 m_pauseText.getGlobalBounds().height / 2));
}

void PauseState::render() { GeometryDash::getInstance().getWindow().getWindow().draw(m_pauseText); }
