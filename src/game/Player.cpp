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
    if (m_isDead)
    {

        return;
    }

    m_animator.update();

    if (!m_onGround)
    {
        m_sprite.rotate(ROTATION_SPEED * GeometryDash::getInstance().getDeltaTime().asSeconds());
    }
    else
    {
        m_sprite.setRotation(0);
    }

    m_acceleration += GRAVITY * GeometryDash::getInstance().getDeltaTime().asSeconds();
    m_acceleration = std::clamp(m_acceleration, -MAX_ACCELERATION, MAX_ACCELERATION);
    m_velocity += m_acceleration * GeometryDash::getInstance().getDeltaTime().asSeconds();
    m_velocity = std::clamp(m_velocity, -MAX_VELOCITY, MAX_VELOCITY);

    m_sprite.setPosition(m_position);
    if (const ArenaItem *collide = arena.collidePlayer(m_sprite.getGlobalBounds()); collide != nullptr)
    {
        if (collide->getType() == ArenaItemType::Spike or collide->getType() == ArenaItemType::TinySpike)
        {
            m_isDead = true;
            return; // No further processing to be done
        }

        sf::Vector2f posC = m_sprite.getPosition();
        posC.y = (collide->getPosition() - collide->getRelativePosition()).y - m_size.y;

        if (std::abs(posC.y - m_position.y) > DEATH_THRESHOLD)
        {
            m_isDead = true;
            return; // No further processing to be done
        }

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
        m_onGround = false;
        m_position.y += m_velocity * GeometryDash::getInstance().getDeltaTime().asSeconds();
        m_sprite.setPosition(m_position);
        if (collide = arena.collidePlayer(m_sprite.getGlobalBounds()); collide != nullptr)
        {
            if (collide->getType() == ArenaItemType::Spike or collide->getType() == ArenaItemType::TinySpike)
            {
                // SL_LOGF_DEBUG("Player collided with a spike: {}", collide->getId());
                m_isDead = true;
                return; // No further processing to be done
            }

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

    // Handle jumps
    if (m_onGround and m_holdJumpLength != 0)
    {
        // Jump
        // SL_LOG_DEBUG("Jumping");
        m_acceleration = JUMP_SPEED;
        m_velocity = JUMP_VELOCITY;
        m_position.y += m_velocity * GeometryDash::getInstance().getDeltaTime().asSeconds();
        m_onGround = false;
        m_holdJumpLength = 0;
    }
    if (!m_holdingJump and m_holdJumpLength != 0)
    {
        m_holdJumpLength -= GeometryDash::getInstance().getDeltaTime().asSeconds();
        if (m_holdJumpLength <= 0)
        {
            // SL_LOG_DEBUG("Ran out of time");
            // Ran out of time
            m_holdJumpLength = 0;
        }
    }
}

void Player::render(const sf::Vector2f &cameraPos)
{
    m_sprite.setPosition(m_position + cameraPos);
    m_sprite.setTextureRect(m_animator.render());
    m_sprite.setScale(m_size.x / static_cast<float>(m_animator.getSize().x),
                      m_size.y / static_cast<float>(m_animator.getSize().y));
    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
    m_sprite.setPosition(m_position);

    if (GeometryDash::RenderCollisionShapes)
    {
        sf::RectangleShape rect{m_size};
        rect.setPosition(m_position + cameraPos);
        rect.setFillColor(sf::Color(20, 20, 20, 20));
        rect.setOutlineColor(sf::Color::Blue);
        rect.setOutlineThickness(1);
        GeometryDash::getInstance().getWindow().getWindow().draw(rect);
    }
}

void Player::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            m_holdJumpLength = JUMP_THRESHOLD;
            m_holdingJump = true;
        }
    }
    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            m_holdingJump = false;
        }
    }
}
