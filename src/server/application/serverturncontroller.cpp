#include "serverturncontroller.h"

ServerTurnController::ServerTurnController() :
    TurnController()
{
    addOnNextTurnFunction([&](auto const& currentParticipantId, auto const& turnNumber) {
        auto behaviourStrategy = participants[currentParticipantId]->getBehaviourStrategy();

        if(behaviourStrategy != nullptr) {
            behaviourStrategy->onNextTurn();
        }
    });
}

void ServerTurnController::attachParticipantToClient(int participantId, int clientIndex) {
    participantToClient[participantId] = clientIndex;
}

int ServerTurnController::getAttachedClient(int participantId) const {
    if(!participantToClient.contains(participantId)) {
        return -1;
    }

    return participantToClient.at(participantId);
}

int ServerTurnController::getAttachedParticipantId(int clientIndex) const {
    for(auto [participantId, clientIndexToFind] : participantToClient) {
        if(clientIndexToFind == clientIndex) {
            return participantId;
        }
    }

    return -1;
}

const std::map<int, int>& ServerTurnController::getAllAttachedClients(void) const {
    return participantToClient;
}

void ServerTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [participantId, _] : participants) {
        // TODO: This should be called on end of turn, not every update tick
        assignEngagements(participantId);
    }

    auto participant = participants.at(currentParticipantId).get();

    if(participant->getBehaviourStrategy() != nullptr) {
        participant->getBehaviourStrategy()->onUpdate(participant->getId(), timeSinceLastFrame, quit);
    }

    checkForItems();
}

bool ServerTurnController::canProgressToNextTurn(int participantId) {
    game_assert(transmitter != nullptr);

    auto& participant = participants[participantId];

    if(!participant->getIsReady()) {
        return false;
    }

    // TODO: Need to add a check to see if any fully dead participants still have projectiles/animations in progress
    if(participant->getEntities().empty()) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
        return true;
    }

    bool haveEntitiesTurnsFinished = true;
    bool haveEntitiesActionsFinished = true;
    for(auto entity : participant->getEntities()) {
        haveEntitiesTurnsFinished = haveEntitiesTurnsFinished && !entity->isTurnInProgress();
        haveEntitiesActionsFinished = haveEntitiesActionsFinished && !entity->hasAnimationsInProgress() 
            && entity->getActionsChain(turnNumber).empty();
    }

    if(!haveEntitiesActionsFinished) {
        return false;
    }

    if(participant->getBehaviourStrategy() != nullptr && participant->getBehaviourStrategy()->endTurnCondition()) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->isPassingNextTurn();

    if(nextTurn) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
    }

    return nextTurn;
}

void ServerTurnController::checkForItems(void) {
    auto itemController = context->getItemController();

    for(auto& [_, participant] : participants) {
        if(!participant->getIsPlayer()) {
            continue;
        }

        for(auto entity : participant->getEntities()) {
            auto items = itemController->getItemsAt(entity->getPosition());

            queueAction(std::make_unique<TakeItemAction>(turnNumber, entity, items));
        }
    }
}

void ServerTurnController::assignEngagements(int participantIdToCheck) {
    auto behaviour = participants[participantIdToCheck]->getBehaviourStrategy();

    for(auto& [participantId, participant] : participants) {
        compareAndEngagementParticipants(participant.get(), participants[participantIdToCheck].get());
    }
}

void ServerTurnController::compareAndEngagementParticipants(Participant* participantA, Participant* participantB) {
    if(participantA->getId() == participantB->getId()) {
        return;
    }

    // We only need to check the engagements for one of the participants
    if(participantA->getEngagements().contains(participantB->getId())) {
        if(participantA->getEntities().empty() || participantB->getEntities().empty()) {
            disengage(participantA->getId(), participantB->getId());
        }

        return;
    }

    // Exit early if we find an engagement
    for(auto entityToCheck : participantA->getEntities()) {
        if(hasEntityEngagement(entityToCheck, participantB)) {
            engage(participantA->getId(), participantB->getId());
            return;
        }
    }
}

bool ServerTurnController::hasEntityEngagement(Entity* entityToCheck, Participant* participant) {
    for(auto entity : participant->getEntities()) {
        auto distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(entityToCheck->getPosition()));

        if(distance <= entityToCheck->getAggroRange()) {
            return true;
        }
    }

    return false;
}

void ServerTurnController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}