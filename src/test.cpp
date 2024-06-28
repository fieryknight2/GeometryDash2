#include <SFML/Graphics/RenderWindow.hpp>
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"

#include "game/Collision.h"
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
    slider.setMaxValue(400);

    Slider otherSlider{sf::Vector2f(100, 100), 200, style};
    otherSlider.setValue(100);
    otherSlider.setMinValue(0);
    otherSlider.setMaxValue(400);

    std::vector points = {sf::Vector2f(0.f, 0.f), sf::Vector2f(100.f, 0.f), sf::Vector2f(50.f, -100.f)};
    sf::ConvexShape triangle(3);
    triangle.setPoint(0, points[0]);
    triangle.setPoint(1, points[1]);
    triangle.setPoint(2, points[2]);
    triangle.setFillColor(sf::Color::Blue);
    triangle.setPosition(sf::Vector2f(100.f, 0.f) + center);
    TriangleCollider triangleCollider{points[0], points[1], points[2]};
    triangleCollider.setPosition(triangle.getPosition());

    sf::Vector2f rectInitPos = sf::Vector2f(-200.f, -200.f) + center;
    sf::RectangleShape rectangle(sf::Vector2f(100.f, 100.f));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(rectInitPos);
    RectangleCollider rectangleCollider{rectangle.getPosition(), rectangle.getSize()};

    sf::ConvexShape ctriangle(3);
    ctriangle.setPoint(0, triangleCollider.getLeftPoint());
    ctriangle.setPoint(1, triangleCollider.getRightPoint());
    ctriangle.setPoint(2, triangleCollider.getTopPoint());
    ctriangle.setFillColor(sf::Color::Black);

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

        rectangle.setPosition(sf::Vector2f(rectInitPos + sf::Vector2f(slider.getValue(), otherSlider.getValue())));
        rectangleCollider.setPosition(rectangle.getPosition());


        if (triangleCollider.collides(rectangle.getGlobalBounds()))
        {
            rectangle.setFillColor(sf::Color::Red);
        }
        else
        {
            rectangle.setFillColor(sf::Color::Green);
        }

        window->clear(sf::Color::White);
        window->draw(triangle);
        window->draw(rectangle);
        window->draw(ctriangle);
        slider.render();
        otherSlider.render();
        window->display();
    }
    window->close();

    return 0;
}
