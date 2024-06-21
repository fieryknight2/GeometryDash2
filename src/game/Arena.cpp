/* Created by Matthew Brown on 6/19/2024 */
#include "game/Arena.h"

#include <cmath>
#include <format>
#include <iostream>
#include <ranges>

#include "GeometryDash.h"
#include "simplelogger.hpp"
#include "tinyxml2.h"

// From https://doc.mapeditor.org/en/latest/reference/global-tile-ids/#gid-tile-flipping
constexpr uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
constexpr uint32_t FLIPPED_VERTICALLY_FLAG = 0x40000000;
constexpr uint32_t FLIPPED_DIAGONALLY_FLAG = 0x20000000;
constexpr uint32_t ROTATED_HEXAGONAL_120_FLAG = 0x10000000;

#ifndef OS_SEP

#ifdef WIN32
#define OS_SEP "\\"
#else
#define OS_SEP "/"
#endif

#endif // OS_SEP

std::string getFileFolder(const std::string &filePath)
{
    std::string path = filePath;
    if (const size_t lastSlash = path.find_last_of("\\/"); lastSlash != std::string::npos)
    {
        path = path.substr(0, lastSlash);
    }

    return path;
}

bool Arena::loadFromFile(const std::string &filePath)
{
    tinyxml2::XMLDocument doc;
    SL_LOG_INFO(std::format("Loading file: {}", filePath));
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS)
    {
        SL_LOG_FATAL(std::format("Failed to load file: {}", filePath));
        return false;
    }

    tinyxml2::XMLElement *rootNode = doc.RootElement();
    if (rootNode == nullptr)
    {
        SL_LOG_FATAL("Failed to find root node");
        return false;
    }

    // Process the nodes
    if (rootNode->Name() == std::string("map"))
    {
        SL_LOG_INFO("Loading map");

        if (rootNode->QueryIntAttribute("width", &m_size.x) != tinyxml2::XML_SUCCESS)
        {
            SL_LOG_FATAL("Failed to load width of map");
            return false;
        }
        if (rootNode->QueryIntAttribute("height", &m_size.y) != tinyxml2::XML_SUCCESS)
        {
            SL_LOG_FATAL("Failed to load height of map");
            return false;
        }
        if (rootNode->QueryIntAttribute("tilewidth", &m_tileSize.x) != tinyxml2::XML_SUCCESS)
        {
            SL_LOG_ERROR("Failed to load the Tile Width of map, defaulting to 64");
            m_tileSize.x = 64;
        }
        if (rootNode->QueryIntAttribute("tileheight", &m_tileSize.y) != tinyxml2::XML_SUCCESS)
        {
            SL_LOG_ERROR("Failed to load the Tile Height of map, defaulting to 64");
            m_tileSize.y = 64;
        }
    }
    else
    {
        SL_LOG_FATAL(std::format("File: {} does not contain a map", filePath));
        SL_LOG_DEBUG(std::format("File: {} contains root element {}", filePath, rootNode->Name()));
        return false;
    }

    std::unique_ptr<TileSet> tileSet{nullptr};
    for (const tinyxml2::XMLElement *cNode = rootNode->FirstChildElement(); cNode != nullptr;
         cNode = cNode->NextSiblingElement())
    {
        SL_LOG_DEBUG(std::format("Processing node {}", cNode->Name()));
        if (cNode->Name() == std::string("tileset"))
        {
            if (tileSet != nullptr)
            {
                SL_LOG_WARNING("Only 1 tileset is currently supported, found multiple. Using first one");
                continue;
            }

            SL_LOG_DEBUG(std::format("Loading tile set {}", cNode->Attribute("name")));
            tileSet = std::make_unique<TileSet>();

            const char *nm;
            if (cNode->QueryStringAttribute("name", &nm) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR("Failed to load name of tile set, defaulting to empty string");
                nm = "";
            }
            tileSet->name = nm;

            if (cNode->QueryIntAttribute("firstgid", &tileSet->firstGid) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_FATAL(std::format("Failed to load firstgid of tile set {}", tileSet->name));
                return false;
            }
            if (cNode->QueryIntAttribute("tilewidth", &tileSet->tileWidth) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR(std::format("Failed to load tilewidth of tile set {}, defaulting to 64", tileSet->name));
                tileSet->tileWidth = 64;
            }
            if (cNode->QueryIntAttribute("tileheight", &tileSet->tileHeight) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR(std::format("Failed to load tileheight of tile set {}, defaulting to 64", tileSet->name));
                tileSet->tileHeight = 64;
            }
            if (cNode->QueryIntAttribute("tilecount", &tileSet->tileCount) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_WARNING("Tilesets with only a single frame are not supported");
                tileSet->tileCount = 0;
            }
            if (cNode->QueryIntAttribute("columns", &tileSet->columnCount) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_WARNING("Tilesets with only a single frame are not supported");
                tileSet->columnCount = 0;
            }
            if (cNode->QueryIntAttribute("padding", &tileSet->padding) != tinyxml2::XML_SUCCESS)
            {
                // Ignore
                tileSet->padding = 0;
            }

            for (const tinyxml2::XMLElement *tileNode = cNode->FirstChildElement(); tileNode != nullptr;
                 tileNode = tileNode->NextSiblingElement())
            {
                if (tileNode->Name() == std::string("image"))
                {
                    tileSet->tiles.push_back(Tile(
                            sf::Vector2i(tileNode->IntAttribute("width", 0), tileNode->IntAttribute("height", 0))));
                    const char *src;
                    if (tileNode->QueryAttribute("source", &src) != tinyxml2::XML_SUCCESS)
                    {
                        SL_LOG_ERROR(std::format("Failed to load source of tile set for image in tile set {}",
                                                 tileSet->name));
                        src = "";
                    }
                    tileSet->tiles.back().texture = src;
                }
            }
        }

        if (cNode->Name() == std::string("layer"))
        {
            SL_LOG_DEBUG(std::format("Loading layer {}", cNode->Attribute("name")));
            const tinyxml2::XMLElement *data = cNode->FirstChildElement();
            if (data == nullptr)
            {
                SL_LOG_FATAL(std::format("Failed to load data of layer {}", cNode->Attribute("name")));
                return false;
            }
            if (data->Attribute("encoding") != std::string("csv"))
            {
                const char *encoding = nullptr;
                if (data->QueryAttribute("encoding", &encoding) != tinyxml2::XML_SUCCESS)
                {
                    SL_LOG_FATAL(std::format("Failed to load encoding of layer {}", cNode->Attribute("name")));
                }
                if (encoding)
                {
                    SL_LOG_FATAL(
                            std::format("Unsupported encoding <{}> of layer {}", encoding, cNode->Attribute("name")));
                }
                return false;
            }

            if (!parseLayer(data->GetText(), {tileSet->tileWidth, tileSet->tileHeight}))
            {
                SL_LOG_FATAL(std::format("Failed to parse layer {}", cNode->Attribute("name")));
                return false;
            }
        }
    }

    if (!tileSet)
    {
        SL_LOG_FATAL("Failed to load any tile sets");
        return false;
    }
    SL_LOG_INFO(std::format("Loaded {}", filePath));

    int i = 0;
    std::string folder = getFileFolder(filePath);
    for (auto iter = tileSet->tiles.begin(); iter != tileSet->tiles.end(); ++i, ++iter)
    {
        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(folder + OS_SEP + iter->texture))
        {
            SL_LOG_FATAL(std::format("Failed to load texture {}", folder + OS_SEP + iter->texture));
            return false;
        }

        m_textures[i + tileSet->firstGid] = texture;
    }

    SL_LOG_DEBUG("Creating world");
    createWorld(*tileSet);

    SL_LOG_INFO("Created arena");

    return true;
}

