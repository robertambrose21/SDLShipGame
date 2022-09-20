#include "enemy.h"

Enemy::Enemy(
    std::shared_ptr<GridRenderer> grid,
    const std::string& name, 
    std::shared_ptr<Player> player, 
    Entity::Stats stats,
    std::vector<std::shared_ptr<Weapon>> weapons
) :
    Entity(grid, name, stats, weapons),
    player(player),
    teeth(weapons[0])
{ }

// Need to tweak this so the player cannot overlap the enemy
void Enemy::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    if(isNeighbour(player)) {
        bite();
    }
    else {
        findPath(player->getPosition(), 1);
    }
}

bool Enemy::endTurnCondition(void) {
    return isNeighbour(player);
}

void Enemy::bite(void) {
    attack(player, teeth);
    useMoves(getStats().movesPerTurn);
}