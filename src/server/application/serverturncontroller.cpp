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

void ServerTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    auto& behaviourStrategy = participants[currentParticipantId]->behaviourStrategy;

    if(behaviourStrategy != nullptr) {
        behaviourStrategy->onUpdate(timeSinceLastFrame, quit);
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