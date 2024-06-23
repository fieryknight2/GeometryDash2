/*
 * Player.h
 * @author Matthew Brown
 * @date 6/21/2024
 */
#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window/Event.hpp"

#include "game/Arena.h"

constexpr float GRAVITY = 6800;
constexpr float JUMP_SPEED = 200;
constexpr float JUMP_VELOCITY = -500;
constexpr float MAX_VELOCITY = 800;
constexpr float MAX_ACCELERATION = 1500;
constexpr float DEATH_THRESHOLD = 5;
constexpr float ROTATION_SPEED = 0.001;
constexpr float JUMP_THRESHOLD = 0.005;


class PlayerAnimator
{
public:
    PlayerAnimator(const sf::Vector2f &sz, const sf::Vector2i frameCount, const int minFm, const int maxFm,
                   const int frameRate, const sf::Vector2i &pd) :
        m_size(sz), m_texFrameCount(frameCount), m_minFrame(minFm), m_maxFrame(maxFm), m_frameRate(frameRate),
        m_padding(pd)
    {
    }
    PlayerAnimator() = default;
    void update();
    [[nodiscard]] sf::IntRect &render();

    [[nodiscard]] sf::Vector2i getSize() const { return m_size; }
    [[nodiscard]] int getMinFrame() const { return m_minFrame; }
    [[nodiscard]] int getMaxFrame() const { return m_maxFrame; }
    [[nodiscard]] int getFrameRate() const { return m_frameRate; }

private:
    sf::IntRect m_rect{};
    double m_dtCounter = 0;

    sf::Vector2i m_size{};
    sf::Vector2i m_texFrameCount{};
    int m_minFrame = 0;
    int m_maxFrame = 0;
    int m_frameRate = 0;
    int m_currentFrame = 0;
    sf::Vector2i m_padding{};
};

class Player
{
public:
    Player() = default;
    Player(const sf::Texture &texture, const sf::Vector2f &position, const sf::Vector2f &size,
           const PlayerAnimator &animator);
    ~Player() = default;

    void update(Arena &arena);
    void render(const sf::Vector2f &cameraPos);

    [[nodiscard]] sf::Vector2f getPosition() const { return m_position; }
    [[nodiscard]] sf::Vector2f getSize() const { return m_size; }

    [[nodiscard]] PlayerAnimator &getAnimator() { return m_animator; }

    [[nodiscard]] sf::FloatRect getBounds() const { return m_sprite.getGlobalBounds(); }

    [[nodiscard]] bool isDead() const { return m_isDead; }

    void handleEvent(const sf::Event &event);

private:
    bool m_isDead = false;
    bool m_onGround = false;

    sf::Sprite m_sprite{};
    sf::Texture m_texture{};

    sf::Vector2f m_position{0, 0};
    sf::Vector2f m_size{64, 64};

    // float m_rotation = 0;
    float m_velocity = 0;
    float m_acceleration = 0;

    PlayerAnimator m_animator;

    bool m_holdingJump = false;
    double m_holdJumpLength = 0;
};
