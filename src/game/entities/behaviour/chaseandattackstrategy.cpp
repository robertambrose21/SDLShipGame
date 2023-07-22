#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(int participantId) :
    BehaviourStrategy(participantId),
    canPassTurn(false)
{
    transmitter = (GameServerMessagesTransmitter*) Application::getContext().getServerMessagesTransmitter();
}

void ChaseAndAttackStrategy::onUpdate(uint32_t timeSinceLastFrame, bool& quit) {
    auto& turnController = Application::getContext().getTurnController();
    auto participant = turnController.getParticipant(participantId);
    auto totalPassable = true;

    for(auto entity : participant->entities) {
        auto target = findClosestTarget(entity);
        auto bWeapon = target == nullptr ? nullptr : getBestInRangeWeapon(entity, target->getPosition());

        if(entity->getFrozenFor() > 0) {
            continue;
        }

        if(entity->isNeighbour(target)) {
            if(turnController.performAttackAction(entity, entity->getCurrentWeapon(), target->getPosition())) {
                transmitter->sendAttack(0, entity->getId(), target->getPosition(), entity->getCurrentWeapon()->getId());
            };

            totalPassable = totalPassable && participant->actions[TurnController::Action::Attack] <= 0;
        }
        else if(participant->actions[TurnController::Action::Attack] > 0 && bWeapon != nullptr) {
            if(turnController.performAttackAction(entity, bWeapon, target->getPosition())) {
                transmitter->sendAttack(0, entity->getId(), target->getPosition(), bWeapon->getId());
            }

            totalPassable = totalPassable && participant->actions[TurnController::Action::Attack] <= 0;
        }
        else {
            if(turnController.performMoveAction(entity, target->getPosition(), 1)) {
                transmitter->sendFindPath(0, entity->getId(), target->getPosition(), 1);
            }

            if(entity->hasPath()) {
                totalPassable = totalPassable && participant->actions[TurnController::Action::Move] <= 0 && entity->getMovesLeft() <= 0;
            }
        }
    }

    canPassTurn = totalPassable;
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

    Application::getContext().getTurnController().setActions(participantId, actions);
    transmitter->sendActionsRollResponse(0, participantId, serializedActions);

    canPassTurn = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return canPassTurn;
}

Entity* ChaseAndAttackStrategy::findClosestTarget(Entity* attacker) {
    Entity* closestEntity = nullptr;
    auto shortestDistance = std::numeric_limits<float>::max();
    
    for(auto entity : Application::getContext().getEntityPool().getEntities()) {
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