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
    int damage = 1;
    player->doDamage(1);
    useMoves(getStats().movesPerTurn);

    std::cout << "Bit the player for [" << damage << "] damage, player now has [" << player->getStats().hp << "] hp" << std::endl;
}