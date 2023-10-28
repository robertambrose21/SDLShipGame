#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(ApplicationContext& context, int participantId) :
    BehaviourStrategy(participantId),
    context(context),
    canPassTurn(false)
{
    transmitter = (GameServerMessagesTransmitter*) context.getServerMessagesTransmitter();
}

void ChaseAndAttackStrategy::onUpdate(int64_t timeSinceLastFrame, bool& quit) {
    auto turnController = context.getTurnController();
    auto participant = turnController->getParticipant(participantId);
    auto isPassable = true;

    for(auto entity : participant->entities) {
        auto target = findClosestTarget(entity);
        auto bWeapon = target == nullptr ? nullptr : getBestInRangeWeapon(entity, target->getPosition());

        if(entity->getFrozenFor() > 0) {
            continue;
        }

        if(target != nullptr && entity->isNeighbour(target)) {
            if(turnController->performAttackAction(entity, entity->getCurrentWeapon(), target->getPosition())) {
                transmitter->sendAttack(0, entity->getId(), target->getPosition(), entity->getCurrentWeapon()->getId());
                isPassable = false;
            }
        }
        else if(bWeapon != nullptr && target != nullptr && bWeapon->isInRange(target->getPosition()) &&
                turnController->performAttackAction(entity, bWeapon, target->getPosition())) {
            // TODO: Temporary hack for actions getting sent too fast - need to implement some sort of queueing system
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            transmitter->sendAttack(0, entity->getId(), target->getPosition(), bWeapon->getId());
            isPassable = false;
        }
        else if(entity->hasPath()) {
            isPassable = isPassable && 
                participant->actions[TurnController::Action::Move] <= 0 && entity->getMovesLeft() <= 0;
        }
        else if(target != nullptr && 
                    glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition())) <= entity->getAggroRange() && 
                    turnController->performMoveAction(entity, target->getPosition(), 1)) {
            transmitter->sendFindPath(0, entity->getId(), target->getPosition(), 1);
            isPassable = false;
        }
    }

    canPassTurn = isPassable;
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
    std::map<TurnController::Action, int> actions;
    std::vector<DiceActionResult> serializedActions;

    for(int i = 0; i < 3; i++) {
        DiceActionResult dice;

        dice.rollNumber = randomD6();
        
        for(int j = 0; j < dice.rollNumber; j++) {
            auto action = randomRange(0, TurnController::Action::Count - 1);
            
            dice.actions[j] = action;
            actions[(TurnController::Action) action]++;
        }

        serializedActions.push_back(dice);
    }

    context.getTurnController()->setActions(participantId, actions);
    transmitter->sendActionsRollResponse(0, participantId, serializedActions);

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