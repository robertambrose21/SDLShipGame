#include "enemy.h"

Enemy::Enemy(
    std::shared_ptr<GridRenderer> grid,
    const std::string& name, 
    std::shared_ptr<PlayerController> player, 
    Entity::Stats stats
) :
    Entity(grid, name, stats),
    player(player)
{ }

void Enemy::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    //
}