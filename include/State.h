/*
 * State.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <string>
#include "SFML/Window/Event.hpp"

class State
{
public:
    State() = default;
    virtual ~State() = default;

    virtual std::string getName() = 0;

    virtual void handleEvent(const sf::Event &event) {}
    virtual void update() = 0;
    virtual void render() = 0;

    virtual bool handleCloseEvent() { return false; }
    [[nodiscard]] bool quit() const { return m_quit; }

protected:
    bool m_quit = false;
};
