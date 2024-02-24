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

    for(auto entity : participant->entities) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(turnNumber).empty()) {
            return false;
        }
    }

    receivedValidNextTurnFlag = false;
    return true;
}

// TODO: This should be queued up instead of just silently ignored.
// - May want to consider in the future sending an ACK to the server in case something gets stuck
void ClientTurnController::receiveSetNextTurnFlag(int participantId, int turnNumber) {
    if(this->currentParticipantId == participantId && this->turnNumber == turnNumber) {
        receivedValidNextTurnFlag = true;
    }
    // TODO: else, handle desync?
}