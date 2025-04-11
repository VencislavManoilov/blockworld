#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <map>

enum class BlockType {
    Air,
    Grass,
    Dirt,
    Stone,
    Diamond,
    Water,
    WoodLog,
    Leaves
};

class Block {
public:
    Block(BlockType type = BlockType::Air);
    void render(sf::RenderTarget& target, float x, float y);
    bool isSolid() const;
    bool isLiquid() const;
    BlockType getType() const;

    static constexpr float SIZE = 32.0f; // Size of each block in pixels
    static void loadTextures();
    static void loadTexture(BlockType type, const std::string& filepath);

private:
    BlockType type;
    sf::RectangleShape shape;

    void initializeVisuals();
    sf::Color getColorForType() const;

    static std::map<BlockType, sf::Texture> textures;
};