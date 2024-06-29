/* Created by Matthew Brown on 6/19/2024 */
#include "game/Arena.h"

#include <algorithm>
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
    SL_LOG_DEBUG(std::format("Loading file: {}", filePath));
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
        SL_LOG_DEBUG("Loading map");

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

    std::vector<TileSet> tileSets;
    for (const tinyxml2::XMLElement *cNode = rootNode->FirstChildElement(); cNode != nullptr;
         cNode = cNode->NextSiblingElement())
    {
        SL_LOG_DEBUG(std::format("Processing node {}", cNode->Name()));
        if (cNode->Name() == std::string("tileset"))
        {
            TileSet tileSet;

            SL_LOG_DEBUG(std::format("Loading tile set {}", cNode->Attribute("name")));

            const char *nm;
            if (cNode->QueryStringAttribute("name", &nm) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR("Failed to load name of tile set, defaulting to empty string");
                nm = "";
            }
            tileSet.name = nm;

            if (cNode->QueryIntAttribute("firstgid", &tileSet.firstGid) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_FATAL(std::format("Failed to load firstgid of tile set {}", tileSet.name));
                return false;
            }
            if (cNode->QueryIntAttribute("tilewidth", &tileSet.tileWidth) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR(std::format("Failed to load tilewidth of tile set {}, defaulting to 64", tileSet.name));
                tileSet.tileWidth = 64;
            }
            if (cNode->QueryIntAttribute("tileheight", &tileSet.tileHeight) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_ERROR(std::format("Failed to load tileheight of tile set {}, defaulting to 64", tileSet.name));
                tileSet.tileHeight = 64;
            }
            if (cNode->QueryIntAttribute("tilecount", &tileSet.tileCount) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_WARNING("Tilesets with only a single frame are not supported");
                tileSet.tileCount = 0;
            }
            if (cNode->QueryIntAttribute("columns", &tileSet.columnCount) != tinyxml2::XML_SUCCESS)
            {
                SL_LOG_WARNING("Tilesets with only a single frame are not supported");
                tileSet.columnCount = 0;
            }
            if (cNode->QueryIntAttribute("padding", &tileSet.padding) != tinyxml2::XML_SUCCESS)
            {
                // Ignore
                tileSet.padding = 0;
            }

            for (const tinyxml2::XMLElement *tileNode = cNode->FirstChildElement(); tileNode != nullptr;
                 tileNode = tileNode->NextSiblingElement())
            {
                if (tileNode->Name() == std::string("image"))
                {
                    tileSet.tiles.push_back(Tile(
                            sf::Vector2i(tileNode->IntAttribute("width", 0), tileNode->IntAttribute("height", 0))));
                    const char *src;
                    if (tileNode->QueryAttribute("source", &src) != tinyxml2::XML_SUCCESS)
                    {
                        SL_LOG_ERROR(std::format("Failed to load source of tile set for image in tile set {}",
                                                 tileSet.name));
                        src = "";
                    }
                    tileSet.tiles.back().texture = src;
                }
            }

            tileSets.push_back(std::move(tileSet));
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

            sf::Vector2i tileSize{0, 0};
            for (const auto &ts: tileSets)
            {
                if (ts.tileWidth > tileSize.x)
                {
                    tileSize.x = ts.tileWidth;
                }
                if (ts.tileHeight > tileSize.y)
                {
                    tileSize.y = ts.tileHeight;
                }
            }
            if (!parseLayer(data->GetText(), tileSize))
            {
                SL_LOG_FATAL(std::format("Failed to parse layer {}", cNode->Attribute("name")));
                return false;
            }
        }
    }

    if (tileSets.empty())
    {
        SL_LOG_FATAL("Failed to load any tile sets");
        return false;
    }
    SL_LOG_DEBUG(std::format("Loaded {}", filePath));

    std::string folder = getFileFolder(filePath);
    for (const auto &tileSet: tileSets)
    {
        int i = 0;
        for (auto iter = tileSet.tiles.begin(); iter != tileSet.tiles.end(); ++i, ++iter)
        {
            auto texture = std::make_shared<sf::Texture>();
            if (!texture->loadFromFile(folder + OS_SEP + iter->texture))
            {
                SL_LOG_FATAL(std::format("Failed to load texture {}", folder + OS_SEP + iter->texture));
                return false;
            }

            m_textures[i + tileSet.firstGid] = texture;
        }
    }

    SL_LOG_DEBUG("Creating world");

    createWorld(tileSets);

    SL_LOG_DEBUG("Created arena");

    return true;
}

