/* Created by Matthew Brown on 6/28/2024 */
#include "game/Collision.h"

#include "simplelogger.hpp"

#include <array>
#include <cmath>

RectangleCollider::RectangleCollider(const sf::Vector2f &topLeft, const sf::Vector2f &size)
{
    m_shape.left = topLeft.x;
    m_shape.top = topLeft.y;
    m_shape.height = size.y;
    m_shape.width = size.x;
}

bool RectangleCollider::collides(const sf::FloatRect &shape)
{
    SL_ASSERT(m_shape.width > 0, "Rectangle collider has zero width");
    SL_ASSERT(m_shape.height > 0, "Rectangle collider has zero height");
    return m_shape.intersects(shape);
}

TriangleCollider::TriangleCollider(const sf::Vector2f &leftPoint, const sf::Vector2f &rightPoint,
                                   const sf::Vector2f &topPoint) :
    m_leftPoint(leftPoint), m_rightPoint(rightPoint), m_topPoint(topPoint)
{
    const sf::Vector2f leftToRight = rightPoint - leftPoint;
    const sf::Vector2f leftToTop = topPoint - leftPoint;
    m_leftMinAngle = std::atan2(leftToTop.y, leftToTop.x);
    m_leftMaxAngle = std::atan2(leftToRight.y, leftToRight.x);

    const sf::Vector2f rightToTop = topPoint - rightPoint;
    const sf::Vector2f rightToLeft = -leftToRight;
    m_rightMaxAngle = std::atan2(rightToTop.y, rightToTop.x);
    m_rightMinAngle = std::atan2(rightToLeft.y, rightToLeft.x);
}

bool TriangleCollider::collides(const sf::FloatRect &shape)
{
    if (std::ranges::any_of(std::array<sf::Vector2f, 4>({
                                    sf::Vector2f(shape.left, shape.top),
                                    sf::Vector2f(shape.left + shape.width, shape.top),
                                    sf::Vector2f(shape.left, shape.top + shape.height),
                                    sf::Vector2f(shape.left + shape.width, shape.top + shape.height),
                            }),
                            [this](const sf::Vector2f &point) { return inside(point); }))
        return true;
    if (std::ranges::any_of(std::array<sf::Vector2f, 3>({m_leftPoint, m_rightPoint, m_topPoint}),
                            [shape](const sf::Vector2f &point) { return shape.contains(point); }))
        return true;

    return false;
}


bool TriangleCollider::inside(const sf::Vector2f &point) const
{
    // Angle of slope
    sf::Vector2f angle = point - m_leftPoint;

    SL_ASSERT_FALSE(angle.y == 0 and angle.x == 0, "Triangle collider has zero slope");
    if (float angleSlope = std::atan2(angle.y, angle.x); angleSlope > m_leftMinAngle and angleSlope < m_leftMaxAngle)
    {
        // Only need to check 2 points otherwise it can't be in the third point
        angle = point - m_rightPoint;
        if (angleSlope = std::atan2(angle.y, angle.x); angleSlope > m_rightMinAngle and angleSlope < m_rightMaxAngle)
            return true;
    }

    return false;
}

void TriangleCollider::setPosition(const sf::Vector2f &position)
{
    // Setting position to (m_leftPoint.x, m_topPoint.y)
    const auto offset = sf::Vector2f(position.x - m_leftPoint.x, position.y - m_topPoint.y);
    m_rightPoint = m_rightPoint + offset;
    m_topPoint = sf::Vector2f(m_topPoint.x + offset.x, position.y);
    m_leftPoint = sf::Vector2f(position.x, m_leftPoint.y + offset.y);
}
