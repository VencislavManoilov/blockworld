#include "Game.hpp"
#include "cmath"

Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE) {
    window.setFramerateLimit(60);
    
    world = std::make_unique<World>();
    player = std::make_unique<Player>(*world);
    camera = std::make_unique<Camera>(window);
    inventory = std::make_unique<Inventory>();

    // Find spawn point at the center of the world (x=0)
    int spawnX = 0;
    int spawnY = 0;
    bool foundSpawn = false;
    
    // Search from top until we find the first solid block
    for (int y = 0; y < 128 && !foundSpawn; ++y) {
        Block* block = world->getBlock(spawnX, y);
        if (block && block->isSolid()) {
            spawnX = 0;
            spawnY = y - 1; // One block above the surface
            foundSpawn = true;
            break;
        }
    }

    player->setPosition(sf::Vector2f(spawnX * Block::SIZE + Block::SIZE/2, spawnY * Block::SIZE));
    camera->setPosition(player->getPosition());
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            // Get mouse position in screen coordinates
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
            // Convert to world coordinates using the camera view
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, camera->getView());
            
            // Convert to block coordinates
            int blockX = static_cast<int>(std::floor(worldPos.x / Block::SIZE));
            int blockY = static_cast<int>(std::floor(worldPos.y / Block::SIZE));
            
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Break block
                world->setBlock(blockX, blockY, BlockType::Air);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                // Place block
                BlockType selectedType = inventory->getSelectedType();
                if (selectedType != BlockType::Air) {
                    world->setBlock(blockX, blockY, selectedType);
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    player->handleInput();
    player->update(deltaTime);
    camera->update(player->getPosition());
    inventory->handleInput();
}

void Game::render() {
    window.clear(sf::Color(135, 206, 235)); // Sky blue background
    
    window.setView(camera->getView());
    world->render(window, camera->getPosition());
    player->render(window);
    
    // Reset view for UI
    window.setView(window.getDefaultView());
    inventory->render(window);
    
    window.display();
}