void Arena::createWorld(const TileSet &set)
{
    m_viewportSize.x = static_cast<float>(m_size.x * m_tileSize.x);
    m_viewportSize.y = static_cast<float>(m_size.y * m_tileSize.y);

    int j = 0;
    for (int i = 0; i < m_size.x * m_size.y; ++i)
    {
        if (m_map[i] != 0)
            j++;
    }
    m_objects.reserve(j);

    const int id = set.firstGid;
    if (!m_textures.contains(id))
    {
        SL_LOG_FATAL("Loaded tile set contains a valid image");
        return;
    }

    sf::Vector2i frameCount(set.columnCount, std::ceil(set.tileCount / set.columnCount));
    SL_LOGF_DEBUG("Frame count for {} is {}x{}", set.name, frameCount.x, frameCount.y);
    for (int r = m_size.y - 1; r > 0; --r)
    {
        for (int c = 0; c < m_size.x; ++c)
        {
            uint32_t value = m_map[r * m_size.x + c];
            if (value == 0)
                continue;

            const bool flippedHorizontally = (value & FLIPPED_HORIZONTALLY_FLAG) != 0;
            const bool flippedVertically = (value & FLIPPED_VERTICALLY_FLAG) != 0;
            const bool flippedDiagonally = (value & FLIPPED_DIAGONALLY_FLAG) != 0;

            // Clear the flags
            value &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG |
                       ROTATED_HEXAGONAL_120_FLAG);

            // SL_LOG_DEBUG(std::format("Creating item at location x: {}, y: {} with frame id: {}", c * m_tileSize.x,
            //                          r * m_tileSize.y, value));
            m_objects.emplace_back(
                    m_textures[id],
                    sf::Vector2f(static_cast<float>(c * m_tileSize.x), static_cast<float>(r * m_tileSize.y)),
                    sf::Vector2f(m_tileSize), frameCount, sf::Vector2i(set.padding, set.padding), value - 1);
            m_objects.back().setFlippedHorizontally(flippedHorizontally);
            m_objects.back().setFlippedVertically(flippedVertically);
            m_objects.back().setFlippedDiagonally(flippedDiagonally);
        }
    }

    m_objects.shrink_to_fit();

    SL_LOGF_DEBUG("Loaded {} objects", m_objects.size());
    SL_LOG_DEBUG("Finding starting Y position");
    for (int i = 0; i < m_size.x * m_size.y; i += m_size.x)
    {
        if (m_map[i] != 0)
        {
            SL_LOG_DEBUG(std::format("Found first item at y position {}", i / m_size.x));
            m_position.y = static_cast<float>(static_cast<double>(i) / static_cast<double>(m_size.x) * m_tileSize.x -
                                              m_tileSize.x * 10);
            SL_LOG_DEBUG(std::format("Starting at y location {}", m_position.y));
            break;
        }
    }
}

