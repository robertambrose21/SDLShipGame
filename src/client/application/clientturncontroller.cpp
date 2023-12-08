#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController(),
    receivedValidNextTurnFlag(false)
{ }

bool ClientTurnController::canProgressToNextTurn(int participantId) {
    if(!receivedValidNextTurnFlag) {
        return false;
    }

    auto& participant = participants[participantId];

    for(auto entity : participant->entities) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(turnNumber).empty()) {
            return false;
        }
    }

    receivedValidNextTurnFlag = false;
    return true;
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, int turnNumber) {
    if(this->currentParticipant == participantId && this->turnNumber == turnNumber) {
        receivedValidNextTurnFlag = true;
    }
    // TODO: else, handle desync?
}