/*
 * Player.h
 * @author Matthew Brown
 * @date 6/21/2024
 */
#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "game/Arena.h"

class PlayerAnimator
{
public:
    PlayerAnimator(const sf::Vector2f &sz, const sf::Vector2i frameCount, const int minFm, const int maxFm,
                   const int frameRate, const sf::Vector2i &pd) :
        size(sz), texFrameCount(frameCount), minFrame(minFm), maxFrame(maxFm), frameRate(frameRate), padding(pd)
    {
    }
    PlayerAnimator() = default;
    void update();
    [[nodiscard]] sf::IntRect &render();

    [[nodiscard]] sf::Vector2i getSize() const { return size; }
    [[nodiscard]] int getMinFrame() const { return minFrame; }
    [[nodiscard]] int getMaxFrame() const { return maxFrame; }
    [[nodiscard]] int getFrameRate() const { return frameRate; }

private:
    sf::IntRect rect{};
    double dtCounter = 0;

    sf::Vector2i size{};
    sf::Vector2i texFrameCount{};
    int minFrame = 0;
    int maxFrame = 0;
    int frameRate = 0;
    int currentFrame = 0;
    sf::Vector2i padding{};
};

class Player
{
public:
    Player() = default;
    Player(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size,
           const PlayerAnimator &animator);
    ~Player() = default;

    void update(Arena &arena);
    void render();

    [[nodiscard]] sf::Vector2f getPosition() const { return m_position; }
    [[nodiscard]] sf::Vector2f getSize() const { return m_size; }

    [[nodiscard]] PlayerAnimator &getAnimator() { return m_animator; }

    [[nodiscard]] sf::FloatRect getBounds() const { return m_sprite.getGlobalBounds(); }

    [[nodiscard]] bool isDead() const { return m_isDead; }

private:
    bool m_isDead = false;

    sf::Sprite m_sprite{};
    sf::Texture m_texture{};

    sf::Vector2f m_position{0, 0};
    sf::Vector2f m_size{64, 64};

    PlayerAnimator m_animator;
};
