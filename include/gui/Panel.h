/*
 * Panel.h
 * @author Matthew Brown
 * @date 6/23/2024
 */
#pragma once

#include "SFML/Graphics/RectangleShape.hpp"

struct PanelStyle
{
    explicit PanelStyle(const sf::Color &bgColor = sf::Color::Black, const sf::Color &brdrColor = sf::Color::White,
                        const float border = 0.0f) :
        backgroundColor(bgColor), borderColor(brdrColor), borderThickness(border)
    {
    }

    float borderThickness = 0.0;
    sf::Color backgroundColor;
    sf::Color borderColor;
};

class Panel
{
public:
    Panel(const sf::Vector2f &position, const sf::Vector2f &size, const PanelStyle &style) :
        m_shape(sf::RectangleShape(size)), m_style(style)
    {
        m_shape.setPosition(position);
    }
    Panel() = default;

    void render();

    void setSize(const sf::Vector2f &size) { m_shape.setSize(size); }
    void setPosition(const sf::Vector2f &position) { m_shape.setPosition(position); }
    void setStyle(const PanelStyle &style) { m_style = style; }

    sf::Vector2f getSize() const { return m_shape.getSize(); }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }
    PanelStyle getStyle() const { return m_style; }

private:
    sf::RectangleShape m_shape;
    PanelStyle m_style;
};
