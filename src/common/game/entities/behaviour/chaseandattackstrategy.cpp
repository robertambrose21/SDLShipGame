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

    auto gameController = getContext().getGameController();
    auto participant = gameController->getParticipant(participantId);

    if(!participant->hasAnyEngagement()) {
        return;
    }

    auto entitiesPassed = 0;
    auto entitiesDisengaged = 0;

    for(auto entity : participant->getEntities()) {
        auto [canEntityPass, canEntityDisengage] = doTurnForEntity(entity, participant);
        
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
        auto participants = gameController->getParticipants();

        for(auto other : participants) {
            if(participant->hasEngagement(other)) {
                gameController->getEngagementController()->disengage(participant->getEngagement()->getId(), other);
            }
        }
    }
}

ChaseAndAttackStrategy::EntityTurnResult ChaseAndAttackStrategy::doTurnForEntity(Entity* entity, Participant* participant) {
    if(!entity->isTurnInProgress()) {
        return { true, false };
    }

    if(entity->getIsFrozen()) {
        return { true, true };
    }

    auto target = getContext().getEntityPool()->findClosestTarget(entity, participant->getId());

    if(target == nullptr) {
        return { true, true };
    }

    auto bWeapon = getBestInRangeWeapon(entity, target->getPosition());
    auto gameController = getContext().getGameController();
    auto turnNumber = participant->getEngagement()->getTurnNumber();

    // TODO: Change 'current weapon' to best melee weapon
    if(entity->isNeighbour(target)) {
        auto action = std::make_unique<AttackAction>(
            participant, 
            entity, 
            turnNumber, 
            entity->getCurrentWeapon(), 
            target->getPosition()
        );
        
        if(entity->getCurrentWeapon()->getUsesLeft() <= 0 || !gameController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(bWeapon != nullptr) {
        auto action = std::make_unique<AttackAction>(
            participant, 
            entity, 
            turnNumber,
            bWeapon, 
            target->getPosition()
        );

        if(bWeapon->getUsesLeft() <= 0 || !gameController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(!entity->hasPath()) {
        auto distanceToTarget = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition()));
        auto action = std::make_unique<MoveAction>(participant, entity, turnNumber, target->getPosition(), 1);
        
        if(!distanceToTarget <= entity->getAggroRange() && !gameController->queueAction(std::move(action))) {
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
        if(weapon->getType() == Stats::WeaponStats::PROJECTILE && weapon->isInRange(target)) {
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
