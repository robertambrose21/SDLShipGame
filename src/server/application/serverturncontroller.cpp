#include "serverturncontroller.h"

ServerTurnController::ServerTurnController() :
    TurnController()
{ }

void ServerTurnController::update(int64_t timeSinceLastFrame, bool& quit) {
    if(participants.size() <= 0) { // TODO: Calling this twice, fix this somehow
        return;
    }
    
    auto& bs = participants[currentParticipant]->behaviourStrategy;

    if(bs != nullptr) {
        bs->onUpdate(timeSinceLastFrame, quit);
    }
    
    TurnController::update(timeSinceLastFrame, quit);
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
        transmitter->sendNextTurn(0, currentParticipant, turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->passNextTurn;

    if(nextTurn) {
        transmitter->sendNextTurn(0, currentParticipant, turnNumber);
    }

    return nextTurn;
}

void ServerTurnController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}