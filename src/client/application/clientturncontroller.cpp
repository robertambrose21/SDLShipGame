#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController(),
    receivedValidNextTurnFlag(false)
{ }

void ClientTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    // no-op
}

bool ClientTurnController::canProgressToNextTurn(int participantId) {
    if(!receivedValidNextTurnFlag) {
        return false;
    }

    auto& participant = participants[participantId];

    for(auto entity : participant->getEntities()) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(turnNumber).empty()) {
            return false;
        }
    }

    receivedValidNextTurnFlag = false;
    return true;
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, int turnNumber) {
    receivedValidNextTurnFlag = true;
}