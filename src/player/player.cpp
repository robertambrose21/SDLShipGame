#include "player.h"

Player::Player(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 })
{ }

void Player::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}
