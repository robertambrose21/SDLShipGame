#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 }, std::vector<std::shared_ptr<Weapon>>())
{ }

void Player::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}

bool Player::endTurnCondition(void) {
    return false;
}