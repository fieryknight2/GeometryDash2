/*
 * Collision.h
 * @author Matthew Brown
 * @date 6/28/2024
 */
#pragma once

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"

class CollisionBox
{
public:
    virtual ~CollisionBox() = default;
    [[nodiscard]] virtual bool collides(const sf::FloatRect &shape) = 0;
    virtual void setPosition(const sf::Vector2f &position) = 0;
};

class TriangleCollider final : public CollisionBox
{
public:
    TriangleCollider(const sf::Vector2f &leftPoint, const sf::Vector2f &rightPoint, const sf::Vector2f &topPoint);
    [[nodiscard]] bool collides(const sf::FloatRect &shape) override;
    void setPosition(const sf::Vector2f &position) override;

    [[nodiscard]] sf::Vector2f getLeftPoint() const { return m_leftPoint; }
    [[nodiscard]] sf::Vector2f getRightPoint() const { return m_rightPoint; }
    [[nodiscard]] sf::Vector2f getTopPoint() const { return m_topPoint; }

private:
    [[nodiscard]] bool inside(const sf::Vector2f &point) const;

    float m_leftMinAngle = 0;
    float m_leftMaxAngle = 0;
    float m_rightMinAngle = 0;
    float m_rightMaxAngle = 0;

    sf::Vector2f m_leftPoint;
    sf::Vector2f m_rightPoint;
    sf::Vector2f m_topPoint;
};

class RectangleCollider final : public CollisionBox
{
public:
    RectangleCollider(const sf::Vector2f &topLeft, const sf::Vector2f &size);
    [[nodiscard]] bool collides(const sf::FloatRect &shape) override;

    void setPosition(const sf::Vector2f &position) override
    {
        m_shape.left = position.x;
        m_shape.top = position.y;
    }

private:
    sf::FloatRect m_shape;
};
