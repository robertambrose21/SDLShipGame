#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 }),
    destination({ 0, 0}),
    direction({ 0.0f, 0.0f })
{ }

void Player::update(const Uint32& timeSinceLastFrame, bool& quit) {
    if(getPosition() == destination) {
        return;
    }

    setPartialPosition(getPartialPosition() + (direction * (timeSinceLastFrame / getSpeed())));
}

void Player::setDestination(const glm::ivec2& destination) {
    this->destination = destination;
    direction = glm::normalize(glm::vec2(destination) - getPartialPosition());
}