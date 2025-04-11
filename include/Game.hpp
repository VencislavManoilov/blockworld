#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "World.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "Inventory.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    std::unique_ptr<World> world;
    std::unique_ptr<Player> player;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Inventory> inventory;
    
    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr char WINDOW_TITLE[] = "2D Minecraft";
};