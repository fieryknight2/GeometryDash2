/*
 * Button.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

struct ButtonStyle
{
    explicit ButtonStyle(const sf::Color bgColor = sf::Color::Black, const sf::Color txtColor = sf::Color::White,
                         const sf::Color brdrColor = sf::Color::White,
                         const sf::Color hvrColor = sf::Color(170, 170, 170),
                         const sf::Color actvColor = sf::Color(70, 70, 70), const float border = 0.0f,
                         const int txtSize = 0, const sf::Font &fnt = sf::Font(), const double pd = 0.0) :
        backgroundColor(bgColor), textColor(txtColor), borderColor(brdrColor), hoverColor(hvrColor),
        activeColor(actvColor), borderThickness(border), textSize(txtSize), font(fnt), padding(pd)
    {
    }

    sf::Color backgroundColor;
    sf::Color textColor;
    sf::Color borderColor;
    sf::Color hoverColor;
    sf::Color activeColor;

    float borderThickness = 0.0;
    int textSize = 0;
    sf::Font font;
    double padding = 0.0;
};

class Button
{
public:
    Button(const sf::Vector2f &position, const sf::Vector2f &size, const std::string &text, const ButtonStyle &style);
    Button();

    void update();
    void render();

    void setText(const std::string &text) { m_text.setString(text); }
    void setSize(const sf::Vector2f &size) { m_shape.setSize(size); }
    void setPosition(const sf::Vector2f &position) { m_shape.setPosition(position); }
    void setStyle(const ButtonStyle &style) { m_style = style; }

    sf::Vector2f getSize() const { return m_shape.getSize(); }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }
    std::string getText() const { return m_text.getString(); }
    ButtonStyle getStyle() const { return m_style; }

    bool isHovered() const { return m_hovered; }
    bool isActive() const { return m_active; }
    bool wasPressed() const { return m_wasPressed; }
    void click()
    {
        m_clickedCallback();
        m_wasPressed = true;
    }

    void setClickedCallback(const std::function<void()> &callback) { m_clickedCallback = callback; }

private:
    sf::Text m_text;
    sf::RectangleShape m_shape;
    ButtonStyle m_style;

    bool m_hovered = false;
    bool m_active = false;
    bool m_wasPressed = false;

    std::function<void()> m_clickedCallback;
};
