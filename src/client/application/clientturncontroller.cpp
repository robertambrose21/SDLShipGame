#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController()
{ }

void ClientTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    // no-op
}

bool ClientTurnController::canProgressToNextTurn(Engagement* engagement) {
    if(nextTurnFlags.empty()) {
        return false;
    }

    // auto& participant = participants[participantId];
    auto participant = engagement->getCurrentParticipant();

    for(auto entity : participant->getEntities()) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(engagement->getTurnNumber()).empty()) {
            return false;
        }
    }

    nextTurnFlags.pop();
    return true;
}

void ClientTurnController::onParticipantTurnEnd(Engagement* engagement) {
    // no-op
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, int receivedTurnNumber) {
    nextTurnFlags.push(receivedTurnNumber);
}