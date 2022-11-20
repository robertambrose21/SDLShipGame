#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(std::shared_ptr<Entity> owner) :
    BehaviourStrategy(owner),
    target(nullptr),
    canPassTurn(false)
{
    entityPool = Application::getContext()->getEntityPool();
}

void ChaseAndAttackStrategy::onUpdate(const uint32_t& timeSinceLastFrame, bool& quit) {
    if(target == nullptr) {
        return;
    }

    if(owner->isNeighbour(target)) {
        if(owner->getCurrentWeapon()->hasFinished()) {
            canPassTurn = true;
        }
        else {
            owner->attack(target, owner->getCurrentWeapon());
        }
    }
    else if(owner->getMovesLeft() <= 0) {
        owner->getCurrentWeapon()->setFinished();
    }
    else {
        owner->findPath(target->getPosition(), 1);
    }
}

void ChaseAndAttackStrategy::onNextTurn(void) {
    // if(owner == nullptr) {
    //     return;
    // }
    
    target = findClosestTarget();
    canPassTurn = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return canPassTurn;
}

std::shared_ptr<Entity> ChaseAndAttackStrategy::findClosestTarget(void) {
    std::shared_ptr<Entity> closestEntity = nullptr;
    auto shortestDistance = std::numeric_limits<float>::max();
    
    for(auto [entityId, entity] : entityPool->getEntities()) {
        if(entity->getParticipantId() == owner->getParticipantId()) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(owner->getPosition()), glm::vec2(entity->getPosition()));

        if(distance < shortestDistance) {
            shortestDistance = distance;
            closestEntity = entity;
        }
    }

    return closestEntity;
}