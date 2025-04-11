#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Block.hpp"

struct Chunk {
    static constexpr int SIZE = 16;
    std::vector<std::vector<Block>> blocks;
    bool isGenerated;
    
    Chunk() : blocks(SIZE, std::vector<Block>(SIZE)), isGenerated(false) {}
};

class World {
public:
    World();
    void update(float deltaTime);
    void render(sf::RenderTarget& target, const sf::Vector2f& cameraPosition);
    Block* getBlock(int x, int y);
    void setBlock(int x, int y, BlockType type);
    bool isPositionSolid(float x, float y) const;

private:
    std::unordered_map<int, std::unordered_map<int, Chunk>> chunks;
    void generateChunk(int chunkX, int chunkY);
    void generateTerrain(Chunk& chunk, int chunkX, int chunkY);
    void generateStructures(Chunk& chunk, int chunkX, int chunkY);
    float generateNoise(float x, float y) const;
    
    static constexpr int RENDER_DISTANCE = 2;
    static constexpr int WATER_LEVEL = 60;
    static constexpr int STONE_LEVEL = 40;
    static constexpr float TERRAIN_SCALE = 0.05f;
};