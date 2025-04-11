#include "Block.hpp"

Block::Block(BlockType type) : type(type) {
    initializeVisuals();
}

void Block::initializeVisuals() {
    shape.setSize(sf::Vector2f(SIZE, SIZE));
    shape.setFillColor(getColorForType());
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

sf::Color Block::getColorForType() const {
    switch (type) {
        case BlockType::Grass:
            return sf::Color(34, 139, 34);  // Forest green
        case BlockType::Dirt:
            return sf::Color(139, 69, 19);  // Saddle brown
        case BlockType::Stone:
            return sf::Color(128, 128, 128); // Gray
        case BlockType::Diamond:
            return sf::Color(0, 191, 255);   // Deep sky blue
        case BlockType::Water:
            return sf::Color(0, 0, 255, 128); // Semi-transparent blue
        case BlockType::WoodLog:
            return sf::Color(139, 69, 19);    // Brown
        case BlockType::Leaves:
            return sf::Color(34, 139, 34, 200); // Semi-transparent green
        default:
            return sf::Color::Transparent;
    }
}