#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(const std::shared_ptr<Entity>& owner) :
    BehaviourStrategy(owner),
    target(nullptr),
    canPassTurn(false)
{
    entityPool = Application::getContext()->getEntityPool();
    transmitter = std::dynamic_pointer_cast<GameServerMessagesTransmitter>(Application::getContext()->getServerMessagesTransmitter());
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
            transmitter->sendAttackEntity(0, owner->getId(), target->getId(), owner->getCurrentWeapon()->getId());
        }
    }
    else if(owner->getMovesLeft() <= 0) {
        owner->getCurrentWeapon()->setFinished();
    }
    else if(!owner->hasPath()) {
        owner->findPath(target->getPosition(), 1);
        transmitter->sendFindPath(0, owner->getId(), target->getPosition(), 1);
    }
}

void ChaseAndAttackStrategy::onNextTurn(void) {
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