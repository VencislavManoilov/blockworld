#include "Player.hpp"
#include <cmath>

Player::Player(World& world) : world(world), isOnGround(false) {
    shape.setSize(sf::Vector2f(Block::SIZE * 0.8f, Block::SIZE * 1.8f));
    shape.setFillColor(sf::Color(255, 127, 80)); // Coral color
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y);
}

void Player::update(float deltaTime) {
    applyPhysics(deltaTime);
    checkCollisions();
}

void Player::handleInput() {
    velocity.x = 0.0f;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -MOVE_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = MOVE_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isOnGround) {
        velocity.y = JUMP_FORCE;
        isOnGround = false;
    }
}

void Player::render(sf::RenderTarget& target) {
    shape.setPosition(position);
    target.draw(shape);
}

void Player::applyPhysics(float deltaTime) {
    // Apply gravity
    if (!isOnGround) {
        velocity.y += GRAVITY * deltaTime;
    }
    
    // Limit fall speed
    if (velocity.y > MAX_FALL_SPEED) {
        velocity.y = MAX_FALL_SPEED;
    }
    
    // Update position
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void Player::checkCollisions() {
    // Get the player's bounding box
    float left = position.x - shape.getSize().x / 2;
    float right = position.x + shape.getSize().x / 2;
    float top = position.y - shape.getSize().y;
    float bottom = position.y;
    
    // Check each corner of the player's bounding box
    bool collidingBottom = false;
    
    // Bottom collision
    if (velocity.y >= 0) {
        if (world.isPositionSolid(left + 2.0f, bottom + 1.0f) ||
            world.isPositionSolid(right - 2.0f, bottom + 1.0f)) {
            position.y = std::floor(bottom / Block::SIZE) * Block::SIZE;
            velocity.y = 0;
            collidingBottom = true;
        }
    }
    
    // Top collision
    if (velocity.y < 0) {
        if (world.isPositionSolid(left + 2.0f, top) ||
            world.isPositionSolid(right - 2.0f, top)) {
            position.y = std::ceil(top / Block::SIZE) * Block::SIZE + shape.getSize().y;
            velocity.y = 0;
        }
    }
    
    // Update ground state
    isOnGround = collidingBottom;
    
    // Left/Right collisions
    if (velocity.x < 0) {
        if (world.isPositionSolid(left, top + 2.0f) ||
            world.isPositionSolid(left, bottom - 2.0f)) {
            position.x = std::ceil(left / Block::SIZE) * Block::SIZE + shape.getSize().x / 2;
            velocity.x = 0;
        }
    }
    else if (velocity.x > 0) {
        if (world.isPositionSolid(right, top + 2.0f) ||
            world.isPositionSolid(right, bottom - 2.0f)) {
            position.x = std::floor(right / Block::SIZE) * Block::SIZE - shape.getSize().x / 2;
            velocity.x = 0;
        }
    }
}

const sf::Vector2f& Player::getPosition() const {
    return position;
}

void Player::setPosition(const sf::Vector2f& pos) {
    position = pos;
}