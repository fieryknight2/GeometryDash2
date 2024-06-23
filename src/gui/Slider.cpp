/* Created by Matthew Brown on 6/23/2024 */
#include "gui/Slider.h"

#include "AssetManager.h"
#include "GeometryDash.h"

void Slider::render()
{

    m_ball.setFillColor(m_style.ballColor);
    m_ball.setOutlineColor(m_style.borderColor);
    m_ball.setOutlineThickness(m_style.ballBorderThickness);
    m_ball.setRadius(m_style.ballRadius);

    m_minText.setString(std::to_string(static_cast<int>(m_min)));
    m_minText.setFont(m_style.font);
    m_minText.setCharacterSize(static_cast<unsigned int>(m_style.textSize));
    m_minText.setFillColor(m_style.textColor);
    m_minText.setPosition(m_position);
    GeometryDash::getInstance().getWindow().getWindow().draw(m_minText);

    m_rect.setPosition(m_position + sf::Vector2f(m_minText.getGlobalBounds().width + m_style.textPadding,
                                                 m_minText.getGlobalBounds().height / 2));
    m_rect.setSize(sf::Vector2f(m_length, m_style.sliderThickness));
    m_rect.setFillColor(m_style.sliderColor);
    m_rect.setOutlineColor(m_style.borderColor);
    m_rect.setOutlineThickness(m_style.borderThickness);
    GeometryDash::getInstance().getWindow().getWindow().draw(m_rect);

    m_maxText.setString(std::to_string(static_cast<int>(m_max)));
    m_maxText.setFont(m_style.font);
    m_maxText.setCharacterSize(static_cast<unsigned int>(m_style.textSize));
    m_maxText.setFillColor(m_style.textColor);
    m_maxText.setPosition(m_position +
                          sf::Vector2f(m_length + m_style.textPadding * 2 + m_minText.getGlobalBounds().width, 0));
    GeometryDash::getInstance().getWindow().getWindow().draw(m_maxText);

    // Draw the ball last

    m_ball.setPosition(m_rect.getPosition() +
                       sf::Vector2f(m_length * (m_value / m_max) - m_style.ballRadius,
                                    m_minText.getGlobalBounds().height / 2 - m_style.ballRadius));
    GeometryDash::getInstance().getWindow().getWindow().draw(m_ball);

    m_valueText.setString(std::to_string(static_cast<int>(m_value)));
    m_valueText.setFont(m_style.font);
    m_valueText.setCharacterSize(static_cast<unsigned int>(m_style.textSize));
    m_valueText.setFillColor(m_style.textColor);
    m_valueText.setPosition(m_ball.getPosition() +
                            sf::Vector2f(0, (m_style.displayValueOnTop ? -1.0f : 1.0f) *
                                                            (m_style.textPadding * 2 + m_style.borderThickness) +
                                                    (m_style.displayValueOnTop ? -m_style.textSize : 0)));
    GeometryDash::getInstance().getWindow().getWindow().draw(m_valueText);
}

void Slider::handleEvent(const sf::Event &event)
{
    const float minX = m_rect.getPosition().x;
    const float maxX = minX + m_length;
    const auto mousePos = sf::Vector2f(sf::Mouse::getPosition(GeometryDash::getInstance().getWindow().getWindow()));
    const auto windowSize = sf::Vector2f(GeometryDash::getInstance().getWindow().getWindow().getSize());
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (m_ball.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                const float newX = std::clamp(windowSize.x - mousePos.x, minX, maxX);
                m_value = m_max * (maxX - newX) / m_length;
                m_ball.setPosition(sf::Vector2f(maxX - m_value, m_ball.getPosition().y));
                m_isDragging = true;
            }
        }
        if (event.mouseButton.button == sf::Mouse::Right)
            m_isDragging = false;
    }
    else if (event.type == sf::Event::MouseMoved)
    {
        if (m_isDragging)
        {
            const float newX = std::clamp(windowSize.x - mousePos.x, minX, maxX);
            m_value = m_max * (maxX - newX) / m_length;
            m_ball.setPosition(sf::Vector2f(maxX - m_value, m_ball.getPosition().y));
            m_isDragging = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (m_isDragging)
        {
            m_isDragging = false;
        }
    }
}
