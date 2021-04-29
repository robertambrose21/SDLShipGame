#include "enemy.h"

Enemy::Enemy(
    std::shared_ptr<Grid> grid,
    const std::string& name, 
    std::shared_ptr<PlayerController> player, 
    int movesPerTurn
) :
    Entity(grid, name, movesPerTurn),
    player(player)
{ }

void Enemy::update(const Uint32& timeSinceLastFrame, bool& quit) {
    glm::vec2 direction = glm::normalize(player->getPartialPosition() - getPartialPosition());

    setPartialPosition(getPartialPosition() + direction * (timeSinceLastFrame / getSpeed()));
}