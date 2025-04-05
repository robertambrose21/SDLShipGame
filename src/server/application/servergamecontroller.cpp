#include "servergamecontroller.h"

ServerGameController::ServerGameController() :
    GameController()
{ }

void ServerGameController::attachParticipantToClient(int participantId, int clientIndex, uint64_t clientId) {
    participantToClient[participantId] = { clientId, clientIndex };
    spdlog::debug("Attached client [{}] to participant [{}]", clientIndex, participantId);
}

void ServerGameController::detachParticipantFromClient(int clientIndex) {
    auto participantId = getAttachedParticipantId(clientIndex);
    participantToClient[participantId].index = -1; // Set to unused clientIndex

    spdlog::debug("Detached client [{}] from participant [{}]", clientIndex, participantId);
}

int ServerGameController::getAttachedClient(int participantId) const {
    if(!participantToClient.contains(participantId)) {
        return -1;
    }

    return participantToClient.at(participantId).index;
}

int ServerGameController::getAttachedParticipantId(int clientIndex) const {
    for(auto [participantId, client] : participantToClient) {
        if(client.index == clientIndex) {
            return participantId;
        }
    }

    return -1;
}

int ServerGameController::getParticipantByClientId(uint64_t clientId) const {
    for(auto [participantId, client] : participantToClient) {
        if(client.id == clientId) {
            return participantId;
        }
    }

    return -1;
}

std::map<int, int> ServerGameController::getAllAttachedClients(void) {
    std::map<int, int> participantToClientIndexes;

    for(auto& [participantId, client] : participantToClient) {
        if(client.index == -1) {
            // Ignore any clients which are not 'attached'
            continue;
        }

        participantToClientIndexes[participantId] = client.index;
    }

    return participantToClientIndexes;
}

void ServerGameController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [participantId, participant] : participants) {
        if(participant->getBehaviourStrategy() != nullptr) {
            participant->getBehaviourStrategy()->onUpdate(participantId, timeSinceLastFrame, quit);
        }
    }
}

bool ServerGameController::canProgressToNextTurn(Engagement* engagement) {
    game_assert(transmitter != nullptr);

    auto participant = engagement->getCurrentParticipant();
    auto currentParticipantId = participant->getId();
    auto turnNumber = engagement->getTurnNumber();

    if(!participant->getIsReady()) {
        return false;
    }

    if(participant->getEntities().empty()) {
        transmitter->sendNextTurnToAllClients(currentParticipantId, engagement->getId(), turnNumber);
        return true;
    }

    bool haveEntitiesTurnsFinished = true;
    bool haveEntitiesActionsFinished = true;
    for(auto entity : participant->getEntities()) {
        haveEntitiesTurnsFinished = haveEntitiesTurnsFinished && !entity->isTurnInProgress();
        haveEntitiesActionsFinished = haveEntitiesActionsFinished && !entity->hasAnimationsInProgress() 
            && entity->getActionsChain(turnNumber).empty();
    }

    if(!haveEntitiesActionsFinished) {
        return false;
    }

    if(participant->getBehaviourStrategy() != nullptr && participant->getBehaviourStrategy()->endTurnCondition()) {
        transmitter->sendNextTurnToAllClients(currentParticipantId, engagement->getId(), turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->isPassingNextTurn();

    if(nextTurn) {
        transmitter->sendNextTurnToAllClients(currentParticipantId, engagement->getId(), turnNumber);
    }

    return nextTurn;
}

void ServerGameController::onParticipantTurnEnd(Engagement* engagement) {
    auto participant = engagement->getCurrentParticipant();
    bool canDisengage = true;

    if(engagement->hasHostileParticipants()) {
        for(auto const& other : engagement->getParticipants()) {
            if(other->getId() == participant->getId()) {
                continue;
            }

            // TODO: Determine actual disengagement range properly
            canDisengage = canDisengage && participant->distanceToOtherParticipant(other) > 15;
        }
    }
    else {
        spdlog::trace("No hostile participants left in engagement {}, removing", engagement->getId());
    }

    if(canDisengage) {
        engagementController->flagForRemoval(engagement->getId());
    }
}

void ServerGameController::checkForItems(int participantId) {
    auto itemController = context->getItemController();

    if(!hasParticipant(participantId)) {
        return;
    }

    auto participant = getParticipant(participantId);

    if(!participant->getIsPlayer()) {
        return;
    }

    for(auto entity : participant->getEntities()) {
        auto items = itemController->getItemsAt(entity->getPosition());

        if(items.empty()) {
            return;
        }
        
        if(participant->hasAnyEngagement()) {
            queueAction(
                std::make_unique<TakeItemAction>(
                    participant,
                    entity,
                    participant->getEngagement()->getTurnNumber(),
                    items
                )
            );
        }
        else {
            executeActionImmediately(std::make_unique<TakeItemAction>(participant, entity, items));
        }
    }
}

void ServerGameController::assignEngagements(int participantIdToCheck) {
    for(auto& [participantId, participant] : participants) {
        compareAndEngageParticipants(participant.get(), participants[participantIdToCheck].get());
    }
}

void ServerGameController::compareAndEngageParticipants(Participant* participantA, Participant* participantB) {
    // Same participant - irrelevant
    if(participantA->getId() == participantB->getId()) {
        return;
    }

    // Not hostile - irrelevant
    if(!participantA->isHostile(participantB) || !participantB->isHostile(participantA)) {
        return;
    }

    // Check if an engagment between these participants already exists
    if(participantA->hasEngagement(participantB)) {
        return;
    }

    bool canEngage = false;
    for(auto entityToCheck : participantA->getEntities()) {
        if(hasEntityEngagement(entityToCheck, participantB)) {
            canEngage = true;
            break;
        }
    }

    if(!canEngage) {
        return;
    }

    if(participantA->getEngagement() == nullptr && participantB->getEngagement() == nullptr) {
        if(participantA->getAverageEntitySpeed() > participantB->getAverageEntitySpeed()) {
            engagementController->createEngagement({ participantA, participantB });
        }
        else {
            engagementController->createEngagement({ participantB, participantA });
        }
    }
    else if(participantA->getEngagement() != nullptr && participantB->getEngagement() == nullptr) {
        engagementController->addToEngagement(participantA->getEngagement()->getId(), participantB);
    }
    else if(participantA->getEngagement() == nullptr && participantB->getEngagement() != nullptr) {
        engagementController->addToEngagement(participantB->getEngagement()->getId(), participantA);
    }
    else {
        engagementController->merge(
            participantA->getEngagement()->getId(), 
            participantB->getEngagement()->getId()
        );
    }
}

bool ServerGameController::hasEntityEngagement(Entity* target, Participant* participant) {
    for(auto entity : participant->getEntities()) {
        if(context->getVisibilityController()->isVisible(entity, target)) {
            return true;
        }
    }

    return false;
}

void ServerGameController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}

void ServerGameController::onPublish(const Event<EntitySetPositionEventData>& event) {
    for(auto& [participantId, participant] : participants) {
        assignEngagements(participantId);
    }

    checkForItems(event.data.entity->getParticipantId());
}