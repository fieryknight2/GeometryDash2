/* Created by Matthew Brown on 6/20/2024 */
#include "game/ArenaItem.h"

#include <cmath>
#include <format>

#include "GeometryDash.h"
#include "simplelogger.hpp"

bool inside(const sf::Vector2f &point, const sf::FloatRect &rect)
{
    return point.x >= rect.left and point.x <= rect.left + rect.width and point.y >= rect.top and
           point.y <= rect.top + rect.height;
}

enum Direction
{
    NONE,
    LEFT,
    RIGHT,
    DOWN
};

sf::FloatRect rotateRectUp(const sf::FloatRect &rect, const sf::FloatRect &respect, Direction dir)
{
    sf::FloatRect rotatedRect;
    rotatedRect.height = rect.height;
    rotatedRect.width = rect.width;

    const sf::Vector2f origin(respect.left + (respect.width / 2), respect.top + (respect.height / 2));

    switch (dir)
    {
        case LEFT:
            rotatedRect.left = rect.top - origin.y;
            rotatedRect.top = rect.left - origin.x;
            break;
        case RIGHT:
            rotatedRect.left = rect.top - origin.y;
            rotatedRect.top = -(rect.left - origin.x) - rect.width;
            break;
        case DOWN:
            rotatedRect.left = rect.left - origin.x;
            rotatedRect.top = -(rect.top - origin.y) - rect.height;
            break;
        case NONE:
            return rect; // No rotation
    }

    rotatedRect.left += origin.x;
    rotatedRect.top += origin.y;

    return rotatedRect;
}

uint64_t ArenaItem::s_idCounter = 0;

ArenaItem::ArenaItem(const std::shared_ptr<sf::Texture> &texture, const sf::Vector2f &position,
                     const sf::Vector2f &size, const sf::Vector2i &frameCount, const sf::Vector2i &padding,
                     const int frame) :
    m_id(s_idCounter++), m_sprite(*texture), m_texture(texture), m_position(position), m_size(size),
    m_texFrameCount(frameCount), m_minFrame(0), m_maxFrame(0), m_frameRate(0), m_currentFrame(frame), m_padding(padding)
{
}

void ArenaItem::resetIds()
{
    // Make sure it doesn't get out of hand
    s_idCounter = 0;
}

void ArenaItem::setAnimation(const int minFrame, const int maxFrame, const int frameRate)
{
    m_minFrame = minFrame;
    m_maxFrame = maxFrame;
    m_frameRate = frameRate;
}

