/*
 * Arena.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include "game/ArenaItem.h"

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class Arena
{
public:
    Arena() = default;
    ~Arena() = default;

    [[nodiscard]] bool loadFromFile(const std::string &filePath);

    void update();
    void render();

    [[nodiscard]] sf::Vector2i getTileSize() const { return m_tileSize; }

    [[nodiscard]] sf::Vector2i getSize() const { return m_size; }
    [[nodiscard]] sf::Vector2f getPosition() const { return m_position; }
    void setPosition(const sf::Vector2f &position) { m_position = position; }
    void setScrollSpeed(const sf::Vector2f &scrollSpeed) { m_scrollSpeed = scrollSpeed; }

    void setViewportSize(const sf::Vector2f &viewport) { m_viewportSize = viewport; }
    sf::Vector2f getViewportSize() const { return m_viewportSize; }

    [[nodiscard]] ArenaItem *getObject(uint64_t id);

    [[nodiscard]] ArenaItem *collidePlayer(const sf::FloatRect &shape);

    void resetPos();

private:
    sf::Vector2i m_size;
    sf::Vector2f m_viewportSize;
    sf::Vector2f m_position;

    sf::Vector2f m_scrollSpeed;

    std::vector<ArenaItem> m_objects;

    // List of tiles in the arena
    std::vector<size_t> m_map;
    sf::Vector2i m_tileSize;

    // ID to Image
    std::unordered_map<int, std::shared_ptr<sf::Texture>> m_textures;

    bool parseLayer(const std::string &layer, sf::Vector2i tileSize);

    // Convienience
    struct Tile
    {
        sf::Vector2i size{};
        std::string texture;
    };

    struct TileSet
    {
        std::string name;
        int firstGid{};
        int tileWidth{};
        int tileHeight{};

        int tileCount = 0;
        int columnCount = 0;
        int padding = 0;

        std::vector<Tile> tiles;
    };

    void createWorld(const TileSet &set);
};
