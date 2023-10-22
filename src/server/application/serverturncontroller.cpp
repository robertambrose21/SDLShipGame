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

    if(participant->behaviourStrategy != nullptr && participant->behaviourStrategy->endTurnCondition()) {
        transmitter->sendNextTurn(0, currentParticipant, turnNumber);
        return true;
    }

    bool haveEntitiesTurnsFinished = true;
    bool haveEntitiesAnimationsFinished = true;
    for(auto entity : participant->entities) {
        haveEntitiesTurnsFinished = haveEntitiesTurnsFinished && !entity->isTurnInProgress();
        haveEntitiesAnimationsFinished = haveEntitiesAnimationsFinished && !entity->hasAnimationsInProgress();
    }

    if(!haveEntitiesAnimationsFinished) {
        return false;
    }

    bool haveActionsCompleted = participant->hasRolledForActions;

    for(auto [_, numActionsLeft] : participant->actions) {
        haveActionsCompleted = haveActionsCompleted && numActionsLeft == 0;
    }

    auto nextTurn = (haveEntitiesTurnsFinished && haveActionsCompleted) || participant->passNextTurn;

    if(nextTurn) {
        transmitter->sendNextTurn(0, currentParticipant, turnNumber);
    }

    return nextTurn;
}

void ServerTurnController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}