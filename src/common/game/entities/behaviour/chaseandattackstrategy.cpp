#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(ApplicationContext& context) :
    BehaviourStrategy(context),
    canPassTurn(true),
    canDisengage(false)
{
    transmitter = (GameServerMessagesTransmitter*) context.getServerMessagesTransmitter();
}

void ChaseAndAttackStrategy::onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit) {
    if(canPassTurn) {
        return;
    }

    auto turnController = getContext().getTurnController();
    auto participant = turnController->getParticipant(participantId);
    auto entitiesPassed = 0;
    auto entitiesDisengaged = 0;

    for(auto entity : participant->getEntities()) {
        auto [canEntityPass, canEntityDisengage] = doTurnForEntity(entity, participantId);
        
        if(canEntityPass) {
            entitiesPassed++;
        }
        if(canEntityDisengage) {
            entitiesDisengaged++;
        }
    }

    canPassTurn = entitiesPassed == participant->getEntities().size();
    canDisengage = entitiesDisengaged == participant->getEntities().size();

    if(canDisengage) {
        auto participants = turnController->getParticipants();

        for(auto participant : participants) {
            if(participantId != participant->getId()) {
                turnController->disengage(participantId, participant->getId());
            }
        }
    }
}

ChaseAndAttackStrategy::EntityTurnResult ChaseAndAttackStrategy::doTurnForEntity(Entity* entity, int participantId) {
    if(!entity->isTurnInProgress()) {
        return { true, false };
    }

    if(entity->getIsFrozen()) {
        return { true, true };
    }

    auto target = getContext().getEntityPool()->findClosestTarget(entity, participantId);

    if(target == nullptr) {
        return { true, true };
    }

    auto bWeapon = getBestInRangeWeapon(entity, target->getPosition());
    auto turnController = getContext().getTurnController();
    auto turnNumber = turnController->getTurnNumber();

    // TODO: Change 'current weapon' to best melee weapon
    if(entity->isNeighbour(target)) {
        auto action = std::make_unique<AttackAction>(turnNumber, entity, entity->getCurrentWeapon(), target->getPosition());
        
        if(!turnController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(bWeapon != nullptr) {
        auto action = std::make_unique<AttackAction>(turnNumber, entity, bWeapon, target->getPosition());

        if(!turnController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(!entity->hasPath()) {
        auto distanceToTarget = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition()));
        auto action = std::make_unique<MoveAction>(turnNumber, entity, target->getPosition(), 1);
        
        if(!distanceToTarget <= entity->getAggroRange() && !turnController->queueAction(std::move(action))) {
            return { true, false };
        }
    }

    return { false, false };
}

Weapon* ChaseAndAttackStrategy::getBestInRangeWeapon(
    Entity* attacker, 
    const glm::ivec2& target
) {
    for(auto weapon : attacker->getWeapons()) {
        if(weapon->getType() == WeaponStats::PROJECTILE && weapon->isInRange(target)) {
            return weapon;
        }
    }

    return nullptr;
}

void ChaseAndAttackStrategy::onNextTurn(void) {
    canPassTurn = false;
    canDisengage = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return canPassTurn;
}

bool ChaseAndAttackStrategy::disengageCondition(void) {
    return canDisengage;
}
