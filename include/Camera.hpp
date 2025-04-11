#pragma once
#include <SFML/Graphics.hpp>

class Camera {
public:
    Camera(sf::RenderWindow& window);
    void update(const sf::Vector2f& target);
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
    sf::View& getView();

private:
    sf::RenderWindow& window;
    sf::View view;
    sf::Vector2f position;

    static constexpr float SMOOTHING = 5.0f;
};