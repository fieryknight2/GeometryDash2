/* Created by Matthew Brown on 6/21/2024 */
#include "game/Player.h"

#include <cmath>
#include <format>
#include "simplelogger.hpp"

#include "GeometryDash.h"

void PlayerAnimator::update()
{
    m_dtCounter += GeometryDash::getInstance().getDeltaTime().asSeconds();

    if (m_dtCounter >= 1.0 / m_frameRate)
    {
        m_currentFrame++;
        if (m_currentFrame >= (m_texFrameCount.x * m_texFrameCount.y) or m_currentFrame > m_maxFrame)
        {
            m_currentFrame = m_minFrame;
        }
        m_dtCounter = 0;
    }
}

sf::IntRect &PlayerAnimator::render()
{
    if (m_texFrameCount == sf::Vector2i(0, 0))
    {
        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.height = m_size.y;
        m_rect.width = m_size.x;

        return m_rect;
    }

    const sf::Vector2i frame(m_currentFrame % m_texFrameCount.x, std::floor(m_currentFrame / m_texFrameCount.x));

    m_rect.left = frame.x * m_size.x + m_padding.x * frame.x;
    m_rect.top = frame.y * m_size.y + m_padding.y * frame.y;
    m_rect.height = m_size.y;
    m_rect.width = m_size.x;

    return m_rect;
}

Player::Player(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size,
               const PlayerAnimator &animator) :
    m_sprite(texture), m_texture(texture), m_position(position), m_size(size), m_animator(animator)
{
}

void Player::update(Arena &arena)
{
    m_animator.update();

    // Temp
    m_sprite.setPosition(m_position);

    m_acceleration += GRAVITY * GeometryDash::getInstance().getDeltaTime().asSeconds();
    m_acceleration = std::clamp(m_acceleration, -MAX_ACCELERATION, MAX_ACCELERATION);
    m_velocity += m_acceleration * GeometryDash::getInstance().getDeltaTime().asSeconds();
    m_velocity = std::clamp(m_velocity, -MAX_VELOCITY, MAX_VELOCITY);

    if (const ArenaItem *collide = arena.collidePlayer(m_sprite.getGlobalBounds()); collide != nullptr)
    {
        // Either dead or on the floor
        if (m_velocity < 0)
        {
            // Going up, death
            // m_isDead = true;
        }

        sf::Vector2f posC = m_sprite.getPosition();
        posC.y = (collide->getPosition() - collide->getRelativePosition()).y - m_size.y;
        m_position = posC;

        // SL_LOGF_DEBUG("Player collided with an object: {}", collide->getId());
        if (m_acceleration > 0)
        {
            m_velocity = 0;
            m_acceleration = 0;
        }

        m_onGround = true;
    }
    else
    {
        if (m_velocity < 0)
        {
            // Going up, death
            // m_isDead = true;
        }

        m_onGround = false;
        m_position.y += m_velocity * GeometryDash::getInstance().getDeltaTime().asSeconds();
        m_sprite.setPosition(m_position);
        if (collide = arena.collidePlayer(m_sprite.getGlobalBounds()); collide != nullptr)
        {
            // Hit the floor
            sf::Vector2f posC = m_sprite.getPosition();
            posC.y = (collide->getPosition() - collide->getRelativePosition()).y - m_size.y;
            m_position = posC;

            // SL_LOGF_DEBUG("Player collided with an object: {}", collide->getId());
            m_velocity = 0;
            m_acceleration = 0;

            m_onGround = true;
        }
    }
}

void Player::render()
{
    m_sprite.setPosition(m_position);
    m_sprite.setTextureRect(m_animator.render());
    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
}

void Player::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            // Jump
            if (m_onGround)
            {
                SL_LOG_DEBUG("Jumping");
                m_acceleration = JUMP_SPEED;
                m_velocity = JUMP_VELOCITY;
                m_onGround = false;
            }
        }
    }
}
