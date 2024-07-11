/*
 * OptionsState.h
 * @author Matthew Brown
 * @date 7/10/2024
 */
#pragma once

#include "State.h"
#include "gui/Button.h"

class OptionsState final : public State
{
public:
    OptionsState();

    std::string getName() override { return "Options"; }
    void handleEvent(const sf::Event &event) override {}
    void update() override;
    void render() override;

private:
    CheckButton m_enableVSync;
    CheckButton m_enableCollisionShapes;
    CheckButton m_showDebugInfo;
    Button m_backButton;

    sf::Font m_defaultFont;

    bool needsRestart = false;
};
