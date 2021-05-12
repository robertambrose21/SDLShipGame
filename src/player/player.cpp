#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 }),
    timeSinceLastMoved(0)
{ }

void Player::update(const Uint32& timeSinceLastFrame, bool& quit) {
    // TODO: Put movement code elsewhere so it can also be used by enemy/other entites
    if(path.empty()) {
        return;
    }

    timeSinceLastMoved += timeSinceLastFrame;

    if(timeSinceLastMoved > getSpeed()) {
        setPosition(path.front());
        path.pop_front();
        timeSinceLastMoved = 0;
    }
}

void Player::setPath(std::deque<glm::ivec2> path) {
    this->path = path;
}