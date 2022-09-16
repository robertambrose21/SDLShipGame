#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 })
{ }

void Player::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}

bool Player::endTurnCondition(void) {
    return false;
}

void Player::shoot(std::shared_ptr<Entity> entity) {
    entity->doDamage(1);
}