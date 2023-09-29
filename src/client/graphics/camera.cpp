#include "camera.h"

Camera::Camera(const glm::ivec2& position)
    : position(position)
{ }

void Camera::move(const glm::ivec2& amount) {
    position = position + amount;
}

void Camera::setPosition(const glm::ivec2& position) {
    this->position = position;
}

glm::ivec2 Camera::getPosition(void) const {
    return position;
}