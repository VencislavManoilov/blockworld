#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Block.hpp"

struct InventorySlot {
    BlockType blockType;
    int quantity;
    
    InventorySlot() : blockType(BlockType::Air), quantity(0) {}
};

class Inventory {
public:
    Inventory();
    void render(sf::RenderTarget& target);
    void addItem(BlockType type, int count = 1);
    bool removeItem(BlockType type, int count = 1);
    void selectSlot(int slot);
    BlockType getSelectedType() const;
    void handleInput();

private:
    static constexpr int SLOT_COUNT = 9;
    static constexpr float SLOT_SIZE = 50.0f;
    static constexpr float SLOT_PADDING = 5.0f;
    
    std::vector<InventorySlot> slots;
    int selectedSlot;
    sf::RectangleShape slotShape;
    sf::RectangleShape selectedSlotShape;
};