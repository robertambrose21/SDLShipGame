#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 3, 10 })
{ }

void Player::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}

bool Player::endTurnCondition(void) {
    return false;
}