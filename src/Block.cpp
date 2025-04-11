#include "Block.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <stdexcept>

std::map<BlockType, sf::Texture> Block::textures;

Block::Block(BlockType type) : type(type) {
    initializeVisuals();
}

void Block::initializeVisuals() {
    if (textures.empty()) {
        try {
            loadTextures();
        } catch (const std::runtime_error& e) {
            std::cerr << "Error loading textures: " << e.what() << std::endl;
            throw;
        }
    }

    shape.setSize(sf::Vector2f(SIZE, SIZE));

    if (type == BlockType::Air) {
        // Skip texture assignment for Air blocks
        shape.setTexture(nullptr);
        return;
    }

    auto it = textures.find(type);
    if (it != textures.end()) {
        shape.setTexture(&it->second);
    } else {
        std::cerr << "Texture not found for block type: " << static_cast<int>(type) << std::endl;
        throw std::runtime_error("Texture not found for block type");
    }

    shape.setOutlineColor(sf::Color(0, 0, 0, 64));
    shape.setOutlineThickness(1.0f);
}

void Block::render(sf::RenderTarget& target, float x, float y) {
    if (type != BlockType::Air) {
        shape.setPosition(x * SIZE, y * SIZE);
        target.draw(shape);
    }
}

bool Block::isSolid() const {
    return type != BlockType::Air && type != BlockType::Water;
}

bool Block::isLiquid() const {
    return type == BlockType::Water;
}

BlockType Block::getType() const {
    return type;
}

void Block::loadTextures() {
    loadTexture(BlockType::Grass, "assets/grass.png");
    loadTexture(BlockType::Dirt, "assets/dirt.png");
    loadTexture(BlockType::Stone, "assets/stone.png");
    loadTexture(BlockType::Diamond, "assets/diamond.png");
    loadTexture(BlockType::Water, "assets/water.png");
    loadTexture(BlockType::WoodLog, "assets/wood_log.png");
    loadTexture(BlockType::Leaves, "assets/leaves.png");
}

void Block::loadTexture(BlockType type, const std::string& filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        throw std::runtime_error("Failed to load texture: " + filepath);
    }
    textures[type] = std::move(texture);
    std::cout << "Loaded texture: " << filepath << " for block type: " << static_cast<int>(type) << std::endl;
}