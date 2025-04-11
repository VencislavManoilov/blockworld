#include "World.hpp"
#include <cmath>
#include <random>

// Simple 2D Perlin noise implementation
class PerlinNoise {
private:
    std::vector<int> p;
    
    static double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    static double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }
    
    static double grad(int hash, double x, double y) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    
public:
    PerlinNoise() {
        p.resize(512);
        for(int i = 0; i < 256; ++i) p[i] = i;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(p.begin(), p.begin() + 256, gen);
        
        for(int i = 0; i < 256; ++i) p[256 + i] = p[i];
    }
    
    double noise(double x, double y) {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        
        x -= std::floor(x);
        y -= std::floor(y);
        
        double u = fade(x);
        double v = fade(y);
        
        int A  = p[X] + Y;
        int AA = p[A];
        int AB = p[A + 1];
        int B  = p[X + 1] + Y;
        int BA = p[B];
        int BB = p[B + 1];
        
        return lerp(v, lerp(u, grad(p[AA], x, y),
                              grad(p[BA], x - 1, y)),
                      lerp(u, grad(p[AB], x, y - 1),
                              grad(p[BB], x - 1, y - 1)));
    }
};

static PerlinNoise perlin;

World::World() {
    perlin = PerlinNoise();
}

void World::update(float deltaTime) {
    // Update active chunks if needed
}

void World::render(sf::RenderTarget& target, const sf::Vector2f& cameraPosition) {
    // Calculate visible chunk range based on camera position
    int startChunkX = static_cast<int>(std::floor((cameraPosition.x - target.getView().getSize().x/2) / (Chunk::SIZE * Block::SIZE))) - 1;
    int endChunkX = static_cast<int>(std::ceil((cameraPosition.x + target.getView().getSize().x/2) / (Chunk::SIZE * Block::SIZE))) + 1;
    int startChunkY = static_cast<int>(std::floor((cameraPosition.y - target.getView().getSize().y/2) / (Chunk::SIZE * Block::SIZE))) - 1;
    int endChunkY = static_cast<int>(std::ceil((cameraPosition.y + target.getView().getSize().y/2) / (Chunk::SIZE * Block::SIZE))) + 1;
    
    // Generate and render visible chunks
    for (int cx = startChunkX; cx <= endChunkX; ++cx) {
        for (int cy = startChunkY; cy <= endChunkY; ++cy) {
            if (!chunks[cx][cy].isGenerated) {
                generateChunk(cx, cy);
            }
            
            // Render chunk
            for (int x = 0; x < Chunk::SIZE; ++x) {
                for (int y = 0; y < Chunk::SIZE; ++y) {
                    chunks[cx][cy].blocks[x][y].render(target, 
                        cx * Chunk::SIZE + x, 
                        cy * Chunk::SIZE + y);
                }
            }
        }
    }
}

Block* World::getBlock(int x, int y) {
    int chunkX = std::floor(static_cast<float>(x) / Chunk::SIZE);
    int chunkY = std::floor(static_cast<float>(y) / Chunk::SIZE);
    int localX = x - chunkX * Chunk::SIZE;
    int localY = y - chunkY * Chunk::SIZE;
    
    if (localX < 0) localX += Chunk::SIZE;
    if (localY < 0) localY += Chunk::SIZE;
    
    if (chunks.count(chunkX) > 0 && chunks[chunkX].count(chunkY) > 0) {
        return &chunks[chunkX][chunkY].blocks[localX][localY];
    }
    return nullptr;
}

void World::setBlock(int x, int y, BlockType type) {
    int chunkX = std::floor(static_cast<float>(x) / Chunk::SIZE);
    int chunkY = std::floor(static_cast<float>(y) / Chunk::SIZE);
    int localX = x - chunkX * Chunk::SIZE;
    int localY = y - chunkY * Chunk::SIZE;
    
    if (localX < 0) localX += Chunk::SIZE;
    if (localY < 0) localY += Chunk::SIZE;
    
    if (chunks.count(chunkX) > 0 && chunks[chunkX].count(chunkY) > 0) {
        chunks[chunkX][chunkY].blocks[localX][localY] = Block(type);
    }
}

bool World::isPositionSolid(float x, float y) const {
    int blockX = static_cast<int>(std::floor(x / Block::SIZE));
    int blockY = static_cast<int>(std::floor(y / Block::SIZE));
    
    int chunkX = std::floor(static_cast<float>(blockX) / Chunk::SIZE);
    int chunkY = std::floor(static_cast<float>(blockY) / Chunk::SIZE);
    
    if (chunks.count(chunkX) > 0 && chunks.at(chunkX).count(chunkY) > 0) {
        int localX = blockX - chunkX * Chunk::SIZE;
        int localY = blockY - chunkY * Chunk::SIZE;
        if (localX < 0) localX += Chunk::SIZE;
        if (localY < 0) localY += Chunk::SIZE;
        return chunks.at(chunkX).at(chunkY).blocks[localX][localY].isSolid();
    }
    return false;
}

void World::generateChunk(int chunkX, int chunkY) {
    chunks[chunkX][chunkY].blocks.resize(Chunk::SIZE, std::vector<Block>(Chunk::SIZE));
    generateTerrain(chunks[chunkX][chunkY], chunkX, chunkY);
    generateStructures(chunks[chunkX][chunkY], chunkX, chunkY);
    chunks[chunkX][chunkY].isGenerated = true;
}

void World::generateTerrain(Chunk& chunk, int chunkX, int chunkY) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> diamond_dist(0.0, 1.0);

    for (int x = 0; x < Chunk::SIZE; ++x) {
        // Generate height using sine wave and noise
        double worldX = (chunkX * Chunk::SIZE + x) * TERRAIN_SCALE;
        double baseHeight = 10.0; // Lower base height since we're starting from top
        double heightVariation = 5.0; // Reduced height variation
        
        // Combine sine wave with noise for natural-looking terrain
        double heightNoise = perlin.noise(worldX * 0.5, 0) * heightVariation;
        double sineComponent = sin(worldX * 0.2) * heightVariation * 0.5;
        int surfaceHeight = static_cast<int>(baseHeight + heightNoise + sineComponent);

        for (int y = 0; y < Chunk::SIZE; ++y) {
            int worldY = chunkY * Chunk::SIZE + y;
            
            // Default to air
            chunk.blocks[x][y] = Block(BlockType::Air);
            
            if (worldY < surfaceHeight) {
                // Surface layer - grass
                if (worldY == surfaceHeight - 1) {
                    chunk.blocks[x][y] = Block(BlockType::Grass);
                }
                // Dirt layer (3 blocks)
                else if (worldY >= surfaceHeight - 4) {
                    chunk.blocks[x][y] = Block(BlockType::Dirt);
                }
                // Deep underground
                else {
                    if (worldY < surfaceHeight - 20 && diamond_dist(gen) > 0.99) {
                        chunk.blocks[x][y] = Block(BlockType::Diamond);
                    } else {
                        chunk.blocks[x][y] = Block(BlockType::Stone);
                    }
                }
            }
        }
    }
}

void World::generateStructures(Chunk& chunk, int chunkX, int chunkY) {
    // Empty - no structures in this simplified version
}