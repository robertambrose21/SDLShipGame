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

    auto actorsPassed = 0;
    auto actorsDisengaged = 0;

    for(auto entity : participant->getActors()) {
        auto [canActorPass, canActorDisengage] = doTurnForActor(entity, participant);
        
        if(canActorPass) {
            actorsPassed++;
        }
        if(canActorDisengage) {
            actorsDisengaged++;
        }
    }

    canPassTurn = actorsPassed == participant->getActors().size();
    canDisengage = actorsDisengaged == participant->getActors().size();

    if(canDisengage) {
        auto participants = gameController->getParticipants();

        for(auto other : participants) {
            if(participant->hasEngagement(other)) {
                gameController->getEngagementController()->disengage(participant->getEngagement()->getId(), other);
            }
        }
    }
}

ChaseAndAttackStrategy::ActorTurnResult ChaseAndAttackStrategy::doTurnForActor(
    entt::entity entity, 
    Participant* participant
) {
    auto& actor = getContext().getEntityRegistry().get<Actor>(entity);

    if(!getContext().getActorController()->isTurnInProgress(entity)) {
        return { true, false };
    }

    if(actor.getIsFrozen()) {
        return { true, true };
    }

    auto targetEntity = getContext().getActorPool()->findClosestTarget(entity, participant->getId());

    if(!targetEntity.has_value()) {
        return { true, true };
    }

    auto const& actorPosition = getContext().getEntityRegistry().get<Position>(entity);
    auto const& targetPosition = getContext().getEntityRegistry().get<Position>(targetEntity.value());

    auto bWeapon = getBestInRangeWeapon(&actor, targetPosition);
    auto gameController = getContext().getGameController();
    auto turnNumber = participant->getEngagement()->getTurnNumber();

    // TODO: Change 'current weapon' to best melee weapon
    if(getContext().getGrid()->areNeighbours(actorPosition, targetPosition)) {
        auto action = std::make_unique<AttackAction>(
            participant, 
            entity, 
            turnNumber, 
            actor.getCurrentWeapon(), 
            targetPosition
        );
        
        if(actor.getCurrentWeapon()->getUsesLeft() <= 0 || !gameController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(bWeapon != nullptr) {
        auto action = std::make_unique<AttackAction>(
            participant, 
            entity, 
            turnNumber,
            bWeapon, 
            targetPosition
        );

        if(bWeapon->getUsesLeft() <= 0 || !gameController->queueAction(std::move(action))) {
            return { true, false };
        }
    }
    else if(!actor.hasPath()) {
        auto distanceToTarget = glm::distance(glm::vec2(actorPosition), glm::vec2(targetPosition));
        auto action = std::make_unique<MoveAction>(
            participant, 
            entity, 
            turnNumber, 
            targetPosition, 
            1
        );
        
        if(!distanceToTarget <= actor.getAggroRange() && !gameController->queueAction(std::move(action))) {
            return { true, false };
        }
    }

    return { false, false };
}

Weapon* ChaseAndAttackStrategy::getBestInRangeWeapon(
    Actor* attacker, 
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