bool ArenaItem::collideTallSpike(const sf::FloatRect &shape) const
{
    const sf::FloatRect m_shape = m_sprite.getGlobalBounds();
    // Quick optimization
    if (!m_shape.intersects(shape))
    {
        return false;
    }

    // Process with the spike pointing up
    Direction dir;
    switch (m_currentFrame)
    {
        case 0:
            dir = NONE;
            break;
        case 1:
            dir = DOWN;
            break;
        case 2:
            dir = LEFT;
            break;
        default:
            dir = RIGHT;
            break;
    }
    const sf::FloatRect rotatedShape = rotateRectUp(shape, m_shape, dir);

    // Middle of the enemy is inside the rotatedShape, collision is guaranteed
    if (inside(rotatedShape.getPosition() + sf::Vector2f{rotatedShape.getSize().x / 2, rotatedShape.getSize().y / 2},
               m_shape))
    {
        return true;
    }
    if (inside({rotatedShape.getPosition().x + rotatedShape.getSize().x,
                rotatedShape.getPosition().y + rotatedShape.getSize().y},
               rotatedShape) and
        !inside({rotatedShape.getPosition().x, rotatedShape.getPosition().y + rotatedShape.getSize().y}, rotatedShape))
    {
        // right corner inside the box
        // Check which side of the box the corner is in
        if (rotatedShape.getPosition().x < m_shape.getPosition().x + (m_shape.getSize().x / 2))
        {
            // Check the position of the corner via the slope?
            if (const sf::Vector2f slope =
                        sf::Vector2f(rotatedShape.getPosition().x + rotatedShape.getPosition().x,
                                     rotatedShape.getPosition().y + rotatedShape.getSize().y) -
                        sf::Vector2f(m_shape.getPosition().x + m_shape.getPosition().x, m_shape.getPosition().y);
                slope.y / slope.x > -1.79) // cot 22.5
            {
                return false;
            }

            return true;
        }

        return true;
    }
    if (inside({rotatedShape.getPosition().x, rotatedShape.getPosition().y + rotatedShape.getSize().y},
               rotatedShape) and
        !inside({rotatedShape.getPosition().x + rotatedShape.getSize().x,
                 rotatedShape.getPosition().y + rotatedShape.getSize().y},
                rotatedShape))
    {
        // left corner inside the box
        if (rotatedShape.getPosition().x > m_shape.getPosition().x + (m_shape.getSize().x / 2))
        {
            if (const sf::Vector2f slope = sf::Vector2f(rotatedShape.getPosition().x,
                                                        rotatedShape.getPosition().y + rotatedShape.getSize().y) -
                                           sf::Vector2f(m_shape.getPosition().x, m_shape.getPosition().y);
                slope.y / slope.x > 0.41) // cot 67.5
            {
                return false;
            }

            return true;
        }

        return true;
    }

    return false;
}

bool ArenaItem::collideSmallSpike(const sf::FloatRect &shape) const
{
    sf::FloatRect m_shape = m_sprite.getGlobalBounds();
    m_shape.height /= 2;
    m_shape.top += m_shape.height;

    // Quick optimization
    if (!m_shape.intersects(shape))
    {
        return false;
    }

    // Process with the spike pointing up
    Direction dir;
    switch (m_currentFrame)
    {
        case 0:
            dir = NONE;
            break;
        case 1:
            dir = DOWN;
            break;
        case 2:
            dir = LEFT;
            break;
        default:
            dir = RIGHT;
            break;
    }
    const sf::FloatRect rotatedShape = rotateRectUp(shape, m_shape, dir);

    // Middle of the enemy is inside the rotatedShape, collision is guaranteed
    if (inside(rotatedShape.getPosition() + sf::Vector2f{rotatedShape.getSize().x / 2, rotatedShape.getSize().y / 2},
               m_shape))
    {
        return true;
    }

    if (inside({rotatedShape.getPosition().x + rotatedShape.getSize().x,
                rotatedShape.getPosition().y + rotatedShape.getSize().y},
               rotatedShape) and
        !inside({rotatedShape.getPosition().x, rotatedShape.getPosition().y + rotatedShape.getSize().y}, rotatedShape))
    {
        // right corner inside the box
        // Check which side of the box the corner is in
        if (rotatedShape.getPosition().x < m_shape.getPosition().x + (m_shape.getSize().x / 2))
        {
            // Check the position of the corner via the slope?
            if (const sf::Vector2f slope =
                        sf::Vector2f(rotatedShape.getPosition().x + rotatedShape.getPosition().x,
                                     rotatedShape.getPosition().y + rotatedShape.getSize().y) -
                        sf::Vector2f(m_shape.getPosition().x + m_shape.getPosition().x, m_shape.getPosition().y);
                slope.y / slope.x > -1.79) // cot 22.5
            {
                return false;
            }

            return true;
        }

        return true;
    }
    if (inside({rotatedShape.getPosition().x, rotatedShape.getPosition().y + rotatedShape.getSize().y},
               rotatedShape) and
        !inside({rotatedShape.getPosition().x + rotatedShape.getSize().x,
                 rotatedShape.getPosition().y + rotatedShape.getSize().y},
                rotatedShape))
    {
        // left corner inside the box
        if (rotatedShape.getPosition().x > m_shape.getPosition().x + (m_shape.getSize().x / 2))
        {
            if (const sf::Vector2f slope = sf::Vector2f(rotatedShape.getPosition().x,
                                                        rotatedShape.getPosition().y + rotatedShape.getSize().y) -
                                           sf::Vector2f(m_shape.getPosition().x, m_shape.getPosition().y);
                slope.y / slope.x > 0.41) // cot 67.5
            {
                return false;
            }

            return true;
        }

        return true;
    }
    return false;
}

