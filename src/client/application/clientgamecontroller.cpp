#include "clientgamecontroller.h"

ClientGameController::ClientGameController() :
    GameController()
{ }

void ClientGameController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    // no-op
}

bool ClientGameController::canProgressToNextTurn(Engagement* engagement) {
    if(!nextTurnFlags.contains(engagement->getId()) || nextTurnFlags[engagement->getId()].empty()) {
        return false;
    }

    auto participant = engagement->getCurrentParticipant();

    for(auto entity : participant->getActors()) {
        auto actor = context->getEntityRegistry().try_get<Actor>(entity);
        auto& chain = context->getEntityRegistry().get<ActionChain>(entity).chain;
        auto turnNumber = engagement->getTurnNumber();

        if(chain.contains(turnNumber) && !chain.at(turnNumber).empty()) {
            return false;
        }

        if(actor->hasAnimationsInProgress()) {
            return false;
        }
    }

    nextTurnFlags[engagement->getId()].pop();

    return true;
}

void ClientGameController::onParticipantTurnEnd(Engagement* engagement) {
    // no-op
}

void ClientGameController::receiveSetNextTurnFlag(int participantId, uint32_t engagementId, int receivedTurnNumber) {
    nextTurnFlags[engagementId].push(receivedTurnNumber);
}

void ClientGameController::onPublish(const Event<RemoveEngagementEventData>& event) {
    nextTurnFlags.erase(event.data.engagementId);
}