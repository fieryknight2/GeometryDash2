/* Created by Matthew Brown on 6/19/2024 */
#include "gui/Button.h"

#include "GeometryDash.h"
#include "SFML/Window/Mouse.hpp"
#include "simplelogger.hpp"

#include <cmath>
#include <format>
#include <utility>

Button::Button(const sf::Vector2f &position, const sf::Vector2f &size, const std::string &text,
               const ButtonStyle &style) : m_text(text, style.font, style.textSize), m_shape(size), m_style(style)
{
    m_shape.setPosition(position);
    m_shape.setFillColor(m_style.backgroundColor);
    m_shape.setOutlineColor(m_style.borderColor);
    m_shape.setOutlineThickness(m_style.borderThickness);
}

Button::Button() = default;

void Button::update()
{
    if (m_wasPressed)
    {
        m_wasPressed = false;
    }

    bool clicked = false;
    m_hovered = false;
    if (m_shape.getGlobalBounds().contains(
                sf::Vector2f(sf::Mouse::getPosition(GeometryDash::getInstance().getWindow().getWindow()))))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            clicked = true;
        }
        else
        {
            m_hovered = true;
        }
    }
    if (m_active == true and clicked == false)
    {
        m_wasPressed = true;
        if (m_clickedCallback)
        {
            m_clickedCallback();
        }
    }
    m_active = clicked;
}

void Button::render()
{
    if (m_active)
    {
        m_shape.setFillColor(m_style.activeColor);
    }
    else if (m_hovered)
    {
        m_shape.setFillColor(m_style.hoverColor);
    }
    else
    {
        m_shape.setFillColor(m_style.backgroundColor);
    }

    m_shape.setOutlineColor(m_style.borderColor);
    m_shape.setOutlineThickness(m_style.borderThickness);

    // Text settings
    m_text.setFont(m_style.font);
    m_text.setString(m_text.getString());
    m_text.setFillColor(m_style.textColor);
    if (m_style.textSize > 0)
    {
        // If the text size is set, ignore the padding
        m_text.setCharacterSize(m_style.textSize);
    }
    else
    {
        // Calculate the size of the text
        const int ySize = std::floor(m_shape.getSize().y - m_style.padding);
        SL_ASSERT(ySize > 0, "Text can't fit inside the button");
        m_text.setCharacterSize(static_cast<unsigned int>(ySize)); // static cast to avoid warning
    }

    // Center the text
    m_text.setOrigin(m_text.getGlobalBounds().getSize() / 2.0f + m_text.getLocalBounds().getPosition());
    m_text.setPosition(m_shape.getPosition() + (m_shape.getSize() / 2.0f));

    // SL_LOG_DEBUG(std::format("x: {}, y: {}", m_text.getPosition().x, m_text.getPosition().y));
    // Render the button
    GeometryDash::getInstance().getWindow().getWindow().draw(m_shape);
    GeometryDash::getInstance().getWindow().getWindow().draw(m_text);
}

IconButton::IconButton(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Texture &texture,
                       ButtonStyle style) : m_sprite(texture), m_shape(size), m_style(std::move(style))
{
    m_shape.setPosition(position);
    m_shape.setFillColor(m_style.backgroundColor);
    m_shape.setOutlineColor(m_style.borderColor);
    m_shape.setOutlineThickness(m_style.borderThickness);
}

IconButton::IconButton() = default;

void IconButton::update()
{
    if (m_wasPressed)
    {
        m_wasPressed = false;
    }

    bool clicked = false;
    m_hovered = false;
    if (m_shape.getGlobalBounds().contains(
                sf::Vector2f(sf::Mouse::getPosition(GeometryDash::getInstance().getWindow().getWindow()))))
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            clicked = true;
        }
        else
        {
            m_hovered = true;
        }
    }
    if (m_active == true and clicked == false)
    {
        if (m_shape.getGlobalBounds().contains(
                    sf::Vector2f(sf::Mouse::getPosition(GeometryDash::getInstance().getWindow().getWindow()))))
        {
            m_wasPressed = true;
            if (m_clickedCallback)
            {
                m_clickedCallback();
            }
        }
    }

    m_active = clicked;
}

void IconButton::render()
{
    if (m_active)
    {
        m_shape.setFillColor(m_style.activeColor);
    }
    else if (m_hovered)
    {
        m_shape.setFillColor(m_style.hoverColor);
    }
    else
    {
        m_shape.setFillColor(m_style.backgroundColor);
    }

    m_shape.setOutlineColor(m_style.borderColor);
    m_shape.setOutlineThickness(m_style.borderThickness);

    // Scale the sprite to fit the button
    m_sprite.setScale((m_shape.getSize().x - static_cast<float>(m_style.padding)) /
                              static_cast<float>(m_sprite.getTextureRect().getSize().x),
                      (m_shape.getSize().y - static_cast<float>(m_style.padding)) /
                              static_cast<float>(m_sprite.getTextureRect().getSize().y));
    m_sprite.setPosition(m_shape.getPosition() +
                         sf::Vector2f(static_cast<float>(m_style.padding), static_cast<float>(m_style.padding)));

    GeometryDash::getInstance().getWindow().getWindow().draw(m_shape);
    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
}

void IconButton::setFrame(const int frame, const sf::Vector2i &frameCount)
{
    SL_ASSERT(frame >= 0, "Frame must be greater than or equal to zero");
    SL_ASSERT(frameCount.x > 0 and frameCount.y > 0, "Frame count must be greater than zero");

    const auto frameSize = sf::Vector2i(static_cast<int>(m_sprite.getTexture()->getSize().x) / frameCount.x,
                                        static_cast<int>(m_sprite.getTexture()->getSize().y) / frameCount.y);
    SL_ASSERT_FALSE(frameSize.x == 0 or frameSize.y == 0, "Frame size of zero is not allowed");

    const auto framePos = sf::Vector2i(frame / frameCount.y, frame / frameCount.x);
    m_sprite.setTextureRect(sf::IntRect(framePos.x * frameSize.x, framePos.y * frameSize.y, frameSize.x, frameSize.y));
}
