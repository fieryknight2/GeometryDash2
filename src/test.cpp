#include <SFML/Graphics/RenderWindow.hpp>
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"

#include "gui/Slider.h"

#include <iostream>

#include "AssetManager.h"
#include "GeometryDash.h"


std::ostream &operator<<(std::ostream &os, const sf::FloatRect &rect)
{
    os << "{" << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height << "}";
    return os;
}


int main()
{
    std::cout << "Hello, World!" << std::endl;
    GeometryDash::getInstance().getWindow().create(WindowSettings{sf::Vector2i(1200, 800), "SFML works!"});
    sf::RenderWindow *window = &GeometryDash::getInstance().getWindow().getWindow();
    sf::Vector2f center(600, 400);

    AssetManager::getInstance().loadFont("assets/fonts/mangabey-regular.otf", "mangabey");
    SliderStyle style = defaultSliderStyle;
    style.font = AssetManager::getInstance().getFont("mangabey");

    Slider slider{sf::Vector2f(100, 0), 200, style};
    slider.setValue(100);
    slider.setMinValue(0);
    slider.setMaxValue(200);

    Slider otherSlider{sf::Vector2f(100, 100), 200, style};
    otherSlider.setValue(100);
    otherSlider.setMinValue(0);
    otherSlider.setMaxValue(200);

    sf::CircleShape circle(50.f); // Rotate Around
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(sf::Vector2f(100.f, 0.f) + center);

    sf::RectangleShape rectangle(sf::Vector2f(100.f, 100.f));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(sf::Vector2f(-50.f, 0.f) + center);

    while (window->isOpen())
    {
        sf::Event event{};
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
            otherSlider.handleEvent(event);
            slider.handleEvent(event);
        }

        window->clear(sf::Color::White);
        window->draw(circle);
        window->draw(rectangle);
        slider.render();
        otherSlider.render();
        window->display();
    }
    window->close();

    return 0;
}
