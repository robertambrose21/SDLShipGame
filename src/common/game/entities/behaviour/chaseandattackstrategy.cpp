#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(ApplicationContext& context) :
    context(context),
    canPassTurn(true)
{
    transmitter = (GameServerMessagesTransmitter*) context.getServerMessagesTransmitter();
}

void ChaseAndAttackStrategy::onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit) {
    if(canPassTurn) {
        return;
    }

    auto turnController = context.getTurnController();
    auto participant = turnController->getParticipant(participantId);
    auto entitiesPassed = 0;

    for(auto entity : participant->entities) {
        if(doTurnForEntity(entity, participantId)) {
            entitiesPassed++;
        }
    }

    canPassTurn = entitiesPassed == participant->entities.size();
}

bool ChaseAndAttackStrategy::doTurnForEntity(Entity* entity, int participantId) {
    if(!entity->isTurnInProgress()) {
        return true;
    }

    if(entity->getIsFrozen()) {
        return true;
    }

    auto target = findClosestTarget(entity, participantId);

    if(target == nullptr) {
        return true;
    }

    auto bWeapon = getBestInRangeWeapon(entity, target->getPosition());
    auto turnController = context.getTurnController();
    auto turnNumber = turnController->getTurnNumber();

    // TODO: Change 'current weapon' to best melee weapon
    if(entity->isNeighbour(target)) {
        auto action = std::make_unique<AttackAction>(turnNumber, entity, entity->getCurrentWeapon(), target->getPosition());
        
        if(!turnController->queueAction(std::move(action))) {
            return true;
        }
    }
    else if(bWeapon != nullptr) {
        auto action = std::make_unique<AttackAction>(turnNumber, entity, bWeapon, target->getPosition());

        if(!turnController->queueAction(std::move(action))) {
            return true;
        }
    }
    else if(!entity->hasPath()) {
        auto distanceToTarget = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition()));
        auto action = std::make_unique<MoveAction>(turnNumber, entity, target->getPosition(), 1);
        
        if(!distanceToTarget <= entity->getAggroRange() && !turnController->queueAction(std::move(action))) {
            return true;
        }
    }

    return false;
}

Weapon* ChaseAndAttackStrategy::getBestInRangeWeapon(
    Entity* attacker, 
    const glm::ivec2& target
) {
    for(auto weapon : attacker->getWeapons()) {
        if(weapon->getType() == Weapon::Type::PROJECTILE && weapon->isInRange(target)) {
            return weapon;
        }
    }

    return nullptr;
}

void ChaseAndAttackStrategy::onNextTurn(void) {
    canPassTurn = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return canPassTurn;
}

Entity* ChaseAndAttackStrategy::findClosestTarget(Entity* attacker, int participantId) {
    Entity* closestEntity = nullptr;
    auto shortestDistance = std::numeric_limits<float>::max();
    
    for(auto entity : context.getEntityPool()->getEntities()) {
        if(entity->getParticipantId() == participantId) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(attacker->getPosition()), glm::vec2(entity->getPosition()));

        if(distance < shortestDistance) {
            shortestDistance = distance;
            closestEntity = entity;
        }
    }

    return closestEntity;
}