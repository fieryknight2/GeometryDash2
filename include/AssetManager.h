/*
 * AssetManager.h
 * @author Matthew Brown
 * @date 6/22/2024
 */
#pragma once

#include <unordered_map>
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "game/Arena.h"

class AssetManager
{
public:
    static AssetManager &getInstance();

    ~AssetManager() = default;

    bool loadTexture(const std::string &filePath, const std::string &id);
    /* Loads a texture from a file and uses the filepath as the id */
    bool loadTexture(const std::string &filePath) { return loadTexture(filePath, filePath); }

    [[nodiscard]] sf::Texture &getTexture(const std::string &id);

    bool loadFont(const std::string &filePath, const std::string &id);
    /* Loads a font from a file and uses the filepath as the id */
    bool loadFont(const std::string &filePath) { return loadFont(filePath, filePath); }

    [[nodiscard]] sf::Font &getFont(const std::string &id);


    bool loadLevel(const std::string &filePath, const std::string &id);
    /* Loads a level from a file and uses the filepath as the id */
    bool loadLevel(const std::string &filePath) { return loadLevel(filePath, filePath); }

    [[nodiscard]] Arena &getLevel(const std::string &id);

    void clean();

private:
    static AssetManager S_instance;

    AssetManager() = default;

    std::unordered_map<std::string, sf::Texture> m_textures;
    std::unordered_map<std::string, sf::Font> m_fonts;
    std::unordered_map<std::string, Arena> m_arenas;

    sf::Font m_defaultFont{};
    sf::Texture m_defaultTexture{};
    Arena m_defaultArena{};
};
