/*
 * ArenaItem.h
 * @author Matthew Brown
 * @date 6/20/2024
 */
#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

enum class ArenaItemType
{
    Default,
    Spike,
    TinySpike,
};

class ArenaItem
{
public:
    ArenaItem(const std::shared_ptr<sf::Texture> &texture, const sf::Vector2f &position, const sf::Vector2f &size,
              const sf::Vector2i &frameCount, const sf::Vector2i &padding, int frame);
    ~ArenaItem() = default;

    void setAnimation(int minFrame, int maxFrame, int frameRate);

    void update();
    void render(const sf::Vector2f &cameraPos, sf::Color tint);

    void setRelativePosition(const sf::Vector2f &position) { m_relativePosition = position; }
    sf::Vector2f getRelativePosition() const { return m_relativePosition; }

    [[nodiscard]] sf::Vector2f getPosition() const { return m_position; }
    [[nodiscard]] sf::Vector2f getSize() const { return m_size; }

    [[nodiscard]] uint64_t getId() const { return m_id; }

    void setType(const ArenaItemType type) { m_type = type; }
    [[nodiscard]] ArenaItemType getType() const { return m_type; }

    [[nodiscard]] bool collides(const sf::FloatRect &shape);

    // Callbacks for custom collision and update logic
    std::function<void()> getOnCollision() const { return m_onCollision; }
    std::function<void()> getOnUpdate() const { return m_onUpdate; }

    void setOnCollision(const std::function<void()> &onCollision) { m_onCollision = onCollision; }
    void setOnUpdate(const std::function<void()> &onUpdate) { m_onUpdate = onUpdate; }

    void setFlippedHorizontally(const bool flipped) { m_flippedHorizontally = flipped; }
    void setFlippedVertically(const bool flipped) { m_flippedVertically = flipped; }
    void setFlippedDiagonally(const bool flipped) { m_flippedDiagonally = flipped; }

    static void resetIds();

private:
    static uint64_t s_idCounter;
    uint64_t m_id = 0;

    sf::Sprite m_sprite;
    std::shared_ptr<sf::Texture> m_texture;

    sf::Vector2f m_position;
    sf::Vector2f m_relativePosition;
    sf::Vector2f m_size;

    ArenaItemType m_type{ArenaItemType::Default};

    std::function<void()> m_onCollision;
    std::function<void()> m_onUpdate;

    sf::Vector2i m_texFrameCount;
    int m_minFrame;
    int m_maxFrame;
    int m_frameRate;
    int m_currentFrame;
    sf::Vector2i m_padding;
    double m_dtCounter = 0;

    bool m_flippedHorizontally = false;
    bool m_flippedVertically = false;
    bool m_flippedDiagonally = false;

    bool collideTallSpike(const sf::FloatRect &shape) const;
    bool collideSmallSpike(const sf::FloatRect &shape) const;
};
