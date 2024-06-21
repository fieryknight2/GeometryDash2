/* Created by Matthew Brown on 6/21/2024 */
#include "game/Player.h"

#include <cmath>
#include <format>
#include "simplelogger.hpp"

#include "GeometryDash.h"

void PlayerAnimator::update()
{
    dtCounter += GeometryDash::getInstance().getDeltaTime().asSeconds();

    if (dtCounter >= 1.0 / frameRate)
    {
        currentFrame++;
        if (currentFrame >= (texFrameCount.x * texFrameCount.y) or currentFrame > maxFrame)
        {
            currentFrame = minFrame;
        }
        dtCounter = 0;
    }
}

sf::IntRect &PlayerAnimator::render()
{
    if (texFrameCount == sf::Vector2i(0, 0))
    {
        rect.left = 0;
        rect.top = 0;
        rect.height = size.y;
        rect.width = size.x;

        return rect;
    }

    const sf::Vector2i frame(currentFrame % texFrameCount.x, std::floor(currentFrame / texFrameCount.x));

    rect.left = frame.x * size.x + padding.x * frame.x;
    rect.top = frame.y * size.y + padding.y * frame.y;
    rect.height = size.y;
    rect.width = size.x;

    return rect;
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
    m_position.y += GeometryDash::getInstance().getDeltaTime().asSeconds() * 50;
    m_sprite.setPosition(m_position);
    if (ArenaItem *collide = arena.collidePlayer(m_sprite.getGlobalBounds()); collide != nullptr)
    {
        SL_LOGF_DEBUG("Player collided with an object: {}", collide->getId());
    }
}

void Player::render()
{
    m_sprite.setPosition(m_position);
    m_sprite.setTextureRect(m_animator.render());
    GeometryDash::getInstance().getWindow().getWindow().draw(m_sprite);
}
