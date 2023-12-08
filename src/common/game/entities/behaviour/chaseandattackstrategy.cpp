#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(ApplicationContext& context, int participantId) :
    BehaviourStrategy(participantId),
    context(context),
    canPassTurn(false)
{
    transmitter = (GameServerMessagesTransmitter*) context.getServerMessagesTransmitter();
}

void ChaseAndAttackStrategy::onUpdate(int64_t timeSinceLastFrame, bool& quit) {
    if(canPassTurn) {
        return;
    }

    auto turnController = context.getTurnController();
    auto participant = turnController->getParticipant(participantId);
    auto entitiesPassed = 0;

    for(auto entity : participant->entities) {
        if(doTurnForEntity(entity)) {
            entitiesPassed++;
        }
    }

    canPassTurn = entitiesPassed == participant->entities.size();
}

bool ChaseAndAttackStrategy::doTurnForEntity(Entity* entity) {
    if(!entity->isTurnInProgress()) {
        return true;
    }

    if(entity->getFrozenFor() > 0) {
        return true;
    }

    auto target = findClosestTarget(entity);

    if(target == nullptr) {
        return true;
    }

    auto bWeapon = getBestInRangeWeapon(entity, target->getPosition());

    // TODO: This is super gross - server turn controller should override queueAction to also send an attack
    if(entity->isNeighbour(target) && context.getTurnController()->queueAction(
            std::move(std::make_unique<AttackAction>(
                context.getTurnController()->getTurnNumber(), entity, entity->getCurrentWeapon(), target->getPosition())))) {
        transmitter->sendAttack(0, entity->getId(), target->getPosition(), entity->getCurrentWeapon()->getId());
    }
    else if(bWeapon != nullptr && bWeapon->isInRange(target->getPosition()) && 
            context.getTurnController()->queueAction(std::make_unique<AttackAction>(
                context.getTurnController()->getTurnNumber(), entity, bWeapon, target->getPosition()))) {
        transmitter->sendAttack(0, entity->getId(), target->getPosition(), bWeapon->getId());
    }
    else if(!entity->hasPath()) {
        if(glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition())) <= entity->getAggroRange() &&
                context.getTurnController()->queueAction(std::make_unique<MoveAction>(
                    context.getTurnController()->getTurnNumber(), entity, target->getPosition(), 1))) {
            transmitter->sendFindPath(0, entity->getId(), target->getPosition(), 1);
        }
        else {
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
        auto dist = glm::distance(glm::vec2(attacker->getPosition()), glm::vec2(target));

        if(weapon->getType() == Weapon::Type::PROJECTILE && weapon->getStats().range >= dist) {
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

Entity* ChaseAndAttackStrategy::findClosestTarget(Entity* attacker) {
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