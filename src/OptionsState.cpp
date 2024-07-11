/* Created by Matthew Brown on 7/10/2024 */

#include "OptionsState.h"

#include "AssetManager.h"
#include "GeometryDash.h"

OptionsState::OptionsState()
{
    const auto size = GeometryDash::getInstance().getWindow().getWindow().getSize();
    const auto center = sf::Vector2f(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);

    constexpr int buttonWidth = 200;
    constexpr int buttonHeight = 50;

    AssetManager::getInstance().loadFont("assets/fonts/mangabey-regular.otf", "mangabey");
    m_defaultFont = AssetManager::getInstance().getFont("mangabey");

    static ButtonStyle style{sf::Color::White,
                             sf::Color::Black,
                             sf::Color::Black,
                             sf::Color(170, 170, 170),
                             sf::Color(70, 70, 70),
                             2.0f,
                             0,
                             m_defaultFont,
                             10.0f};

    m_enableVSync.setText("Enable VSync");
    m_enableVSync.setStyle(style);
    m_enableVSync.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, -buttonHeight / 2.0f - buttonHeight * 1.25));
    m_enableVSync.setChecked(GeometryDash::EnableVSync);

    m_enableCollisionShapes.setText("Enable Collision Shapes");
    m_enableCollisionShapes.setStyle(style);
    m_enableCollisionShapes.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, 0));
    m_enableCollisionShapes.setChecked(GeometryDash::RenderCollisionShapes);

    m_showDebugInfo.setText("Show Debug Info");
    m_showDebugInfo.setStyle(style);
    m_showDebugInfo.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, buttonHeight / 2.0f + buttonHeight * 1.25));
    m_showDebugInfo.setChecked(GeometryDash::EnableDebug);

    m_backButton.setText("Back");
    m_backButton.setStyle(style);
    m_backButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_backButton.setPosition(center - sf::Vector2f(buttonWidth / 2.0f, buttonHeight / 2.0f + buttonHeight * 2.5));
}

void OptionsState::update()
{
    m_enableVSync.update();
    m_enableCollisionShapes.update();
    m_showDebugInfo.update();
    m_backButton.update();

    if (m_enableVSync.isChecked() != GeometryDash::EnableVSync)
    {
        needsRestart = true;
    }

    GeometryDash::EnableVSync = m_enableVSync.isChecked();
    GeometryDash::RenderCollisionShapes = m_enableCollisionShapes.isChecked();
    GeometryDash::EnableDebug = m_showDebugInfo.isChecked();

    if (m_backButton.wasPressed())
    {
        if (needsRestart)
        {
            GeometryDash::Restart = true;
            m_quit = true;
        }
        else
        {
            GeometryDash::getInstance().changeState(std::make_shared<MainMenuState>());
        }
    }
}

void OptionsState::render()
{
    m_backButton.render();
    m_enableVSync.render();
    m_enableCollisionShapes.render();
    m_showDebugInfo.render();
}
