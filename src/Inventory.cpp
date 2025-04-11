#include "Inventory.hpp"

Inventory::Inventory() : selectedSlot(0), slots(SLOT_COUNT) {
    slotShape.setSize(sf::Vector2f(SLOT_SIZE, SLOT_SIZE));
    slotShape.setFillColor(sf::Color(128, 128, 128, 200));
    slotShape.setOutlineColor(sf::Color::White);
    slotShape.setOutlineThickness(1.0f);
    
    selectedSlotShape = slotShape;
    selectedSlotShape.setOutlineColor(sf::Color::Yellow);
    selectedSlotShape.setOutlineThickness(2.0f);
    
    // Initialize with some blocks
    slots[0].blockType = BlockType::Grass;
    slots[0].quantity = 64;
    slots[1].blockType = BlockType::Stone;
    slots[1].quantity = 64;
    slots[2].blockType = BlockType::WoodLog;
    slots[2].quantity = 64;
}

void Inventory::render(sf::RenderTarget& target) {
    sf::Vector2f windowSize = target.getView().getSize();
    float startX = (windowSize.x - (SLOT_COUNT * (SLOT_SIZE + SLOT_PADDING))) / 2;
    float y = windowSize.y - SLOT_SIZE - 10.0f;
    
    for (int i = 0; i < SLOT_COUNT; ++i) {
        float x = startX + i * (SLOT_SIZE + SLOT_PADDING);
        
        // Draw slot background
        if (i == selectedSlot) {
            selectedSlotShape.setPosition(x, y);
            target.draw(selectedSlotShape);
        } else {
            slotShape.setPosition(x, y);
            target.draw(slotShape);
        }
        
        // Draw block preview if slot is not empty
        if (slots[i].quantity > 0) {
            Block block(slots[i].blockType);
            block.render(target, x / Block::SIZE, y / Block::SIZE);
        }
    }
}

void Inventory::handleInput() {
    // Number keys 1-9 for slot selection
    for (int i = 0; i < SLOT_COUNT; ++i) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectSlot(i);
        }
    }
}

void Inventory::addItem(BlockType type, int count) {
    // First try to stack with existing items
    for (auto& slot : slots) {
        if (slot.blockType == type && slot.quantity < 64) {
            int space = 64 - slot.quantity;
            int add = std::min(count, space);
            slot.quantity += add;
            count -= add;
            
            if (count == 0) return;
        }
    }
    
    // Then try to find empty slots
    if (count > 0) {
        for (auto& slot : slots) {
            if (slot.quantity == 0) {
                slot.blockType = type;
                slot.quantity = std::min(count, 64);
                count -= slot.quantity;
                
                if (count == 0) return;
            }
        }
    }
}

bool Inventory::removeItem(BlockType type, int count) {
    int available = 0;
    
    // First check if we have enough items
    for (const auto& slot : slots) {
        if (slot.blockType == type) {
            available += slot.quantity;
        }
    }
    
    if (available < count) {
        return false;
    }
    
    // Remove items
    for (auto& slot : slots) {
        if (slot.blockType == type) {
            int remove = std::min(count, slot.quantity);
            slot.quantity -= remove;
            count -= remove;
            
            if (slot.quantity == 0) {
                slot.blockType = BlockType::Air;
            }
            
            if (count == 0) {
                return true;
            }
        }
    }
    
    return true;
}

void Inventory::selectSlot(int slot) {
    if (slot >= 0 && slot < SLOT_COUNT) {
        selectedSlot = slot;
    }
}

BlockType Inventory::getSelectedType() const {
    return slots[selectedSlot].blockType;
}