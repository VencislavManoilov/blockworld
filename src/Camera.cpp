#include "Camera.hpp"

Camera::Camera(sf::RenderWindow& window) : window(window) {
    view = window.getDefaultView();
}

void Camera::update(const sf::Vector2f& target) {
    // Smoothly move camera towards target
    sf::Vector2f targetPos = target;
    position = position + (targetPos - position) * SMOOTHING * 0.016f; // Assuming 60 FPS
    
    view.setCenter(position);
    window.setView(view);
}

sf::Vector2f Camera::getPosition() const {
    return position;
}

void Camera::setPosition(const sf::Vector2f& pos) {
    position = pos;
    view.setCenter(position);
}

sf::View& Camera::getView() {
    return view;
}