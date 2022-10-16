#include "player.h"

Player::Player() :
    Entity("Player", { 3, 10 })
{ }

void Player::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}

bool Player::endTurnCondition(void) {
    return false;
}