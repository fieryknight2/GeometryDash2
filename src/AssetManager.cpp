/* Created by Matthew Brown on 6/22/2024 */
#include "AssetManager.h"

#include <cmath>
#include "simplelogger.hpp"

struct Color
{
    Color() = default;
    Color(const float r, const float g, const float b) : r(r), g(g), b(b) {}

    float r = 0;
    float g = 0;
    float b = 0;
};

Color hueToRGB(float hue, float c, float x)
{
    while (hue < 0.0f)
    {
        hue += 1.0f;
    }
    while (hue > 1.0f)
    {
        hue -= 1.0f;
    }
    // for ease
    const float conv = hue * 360.0f;

    if (conv < 60)
    {
        return {c, x, 0};
    }
    if (60 == conv or conv < 120)
    {
        return {x, c, 0};
    }
    if (120 == conv or conv < 180)
    {
        return {0, c, x};
    }
    if (180 == conv or conv < 240)
    {
        return {0, x, c};
    }
    if (240 == conv or conv < 300)
    {
        return {x, 0, c};
    }
    if (300 == conv or conv < 360)
    {
        return {c, 0, x};
    }

    return {0, 0, 0};
}

sf::Color fromHSL(const float h, const float s, const float l)
{
    /* Requires h, s, and l in the range of [0, 1] */

    // https://www.rapidtables.com/convert/color/hsl-to-rgb.html
    const float c = (1 - std::abs(2.0f * l - 1.0f)) * s;
    const float x = c * (1 - std::abs(std::fmod(h * 6, 2.0f) - 1.0f));
    const float m = l - c / 2;

    // SL_LOGF_DEBUG("h: {}, c: {}, x: {}, m: {}", h, c, x, m);
    Color p = hueToRGB(h, c, x);

    return {static_cast<sf::Uint8>((p.r + m) * 255), static_cast<sf::Uint8>((p.g + m) * 255),
            static_cast<sf::Uint8>((p.b + m) * 255)};
}

AssetManager AssetManager::S_instance{};

AssetManager &AssetManager::getInstance() { return AssetManager::S_instance; }

bool AssetManager::loadTexture(const std::string &filePath, const std::string &id)
{
    if (m_textures.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload texture with id <{}>", id);
        return true;
    }
    SL_LOGF_INFO("Loading texture <{}> with id: {}", filePath, id);

    sf::Texture texture;
    if (!texture.loadFromFile(filePath))
    {
        SL_LOGF_ERROR("Failed to load texture <{}> with id {}", filePath, id);
        return false;
    }

    m_textures[id] = texture;
    return true;
}

sf::Texture &AssetManager::getTexture(const std::string &id)
{
    if (!m_textures.contains(id))
    {
        return m_defaultTexture;
    }

    return m_textures[id];
}


bool AssetManager::loadFont(const std::string &filePath, const std::string &id)
{
    if (m_fonts.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload font with id <{}>", id);
        return true;
    }
    SL_LOGF_INFO("Loading font <{}> with id: {}", filePath, id);

    sf::Font font;
    if (!font.loadFromFile(filePath))
    {
        SL_LOGF_ERROR("Failed to load font <{}> with id {}", filePath, id);
        return false;
    }

    m_fonts[id] = font;
    return true;
}

sf::Font &AssetManager::getFont(const std::string &id)
{
    if (!m_fonts.contains(id))
    {
        return m_defaultFont;
    }

    return m_fonts[id];
}

bool AssetManager::loadLevel(const std::string &filePath, const std::string &id)
{
    if (m_arenas.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload level with id <{}>", id);
        return true;
    }
    SL_LOGF_INFO("Loading level <{}> with id: {}", filePath, id);

    Arena arena;
    if (!arena.loadFromFile(filePath))
    {
        SL_LOGF_ERROR("Failed to load level <{}> with id {}", filePath, id);
        return false;
    }

    m_arenas[id] = arena;
    return true;
}

Arena &AssetManager::getLevel(const std::string &id)
{
    if (!m_arenas.contains(id))
    {
        return m_defaultArena;
    }

    return m_arenas[id];
}

void AssetManager::clean()
{
    m_textures.clear();
    m_fonts.clear();
    m_arenas.clear();
}
