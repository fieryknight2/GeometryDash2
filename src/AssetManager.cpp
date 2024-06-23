/* Created by Matthew Brown on 6/22/2024 */
#include "AssetManager.h"

#include "simplelogger.hpp"

AssetManager AssetManager::S_instance{};

AssetManager &AssetManager::getInstance() { return AssetManager::S_instance; }

bool AssetManager::loadTexture(const std::string &filePath, const std::string &id)
{
    SL_LOGF_INFO("Loading texture <{}> with id: {}", filePath, id);
    if (m_textures.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload texture with id <{}>", id);
        return true;
    }

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
    SL_LOGF_INFO("Loading font <{}> with id: {}", filePath, id);
    if (m_fonts.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload font with id <{}>", id);
        return true;
    }

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
    SL_LOGF_INFO("Loading level <{}> with id: {}", filePath, id);
    if (m_arenas.contains(id))
    {
        // Already loaded, don't waste your time
        SL_LOGF_DEBUG("Attempted to reload level with id <{}>", id);
        return true;
    }

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
