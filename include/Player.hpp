#pragma once
#include <SFML/Graphics.hpp>
#include "World.hpp"

class Player {
public:
    Player(World& world);
    void update(float deltaTime);
    void render(sf::RenderTarget& target);
    void handleInput();
    const sf::Vector2f& getPosition() const;
    void setPosition(const sf::Vector2f& pos);

private:
    World& world;
    sf::RectangleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    bool isOnGround;

    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = -400.0f;
    static constexpr float GRAVITY = 800.0f;
    static constexpr float MAX_FALL_SPEED = 500.0f;
    
    void applyPhysics(float deltaTime);
    void checkCollisions();
};