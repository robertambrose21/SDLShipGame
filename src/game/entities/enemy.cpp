#include "enemy.h"
#include "game/application/application.h"

Enemy::Enemy(
    const std::string& name,
    Entity::Stats stats
) :
    Entity(name, stats),
    target(nullptr),
    canPassTurn(false)
{
    entityPool = Application::getContext()->getEntityPool();
}

// Need to tweak this so the player cannot overlap the enemy
void Enemy::additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit) {
    if(target == nullptr) {
        return;
    }

    if(isNeighbour(target)) {
        if(getCurrentWeapon()->hasFinished()) {
            canPassTurn = true;
        }
        else {
            attack(target, getCurrentWeapon());
        }
    }
    else if(getMovesLeft() <= 0) {
        getCurrentWeapon()->setFinished();
    }
    else {
        findPath(target->getPosition(), 1);
    }
}

std::shared_ptr<Entity> Enemy::findClosestTarget(void) {
    std::shared_ptr<Entity> closestEntity = nullptr;
    auto shortestDistance = std::numeric_limits<float>::max();
    
    for(auto entity : entityPool->getEntities()) {
        if(entity->getParticipantId() == getParticipantId()) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition()));

        if(distance < shortestDistance) {
            shortestDistance = distance;
            closestEntity = entity;
        }
    }

    return closestEntity;
}

bool Enemy::endTurnCondition(void) {
    return canPassTurn;
}

void Enemy::nextTurn() {
    Entity::nextTurn();

    target = findClosestTarget();
    canPassTurn = false;
}