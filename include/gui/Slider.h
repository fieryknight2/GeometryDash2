/*
 * Slider.h
 * @author Matthew Brown
 * @date 6/23/2024
 */
#pragma once

#include <utility>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Window/Event.hpp"

struct SliderStyle
{
    explicit SliderStyle(const float bllRds = 10.0f, const sf::Color &dgColor = sf::Color::Black,
                         const float bBorder = 0.0f, const float sThickness = 0.0f,
                         const sf::Color &sdrColor = sf::Color::White, const float border = 0.0f,
                         const sf::Color &txtColor = sf::Color::White, const float txtSize = 0.0f,
                         const sf::Font &fnt = sf::Font(), const bool dMinMax = true, const bool dValue = true,
                         float textPddg = 0.0f, float rnding = 0.0f, const bool dsOnTop = false) :
        ballRadius(bllRds), ballBorderThickness(bBorder), borderThickness(border), sliderThickness(sThickness),
        sliderColor(sdrColor), ballColor(dgColor), textColor(txtColor), textSize(txtSize), font(fnt),
        displayMinMax(dMinMax), displayValue(dValue), textPadding(textPddg), rounding(rnding),
        displayValueOnTop(dsOnTop)
    {
    }

    float ballRadius = 10.0f;
    float ballBorderThickness = 0.0f;
    float borderThickness = 0.1f;
    float sliderThickness = 10.0f;
    sf::Color borderColor;
    sf::Color sliderColor;
    sf::Color ballColor;
    sf::Color textColor;
    float textSize = 0.0f;
    sf::Font font{};
    bool displayMinMax = true;
    bool displayValue = true;
    float textPadding = 0.0f;
    float rounding = 0.0f;
    bool displayValueOnTop = false;
};

class Slider
{
public:
    Slider(const sf::Vector2f &position, const float &length, SliderStyle style) :
        m_ball(sf::CircleShape()), m_style(std::move(style)), m_length(length)
    {
        m_position = position;
    }
    Slider() = default;

    void render();
    void handleEvent(const sf::Event &event);

    void setLength(const float &length) { m_length = length; }
    void setPosition(const sf::Vector2f &position) { m_position = position; }
    void setStyle(const SliderStyle &style) { m_style = style; }
    void setValue(const float value) { m_value = value; }
    void setMinValue(const float min) { m_min = min; }
    void setMaxValue(const float max) { m_max = max; }


    float getLength() const { return m_length; }
    sf::Vector2f getPosition() const { return m_position; }
    SliderStyle getStyle() const { return m_style; }
    float getValue() const { return m_value; }
    float getMinValue() const { return m_min; }
    float getMaxValue() const { return m_max; }

private:
    sf::CircleShape m_ball;
    sf::RectangleShape m_rect;
    SliderStyle m_style;

    sf::Text m_minText;
    sf::Text m_maxText;
    sf::Text m_valueText;

    sf::Vector2f m_position;

    float m_value = 0.0f;
    float m_min = 0.0f;
    float m_max = 0.0f;

    float m_length = 0.0f;

    bool m_isDragging = false;
};
