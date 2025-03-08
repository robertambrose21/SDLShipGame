#include "clientturncontroller.h"

ClientTurnController::ClientTurnController() :
    TurnController()
{ }

void ClientTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    // no-op
}

bool ClientTurnController::canProgressToNextTurn(Engagement* engagement) {
    if(!nextTurnFlags.contains(engagement->getId()) || nextTurnFlags[engagement->getId()].empty()) {
        return false;
    }

    auto participant = engagement->getCurrentParticipant();

    for(auto entity : participant->getEntities()) {
        if(entity->hasAnimationsInProgress() || !entity->getActionsChain(engagement->getTurnNumber()).empty()) {
            return false;
        }
    }

    nextTurnFlags[engagement->getId()].pop();
    return true;
}

void ClientTurnController::onParticipantTurnEnd(Engagement* engagement) {
    // no-op
}

void ClientTurnController::receiveSetNextTurnFlag(int participantId, uint32_t engagementId, int receivedTurnNumber) {
    nextTurnFlags[engagementId].push(receivedTurnNumber);
}