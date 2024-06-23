/*
 * GameObject.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
    GameObject() : m_id(S_idCounter++) {}
    virtual ~GameObject() = default;

    virtual void update() = 0;
    virtual void render(sf::Vector2f cameraPos) = 0;

    [[nodiscard]] int getZIndex() const { return m_zIndex; }
    void setZIndex(const int zIndex) { m_zIndex = zIndex; }

    [[nodiscard]] uint64_t getId() const { return m_id; }

private:
    static uint64_t S_idCounter;

    uint64_t m_id = -1;
    int m_zIndex = 0;
};
