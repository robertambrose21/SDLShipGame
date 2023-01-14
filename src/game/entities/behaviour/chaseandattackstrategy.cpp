#include "chaseandattackstrategy.h"

ChaseAndAttackStrategy::ChaseAndAttackStrategy(int participantId) :
    BehaviourStrategy(participantId),
    // target(nullptr),
    canPassTurn(false)
{
    auto context = Application::getContext();
    entityPool = context->getEntityPool();
    turnController = context->getTurnController();
    transmitter = std::dynamic_pointer_cast<GameServerMessagesTransmitter>(context->getServerMessagesTransmitter());

    presetActions = {
        { TurnController::Action::Move, 1 },
        { TurnController::Action::Attack, 1}
    };
}

void ChaseAndAttackStrategy::onUpdate(uint32_t timeSinceLastFrame, bool& quit) {
    auto participant = turnController->getParticipant(participantId);
    auto totalPassable = true;

    for(auto entity : participant->entities) {
        auto target = findClosestTarget(entity);

        if(entity->isNeighbour(target)) {
            if(turnController->performAttackAction(entity, entity->getCurrentWeapon(), target)) {
                transmitter->sendAttackEntity(0, entity->getId(), target->getId(), entity->getCurrentWeapon()->getId());
            };

            totalPassable = totalPassable && participant->actions[TurnController::Action::Attack] <= 0;
        }
        else {
            if(turnController->performMoveAction(entity, target->getPosition(), 1)) {
                transmitter->sendFindPath(0, entity->getId(), target->getPosition(), 1);
            }

            if(entity->hasPath()) {
                totalPassable = totalPassable && participant->actions[TurnController::Action::Move] <= 0 && entity->getMovesLeft() <= 0;
            }
        }
    }

    canPassTurn = totalPassable;
}

void ChaseAndAttackStrategy::onNextTurn(void) {
    // target = findClosestTarget();
    turnController->setActions(participantId, presetActions);

    std::vector<int> actions = { 0, 1 };

    transmitter->sendActionsRollResponse(0, participantId, 2, &actions[0]);

    canPassTurn = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return 
        canPassTurn; //|| 
        // turnController->getParticipant(participantId)->actions[TurnController::Action::Move] <= 0;
}

std::shared_ptr<Entity> ChaseAndAttackStrategy::findClosestTarget(const std::shared_ptr<Entity>& attacker) {
    std::shared_ptr<Entity> closestEntity = nullptr;
    auto shortestDistance = std::numeric_limits<float>::max();
    
    for(auto [entityId, entity] : entityPool->getEntities()) {
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