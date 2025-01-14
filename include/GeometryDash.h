/*
 * GeometryDash.h
 * @author Matthew Brown
 * @date 6/19/2024
 */
#pragma once

#include <memory>

#include "MainMenuState.h"
#include "Window.h"

class GeometryDash
{
public:
    static GeometryDash &getInstance();
    ~GeometryDash() = default;

    static void LoadSettings();
    static void SaveSettings();

    bool run() noexcept(false);

    std::shared_ptr<State> getState() { return m_state; }
    void changeState(const std::shared_ptr<State> &state) noexcept(false);

    Window &getWindow() { return m_window; }
    sf::Clock &getClock() { return m_clock; }

    sf::Time getDeltaTime() const { return m_deltaTime; }

    static bool RenderCollisionShapes;
    static bool EnableDebug;
    static bool EnableVSync;
    static bool Restart;

    static void Reset();

private:
    // Singleton
    static GeometryDash S_instance;

    GeometryDash() = default;

    std::shared_ptr<State> m_state;
    Window m_window;
    sf::Clock m_clock;
    sf::Time m_deltaTime;
};