void Arena::createWorld(const std::vector<TileSet> &set)
{
    m_viewportSize.x = static_cast<float>(m_size.x * m_tileSize.x);
    m_viewportSize.y = static_cast<float>(m_size.y * m_tileSize.y);

    int j = 0;
    for (const auto &pos: m_map)
    {
        if (pos != 0)
            j++;
    }
    m_objects.reserve(j);

    if (set.empty())
    {
        SL_LOG_FATAL("Failed to load any tile sets");
        return;
    }

    std::vector<TileSet> nset = set;
    std::ranges::sort(nset, [](const TileSet &a, const TileSet &b) { return a.firstGid < b.firstGid; });
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
            // if (flippedHorizontally and flippedVertically and flippedDiagonally)
            //     SL_LOGF_DEBUG("flipped horizontally: {}, flipped vertically: {}, flipped diagonally: {}",
            //                   flippedHorizontally, flippedVertically, flippedDiagonally);

            // Clear the flags
            value &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG |
                       ROTATED_HEXAGONAL_120_FLAG);

            const TileSet *ts = nullptr;
            for (auto iter = nset.begin(); iter != nset.end(); ++iter)
            {
                if (iter + 1 == nset.end() or (value >= iter->firstGid and value < (iter + 1)->firstGid))
                {
                    ts = iter.base();
                    break;
                }
            }
            if (ts == nullptr)
            {
                SL_LOGF_FATAL("Failed to find tile set for value {}", value);
                return;
            }

            sf::Vector2i frameCount(ts->columnCount, std::ceil(ts->tileCount / ts->columnCount));
            // SL_LOGF_DEBUG("Frame count for {} is {}x{}", ts->name, frameCount.x, frameCount.y);
            const int id = ts->firstGid;
            if (!m_textures.contains(id))
            {
                SL_LOG_FATAL("Loaded tile set does not contain a valid image");
                return;
            }

            // SL_LOG_DEBUG(std::format("Creating item at location x: {}, y: {} with frame id: {}", c * m_tileSize.x,
            //                          r * m_tileSize.y, value));
            m_objects.emplace_back(
                    m_textures[id],
                    sf::Vector2f(static_cast<float>(c * m_tileSize.x), static_cast<float>(r * m_tileSize.y)),
                    sf::Vector2f(m_tileSize), frameCount, sf::Vector2i(ts->padding, ts->padding), value - ts->firstGid);
            m_objects.back().setFlippedHorizontally(flippedHorizontally);
            m_objects.back().setFlippedVertically(flippedVertically);
            m_objects.back().setFlippedDiagonally(flippedDiagonally);
            // m_objects.back().setOnCollision([]() { SL_LOG_DEBUG("Collision!"); });

            if (ts->name == "Spikes")
            {
                m_objects.back().setType(ArenaItemType::Spike);
            }
            else if (ts->name == "TinySpikes")
            {
                m_objects.back().setType(ArenaItemType::TinySpike);
            }
            else if (ts->name == "Default" or ts->name == "SimpleTileSet")
            {
                m_objects.back().setType(ArenaItemType::Default);
            }
            else
            {
                SL_LOGF_WARNING("Unknown object type: {}", ts->name);
            }
        }
    }

    m_objects.shrink_to_fit();

    SL_LOGF_DEBUG("Loaded {} objects", m_objects.size());

    resetPos();
}

void Arena::resetPos()
{
    m_position.x = 0;
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
    m_map.resize(m_size.x * m_size.y);
    for (auto &pos: m_map)
    {
        pos = 0;
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
#ifndef NDEBUG
    m_collisions = 0;
#endif // NDEBUG
    for (auto &arenaItem: m_objects)
    {
        // Only collide items in the viewport
        if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x / 2 or
            arenaItem.getPosition().x < m_position.x - m_viewportSize.x)
            continue;
        if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y or
            arenaItem.getPosition().y < m_position.y - m_viewportSize.y)
            continue;

#ifndef NDEBUG
        ++m_collisions;
#endif // NDEBUG
        if (arenaItem.collides(shape))
        {
            arenaItem.setRelativePosition(m_position);
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
        // if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x + static_cast<float>(m_tileSize.x * 2) or
        //     arenaItem.getPosition().x < m_position.x - m_viewportSize.x - static_cast<float>(m_tileSize.x * 2))
        //     continue;
        // if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y + static_cast<float>(m_tileSize.y * 2) or
        //     arenaItem.getPosition().y < m_position.y - m_viewportSize.y - static_cast<float>(m_tileSize.y * 2))
        //     continue;

        arenaItem.update();
    }
}

void Arena::render(const sf::Vector2f &cameraPos, sf::Color tint)
{
#ifndef NDEBUG
    m_rendered = 0;
#endif // NDEBUG
    for (auto &arenaItem: m_objects)
    {

        // Only render items that are in the viewport
        if (arenaItem.getPosition().x > m_position.x + m_viewportSize.x + static_cast<float>(m_tileSize.x * 2) or
            arenaItem.getPosition().x < m_position.x - m_viewportSize.x - static_cast<float>(m_tileSize.x * 2))
            continue;
        if (arenaItem.getPosition().y > m_position.y + m_viewportSize.y + static_cast<float>(m_tileSize.y * 2) or
            arenaItem.getPosition().y < m_position.y - m_viewportSize.y - static_cast<float>(m_tileSize.y * 2))
            continue;

#ifndef NDEBUG
        ++m_rendered;
#endif // NDEBUG

        arenaItem.setRelativePosition(m_position);
        arenaItem.render(cameraPos, tint);
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
