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
}

void ChaseAndAttackStrategy::onUpdate(uint32_t timeSinceLastFrame, bool& quit) {
    auto participant = turnController->getParticipant(participantId);
    auto totalPassable = true;

    for(auto entity : participant->entities) {
        auto target = findClosestTarget(entity);
        auto bWeapon = target == nullptr ? nullptr : getBestInRangeWeapon(entity, target->getPosition());

        if(entity->getIsFrozen()) {
            continue;
        }

        if(entity->isNeighbour(target)) {
            if(turnController->performAttackAction(entity, entity->getCurrentWeapon(), target)) {
                transmitter->sendAttackEntity(0, entity->getId(), target->getId(), entity->getCurrentWeapon()->getId());
            };

            totalPassable = totalPassable && participant->actions[TurnController::Action::Attack] <= 0;
        }
        else if(participant->actions[TurnController::Action::Attack] > 0 && bWeapon != nullptr) {
            if(turnController->performAttackAction(entity, bWeapon, target)) {
                transmitter->sendAttackEntity(0, entity->getId(), target->getId(), bWeapon->getId());
            }

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

std::shared_ptr<Weapon> ChaseAndAttackStrategy::getBestInRangeWeapon(
    const std::shared_ptr<Entity>& attacker, 
    const glm::ivec2& target
) {
    for(auto [_, weapon] : attacker->getWeapons()) {
        auto dist = glm::distance(glm::vec2(attacker->getPosition()), glm::vec2(target));

        if(weapon->getType() == Weapon::Type::PROJECTILE && weapon->getStats().range >= dist) {
            return weapon;
        }
    }

    return nullptr;
}

void ChaseAndAttackStrategy::onNextTurn(void) {
    // target = findClosestTarget();

    std::map<TurnController::Action, int> presetActions = {
        { TurnController::Action::Move, randomDN(3) },
        { TurnController::Action::Attack, randomDN(3) }
    };

    turnController->setActions(participantId, presetActions);

    std::vector<int> actions;// = { 0, 1 };
    for(auto [action, num] : presetActions) {
        for(int i = 0; i < num; i++) {
            actions.push_back(action);
        }
    }

    transmitter->sendActionsRollResponse(0, participantId, actions.size(), &actions[0]);

    canPassTurn = false;
}

bool ChaseAndAttackStrategy::endTurnCondition(void) {
    return canPassTurn;
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