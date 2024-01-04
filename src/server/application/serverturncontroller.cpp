#include "serverturncontroller.h"

ServerTurnController::ServerTurnController() :
    TurnController()
{
    addOnNextTurnFunction([&](auto const& currentParticipantId, auto const& turnNumber) {
        auto& behaviourStrategy = participants[currentParticipantId]->behaviourStrategy;

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
    auto participant = participants.at(currentParticipantId).get();

    if(participant->behaviourStrategy != nullptr) {
        participant->behaviourStrategy->onUpdate(participant->id, timeSinceLastFrame, quit);
    }
}

bool ServerTurnController::canProgressToNextTurn(int participantId) {
    game_assert(transmitter != nullptr);

    auto& participant = participants[participantId];

    if(!participant->isReady) {
        return false;
    }

    if(participant->entities.empty()) {
        return false;
    }

    bool haveEntitiesTurnsFinished = true;
    bool haveEntitiesActionsFinished = true;
    for(auto entity : participant->entities) {
        haveEntitiesTurnsFinished = haveEntitiesTurnsFinished && !entity->isTurnInProgress();
        haveEntitiesActionsFinished = haveEntitiesActionsFinished && !entity->hasAnimationsInProgress() 
            && entity->getActionsChain(turnNumber).empty();
    }

    if(!haveEntitiesActionsFinished) {
        return false;
    }

    if(participant->behaviourStrategy != nullptr && participant->behaviourStrategy->endTurnCondition()) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->passNextTurn;

    if(nextTurn) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
    }

    return nextTurn;
}

void ServerTurnController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}