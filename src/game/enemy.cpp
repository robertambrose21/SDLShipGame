#include "enemy.h"

Enemy::Enemy(
    std::shared_ptr<GridRenderer> grid,
    const std::string& name, 
    std::shared_ptr<Player> player, 
    Entity::Stats stats
) :
    Entity(grid, name, stats),
    player(player),
    canPassTurn(false)
{ }

// Need to tweak this so the player cannot overlap the enemy
void Enemy::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    if(isNeighbour(player)) {
        if(getCurrentWeapon()->hasFinished()) {
            canPassTurn = true;
        }
        else {
            attack(player, getCurrentWeapon());
        }
    }
    else if(getMovesLeft() <= 0) {
        getCurrentWeapon()->setFinished();
    }
    else {
        findPath(player->getPosition(), 1);
    }
}

bool Enemy::endTurnCondition(void) {
    return canPassTurn;
}

void Enemy::nextTurn() {
    Entity::nextTurn();
    canPassTurn = false;
}