bool Arena::parseLayer(const std::string &layer, sf::Vector2i tileSize)
{
    delete[] m_map;
    m_map = new size_t[m_size.x * m_size.y];
    for (int i = 0; i < m_size.x * m_size.y; ++i)
    {
        m_map[i] = 0;
    }

    int pos = 0;
    std::string cSquare;
    for (const char i: layer)
    {
        if (pos >= m_size.x * m_size.y)
        {
            SL_LOG_FATAL("Layer is too large");
            return false;
        }

        if (i == ',')
        {
            try
            {
                m_map[pos] = std::stoll(cSquare);
            }
            catch (const std::out_of_range &)
            {
                SL_LOG_FATAL("Failed to parse layer due to out of range number");
                return false;
            }
            cSquare.clear();
            ++pos;

            continue;
        }
        if (i == '\n')
        {
            continue;
        }

        cSquare += i;
    }

    if (pos != m_size.x * m_size.y - 1)
    {
        SL_LOG_WARNING("Layer is smaller than expected");
        SL_LOG_DEBUG(std::format("Expected: {}, Got: {}", m_size.x * m_size.y, pos));
    }

    return true;
}

ArenaItem *Arena::collidePlayer(const sf::FloatRect &shape)
{
    /* There is a potential edge case here not handled where the
     player collides with 2 tiles in the same frame, in that case it should
     just collide randomly and should not make a difference to the gameplay */
    for (auto &arenaItem: m_objects)
    {
        // Only collide items in the viewport
        if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x + static_cast<float>(m_tileSize.x * 2) or
            arenaItem.getPosition().x < m_position.x - m_viewportSize.x - static_cast<float>(m_tileSize.x * 2))
            continue;
        if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y + static_cast<float>(m_tileSize.y * 2) or
            arenaItem.getPosition().y < m_position.y - m_viewportSize.y - static_cast<float>(m_tileSize.y * 2))
            continue;

        // Shift the rect by m_position
        // rect.top = (arenaItem.getPosition() + m_position).y;
        // rect.left = (arenaItem.getPosition() + m_position).x;
        // rect.width = arenaItem.getSize().x;
        // rect.height = arenaItem.getSize().y;
        //
        // if (rect.intersects(shape))
        // {
        //     return &arenaItem;
        // };
        if (arenaItem.collides(shape))
        {
            return &arenaItem;
        }
    }

    return nullptr;
}

void Arena::update()
{
    m_position += m_scrollSpeed * GeometryDash::getInstance().getDeltaTime().asSeconds();

    for (auto &arenaItem: m_objects)
    {
        arenaItem.setRelativePosition(m_position);
        // Only update items that are in the viewport
        if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x + static_cast<float>(m_tileSize.x * 2) or
            arenaItem.getPosition().x < m_position.x - m_viewportSize.x - static_cast<float>(m_tileSize.x * 2))
            continue;
        if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y + static_cast<float>(m_tileSize.y * 2) or
            arenaItem.getPosition().y < m_position.y - m_viewportSize.y - static_cast<float>(m_tileSize.y * 2))
            continue;

        arenaItem.update();
    }
}

void Arena::render()
{
    for (auto &arenaItem: m_objects)
    {
        // Only render items that are in the viewport
        if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x + static_cast<float>(m_tileSize.x * 2) or
            arenaItem.getPosition().x < m_position.x - m_viewportSize.x - static_cast<float>(m_tileSize.x * 2))
            continue;
        if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y + static_cast<float>(m_tileSize.y * 2) or
            arenaItem.getPosition().y < m_position.y - m_viewportSize.y - static_cast<float>(m_tileSize.y * 2))
            continue;

        arenaItem.setRelativePosition(m_position);
        arenaItem.render();
    }
}

ArenaItem *Arena::getObject(uint64_t id)
{
    const auto found =
            std::ranges::find_if(m_objects, [id](const ArenaItem &arenaItem) { return arenaItem.getId() == id; });
    if (found == m_objects.end())
    {
        return nullptr;
    }

    return &(*found);
}