bool ArenaItem::collides(const sf::FloatRect &shape)
{
    m_sprite.setScale(1, 1);
    m_sprite.setRotation(0);
    m_sprite.setPosition(m_position - m_relativePosition);
    bool collides = false;

    const sf::FloatRect m_shape = m_sprite.getGlobalBounds();
    if (m_type == ArenaItemType::Default)
    {
        collides = m_shape.intersects(shape);
        if (collides and m_onCollision != nullptr)
        {
            m_onCollision();
        }
    }
    else if (m_type == ArenaItemType::Spike)
    {
        collides = collideTallSpike(shape);
    }
    else if (m_type == ArenaItemType::TinySpike)
    {
        collides = collideSmallSpike(shape);
    }

    return collides;
}

void ArenaItem::update()
{
    if (m_texFrameCount.x * m_texFrameCount.y > 1 and m_frameRate > 0 and m_minFrame != m_maxFrame)
    {
        m_dtCounter += GeometryDash::getInstance().getDeltaTime().asSeconds();
        if (m_dtCounter >= 1.0 / m_frameRate)
        {
            m_currentFrame++;
            if (m_currentFrame >= (m_texFrameCount.x * m_texFrameCount.y) or m_currentFrame > m_maxFrame)
            {
                m_currentFrame = m_minFrame;
            }
        }
    }
}

void ArenaItem::render(const sf::Vector2f &cameraPos, sf::Color tint)
{
    // SL_LOG_DEBUG(std::format("Rendering item at location ({}, {})", m_position.x - m_relativePosition.x,
    //                          m_position.y - m_relativePosition.y));
    m_sprite.setPosition(m_position - m_relativePosition + cameraPos);
    m_sprite.setTexture(*m_texture);

    m_sprite.setColor(tint);

    const sf::Vector2i frame(m_currentFrame % m_texFrameCount.x, std::floor(m_currentFrame / m_texFrameCount.x));
    const sf::IntRect rect{sf::Vector2i(frame.x * static_cast<int>(m_size.x), frame.y * static_cast<int>(m_size.y)) +
                                   sf::Vector2i(m_padding.x * frame.x, m_padding.y * frame.y),
                           sf::Vector2i(static_cast<int>(m_size.x), static_cast<int>(m_size.y))};

    float scaleX = 1;
    float scaleY = 1;
    if (m_flippedDiagonally)
    {
        // Tiled is weird.
        m_sprite.setRotation(-90);
        m_sprite.setPosition(m_sprite.getPosition() + sf::Vector2f(m_size.x, 0));

        if (m_flippedHorizontally)
        {
            scaleX = -1;
            m_sprite.setPosition(m_sprite.getPosition() + sf::Vector2f(-m_size.x, 0));
        }
        if (m_flippedVertically)
        {
            scaleY = -1;
            m_sprite.setPosition(m_sprite.getPosition() + sf::Vector2f(0, m_size.y));
        }
    }
    else
    {
        if (m_flippedHorizontally)
        {
            scaleX = -1;
            m_sprite.setPosition(m_sprite.getPosition() + sf::Vector2f(m_size.x, 0));
        }
        if (m_flippedVertically)
        {
            scaleY = -1;
            m_sprite.setPosition(m_sprite.getPosition() + sf::Vector2f(0, m_size.y));
        }
    }
    m_sprite.setScale(scaleX, scaleY);

    m_sprite.setTextureRect(rect);

    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
}
