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
    std::uniform_real_distribution<> tree_dist(0.0, 1.0);
    std::vector<std::pair<int, int>> treePlacements;

    for (int x = 0; x < Chunk::SIZE; ++x) {
        // Generate height using multiple noise functions for more interesting terrain
        double worldX = (chunkX * Chunk::SIZE + x) * TERRAIN_SCALE;
        double baseHeight = Chunk::SIZE - 10.0;
        
        // Add multiple noise components at different scales for more natural terrain
        double noiseScale1 = 0.5; // Large-scale terrain features
        double noiseScale2 = 2.0; // Medium details
        double noiseScale3 = 5.0; // Small details
        
        double heightNoise = perlin.noise(worldX * noiseScale1, 0) * 6.0 +
                             perlin.noise(worldX * noiseScale2, 0) * 3.0 +
                             perlin.noise(worldX * noiseScale3, 0) * 1.0;
                             
        // Add hills with sine waves of different periods
        double sineComponent = sin(worldX * 0.2) * 3.0 +
                               sin(worldX * 0.05) * 5.0;
                               
        int surfaceHeight = static_cast<int>(baseHeight + heightNoise + sineComponent);

        // Track terrain surface for tree placement
        int terrainSurfaceY = surfaceHeight + 1;
        
        for (int y = 0; y < Chunk::SIZE; ++y) {
            int worldY = chunkY * Chunk::SIZE + y;
            
            // Default to air
            chunk.blocks[x][y] = Block(BlockType::Air);
            
            if (worldY > surfaceHeight) {
                // Surface layer - grass
                if (worldY == surfaceHeight + 1) {
                    chunk.blocks[x][y] = Block(BlockType::Grass);
                    
                    // Consider placing a tree on this grass block
                    if (tree_dist(gen) < 0.05) {  // 5% chance for a tree
                        treePlacements.push_back(std::make_pair(x, y));
                    }
                }
                // Dirt layer (3-5 blocks with variable depth)
                else if (worldY <= surfaceHeight + 3 + static_cast<int>(perlin.noise(worldX * 5.0, worldY * 5.0) * 2.0)) {
                    chunk.blocks[x][y] = Block(BlockType::Dirt);
                }
                // Deep underground
                else {
                    // Add cave systems (air pockets)
                    double caveNoise = perlin.noise(worldX * 0.1, worldY * 0.1);
                    double caveDensity = 0.3; // Higher = fewer caves
                    
                    if (caveNoise > caveDensity) {
                        if (worldY > surfaceHeight + 20 && diamond_dist(gen) > 0.98) {
                            chunk.blocks[x][y] = Block(BlockType::Diamond);
                        } else {
                            chunk.blocks[x][y] = Block(BlockType::Stone);
                        }
                    }
                    // Otherwise leave as air (cave)
                }
            }
        }
    }
    
    // Generate trees after terrain is complete
    for (const auto& treePos : treePlacements) {
        generateTree(chunk, treePos.first, treePos.second);
    }
}

void World::generateTree(Chunk& chunk, int x, int y) {
    // Define tree characteristics
    const int trunkHeight = 4 + rand() % 3; // 4-6 blocks tall
    const int leavesRadius = 2;
    
    // Generate trunk
    for (int h = 1; h <= trunkHeight; h++) {
        if (y - h >= 0) {
            chunk.blocks[x][y - h] = Block(BlockType::WoodLog);
        }
    }
    
    // Generate leaves (in a circular pattern)
    for (int ly = -leavesRadius; ly <= leavesRadius; ly++) {
        for (int lx = -leavesRadius; lx <= leavesRadius; lx++) {
            // Create rounded leaf shape
            if (lx*lx + ly*ly <= leavesRadius*leavesRadius + 1) {
                int leafX = x + lx;
                int leafY = y - trunkHeight - 1 + ly;
                
                // Check bounds of the chunk
                if (leafX >= 0 && leafX < Chunk::SIZE && leafY >= 0 && leafY < Chunk::SIZE) {
                    // Only place leaves where there's air
                    if (chunk.blocks[leafX][leafY].getType() == BlockType::Air) {
                        chunk.blocks[leafX][leafY] = Block(BlockType::Leaves);
                    }
                }
            }
        }
    }
    
    // Generate some extra leaves on top
    if (y - trunkHeight - 2 >= 0 && x < Chunk::SIZE) {
        chunk.blocks[x][y - trunkHeight - 2] = Block(BlockType::Leaves);
    }
}

void World::generateStructures(Chunk& chunk, int chunkX, int chunkY) {
    // Empty - no structures in this simplified version
}