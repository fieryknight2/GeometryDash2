/* Created by Matthew Brown on 6/20/2024 */
#include "game/ArenaItem.h"

#include <cmath>
#include <format>

#include "GeometryDash.h"
#include "simplelogger.hpp"

uint64_t ArenaItem::s_idCounter = 0;

ArenaItem::ArenaItem(const std::shared_ptr<sf::Texture> &texture, const sf::Vector2f &position,
                     const sf::Vector2f &size, const sf::Vector2i &frameCount, const sf::Vector2i &padding,
                     const int frame) :
    m_id(s_idCounter++), m_sprite(*texture), m_texture(texture), m_position(position), m_size(size),
    m_texFrameCount(frameCount), m_minFrame(0), m_maxFrame(0), m_frameRate(0), m_currentFrame(frame), m_padding(padding)
{
}

void ArenaItem::regenCollider()
{
    enum Direction
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Direction d = UP;
    if (m_currentFrame == 0)
        d = UP;
    else if (m_currentFrame == 1)
        d = DOWN;
    else if (m_currentFrame == 2)
        d = LEFT;
    else if (m_currentFrame == 3)
        d = RIGHT;

    // Deal with flipping - On second thought, I'm not going to bother
    // if (m_flippedHorizontally and d == UP or m_flippedHorizontally and d == DOWN)
    //     d = (d == DOWN) ? UP : DOWN;
    // if (m_flippedVertically and d == LEFT or m_flippedVertically and d == RIGHT)
    //     d = (d == RIGHT) ? LEFT : RIGHT;

    if (m_type == ArenaItemType::Spike)
    {
        sf::Vector2f leftPoint, rightPoint, topPoint;
        switch (d)
        {
            case LEFT:
                leftPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y);
                topPoint = sf::Vector2f(m_position.x, m_position.y + (m_size.y / 2));
                break;
            case RIGHT:
                leftPoint = sf::Vector2f(m_position.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x, m_position.y);
                topPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y + (m_size.y / 2));
                break;
            case UP:
                leftPoint = sf::Vector2f(m_position.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y + m_size.y);
                topPoint = sf::Vector2f(m_position.x + (m_size.x / 2), m_position.y);
                break;
            case DOWN:
                leftPoint = sf::Vector2f(m_position.x, m_position.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y);
                topPoint = sf::Vector2f(m_position.x + (m_size.x / 2), m_position.y + m_size.y);
                break;
        }
        m_collision = std::make_shared<TriangleCollider>(leftPoint, rightPoint, topPoint);
    }
    else if (m_type == ArenaItemType::TinySpike)
    {
        sf::Vector2f leftPoint, rightPoint;
        sf::Vector2f topPoint{m_position.x + (m_size.x / 2), m_position.y + (m_size.y / 2)};
        switch (d)
        {
            case LEFT:
                leftPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y);
                break;
            case RIGHT:
                leftPoint = sf::Vector2f(m_position.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x, m_position.y);
                break;
            case UP:
                leftPoint = sf::Vector2f(m_position.x, m_position.y + m_size.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y + m_size.y);
                break;
            case DOWN:
                leftPoint = sf::Vector2f(m_position.x, m_position.y);
                rightPoint = sf::Vector2f(m_position.x + m_size.x, m_position.y);
                break;
        }
        m_collision = std::make_shared<TriangleCollider>(leftPoint, rightPoint, topPoint);
    }
    else
    {
        m_collision = std::make_shared<RectangleCollider>(m_position, m_size);
    }
}

void ArenaItem::setType(const ArenaItemType type)
{
    m_type = type;
    regenCollider();
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

void ArenaItem::setColliderPos()
{
    m_collision->setPosition(m_sprite.getPosition());

    if (m_type == ArenaItemType::TinySpike)
    {
        if (m_currentFrame == 0)
        {
            m_collision->setPosition(m_sprite.getPosition() + sf::Vector2f(0, m_size.y / 2));
        }
    }
}

bool ArenaItem::collides(const sf::FloatRect &shape)
{
    m_sprite.setScale(1, 1);
    m_sprite.setRotation(0);
    m_sprite.setPosition(m_position - m_relativePosition);

    setColliderPos();
    SL_ASSERT(m_collision != nullptr, "No collider for this arena item");
    if (!m_collision)
        return false;

    if (m_collision->collides(shape))
    {
        if (m_onCollision != nullptr)
        {
            m_onCollision();
        }

        return true;
    }

    return false;
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

    if (GeometryDash::RenderCollisionShapes)
    {
        if (m_type == ArenaItemType::TinySpike or m_type == ArenaItemType::Spike)
        {
            setColliderPos();
            sf::ConvexShape shape(3);
            shape.setPoint(0, std::dynamic_pointer_cast<TriangleCollider>(m_collision)->getLeftPoint());
            shape.setPoint(1, std::dynamic_pointer_cast<TriangleCollider>(m_collision)->getRightPoint());
            shape.setPoint(2, std::dynamic_pointer_cast<TriangleCollider>(m_collision)->getTopPoint());
            shape.setFillColor(sf::Color(20, 20, 20, 20));
            shape.setOutlineColor(sf::Color::Blue);
            shape.setOutlineThickness(1);
            // shape.setPosition(m_sprite.getPosition());
            GeometryDash::getInstance().getWindow().getWindow().draw(shape);
        }
        else
        {
            // sf::RectangleShape shape(m_size);
            // shape.setPosition(m_sprite.getPosition());
            // shape.setFillColor(sf::Color::Transparent);
            // shape.setOutlineColor(sf::Color::Blue);
            // shape.setOutlineThickness(1);
            // GeometryDash::getInstance().getWindow().getWindow().draw(shape);
        }
    }
}
