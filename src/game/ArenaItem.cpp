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

bool ArenaItem::collides(const sf::FloatRect &shape)
{
    m_sprite.setPosition(m_position - m_relativePosition);
    const bool collides = m_sprite.getGlobalBounds().intersects(shape);
    if (collides and m_onCollision != nullptr)
    {
        m_onCollision();
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

void ArenaItem::render(const sf::Vector2f &cameraPos)
{
    // SL_LOG_DEBUG(std::format("Rendering item at location ({}, {})", m_position.x - m_relativePosition.x,
    //                          m_position.y - m_relativePosition.y));
    m_sprite.setPosition(m_position - m_relativePosition + cameraPos);
    // m_sprite.setOrigin(sf::Vector2f(m_size.x / 2, m_size.y / 2));
    m_sprite.setTexture(*m_texture);

    const sf::Vector2i frame(m_currentFrame % m_texFrameCount.x, std::floor(m_currentFrame / m_texFrameCount.x));
    sf::IntRect rect{sf::Vector2i(frame.x * static_cast<int>(m_size.x), frame.y * static_cast<int>(m_size.y)) +
                             sf::Vector2i(m_padding.x * frame.x, m_padding.y * frame.y),
                     sf::Vector2i(static_cast<int>(m_size.x), static_cast<int>(m_size.y))};

    const sf::Vector2i texSize(static_cast<int>(m_size.x * static_cast<float>(m_texFrameCount.x)),
                               static_cast<int>(m_size.y * static_cast<float>(m_texFrameCount.y)));

    // TODO: Fix flipping not working
    // m_sprite.setScale(1, 1);
    // if (m_flippedDiagonally)
    // {
    //     m_sprite.setScale(1, -1);
    // }
    // else if (m_flippedHorizontally)
    // {
    //     m_sprite.setScale(-1, 1);
    // }
    // else if (m_flippedVertically)
    // {
    //     m_sprite.setScale(1, -1);
    // }

    // if (m_flippedDiagonally)
    // {
    //     rect.top = texSize.y - rect.top;
    //     rect.left = texSize.x - rect.left;
    //
    //     rect.height = -rect.height;
    //     rect.width = -rect.width;
    //
    //     // SL_LOGF_DEBUG("Rendering frame {}, loc ({}, {}, {}, {})", m_currentFrame, rect.top, rect.left, rect.width,
    //     //               rect.height);
    // }
    // else if (m_flippedHorizontally)
    // {
    //     rect.left = texSize.x - rect.left;
    //     rect.width = -rect.width;
    // }
    // else if (m_flippedVertically)
    // {
    //     rect.top = texSize.y - rect.top;
    //     rect.height = -rect.height;
    // }
    m_sprite.setTextureRect(rect);


    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
}
