#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController()
{ }

void ClientTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    // no-op
}

bool ClientTurnController::canProgressToNextTurn(int participantId) {
    if(nextTurnFlags.empty()) {
        return false;
    }

    auto& participant = participants[participantId];

    for(auto entity : participant->getEntities()) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(turnNumber).empty()) {
            return false;
        }
    }

    nextTurnFlags.pop();
    return true;
}

void ClientTurnController::onParticipantTurnEnd(int participantId) {
    // no-op
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, int receivedTurnNumber) {
    nextTurnFlags.push(receivedTurnNumber);
}