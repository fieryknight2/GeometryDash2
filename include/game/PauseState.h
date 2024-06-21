/*
 * PauseState.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include "SFML/Graphics/Text.hpp"
#include "State.h"

class PauseState final : public State
{
public:
    std::string getName() override { return "Pause"; }

    void create() override;
    void update() override {}
    void render() override;
    void destroy() override {}

private:
    sf::Font m_defaultFont;
    sf::Text m_pauseText;
};
