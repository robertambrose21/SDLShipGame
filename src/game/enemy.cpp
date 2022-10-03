#include "enemy.h"

Enemy::Enemy(
    std::shared_ptr<GridRenderer> grid,
    const std::string& name, 
    std::shared_ptr<Player> player, 
    Entity::Stats stats
) :
    Entity(grid, name, stats),
    player(player)
{ }

// Need to tweak this so the player cannot overlap the enemy
void Enemy::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    if(isNeighbour(player)) {
        attack(player, getCurrentWeapon());
    }
    else if(getMovesLeft() <= 0) {
        getCurrentWeapon()->setFinished();
    }
    else {
        findPath(player->getPosition(), 1);
    }
}

bool Enemy::endTurnCondition(void) {
    return false;
}