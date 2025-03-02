#include "serverturncontroller.h"

ServerTurnController::ServerTurnController() :
    TurnController()
{
    addOnNextTurnFunction([&](auto const& currentParticipantId, auto const& turnNumber) {
        auto behaviourStrategy = participants[currentParticipantId]->getBehaviourStrategy();

        if(behaviourStrategy != nullptr) {
            behaviourStrategy->onNextTurn();
        }
    });
}

void ServerTurnController::attachParticipantToClient(int participantId, int clientIndex, uint64_t clientId) {
    participantToClient[participantId] = { clientId, clientIndex };
    spdlog::debug("Attached client [{}] to participant [{}]", clientIndex, participantId);
}

void ServerTurnController::detachParticipantFromClient(int clientIndex) {
    auto participantId = getAttachedParticipantId(clientIndex);
    participantToClient[participantId].index = -1; // Set to unused clientIndex

    spdlog::debug("Detached client [{}] from participant [{}]", clientIndex, participantId);
}

int ServerTurnController::getAttachedClient(int participantId) const {
    if(!participantToClient.contains(participantId)) {
        return -1;
    }

    return participantToClient.at(participantId).index;
}

int ServerTurnController::getAttachedParticipantId(int clientIndex) const {
    for(auto [participantId, client] : participantToClient) {
        if(client.index == clientIndex) {
            return participantId;
        }
    }

    return -1;
}

int ServerTurnController::getParticipantByClientId(uint64_t clientId) const {
    for(auto [participantId, client] : participantToClient) {
        if(client.id == clientId) {
            return participantId;
        }
    }

    return -1;
}

std::map<int, int> ServerTurnController::getAllAttachedClients(void) {
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

void ServerTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [participantId, _] : participants) {
        // TODO: This should be called when an entity moves, not every update tick
        assignEngagements(participantId);
    }

    // TODO: update all AI
    // auto participant = participants.at(currentParticipantId).get();

    // if(participant->getBehaviourStrategy() != nullptr) {
    //     participant->getBehaviourStrategy()->onUpdate(participant->getId(), timeSinceLastFrame, quit);
    // }

    // TODO: This should be called when an entity moves, not every update tick
    checkForItems();
}

bool ServerTurnController::canProgressToNextTurn(Engagement* engagement) {
    game_assert(transmitter != nullptr);

    // auto& participant = participants[participantId];
    auto participant = engagement->getCurrentParticipant();
    auto currentParticipantId = participant->getId();
    auto turnNumber = engagement->getTurnNumber();

    if(!participant->getIsReady()) {
        return false;
    }

    if(participant->getEntities().empty()) {
        transmitter->sendNextTurnToAllClients(currentParticipantId, turnNumber);
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
        transmitter->sendNextTurnToAllClients(currentParticipantId, turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->isPassingNextTurn();

    if(nextTurn) {
        transmitter->sendNextTurnToAllClients(currentParticipantId, turnNumber);
    }

    return nextTurn;
}

void ServerTurnController::onParticipantTurnEnd(Engagement* engagement) {
    // auto participant = context->getTurnController()->getParticipant(participantId);
    // game_assert(participant != nullptr);

    // for(auto const& [otherParticipantId, _] : participant->getEngagements()) {
    //     auto otherParticipant = context->getTurnController()->getParticipant(otherParticipantId);

    //     if(otherParticipant == nullptr) {
    //         spdlog::debug(
    //             "Cannot disengage non-existant participant {} from this participant {}", 
    //             participantId, 
    //             otherParticipantId
    //         );
    //         continue;
    //     }

    //     auto distance = participant->distanceToOtherParticipant(otherParticipant);

    //     // TODO: Determine actual disengagement range properly
    //     // TODO: Bug! call disengage outside of the loop, otherwise an rb tree error can occur in the set
    //     if(distance > 15) {
    //         disengage(participantId, otherParticipantId);
    //     }
    // }
}

void ServerTurnController::checkForItems(void) {
    auto itemController = context->getItemController();

    for(auto& [_, participant] : participants) {
        if(!participant->getIsPlayer()) {
            continue;
        }

        for(auto entity : participant->getEntities()) {
            auto items = itemController->getItemsAt(entity->getPosition());
            
            if(!items.empty()) {
                // TODO:
                queueAction(std::make_unique<TakeItemAction>(participant.get(), entity, items));
            }
        }
    }
}

void ServerTurnController::assignEngagements(int participantIdToCheck) {
    // auto behaviour = participants[participantIdToCheck]->getBehaviourStrategy();

    for(auto& [participantId, participant] : participants) {
        compareAndEngageParticipants(participant.get(), participants[participantIdToCheck].get());
    }
}

void ServerTurnController::compareAndEngageParticipants(Participant* participantA, Participant* participantB) {
    // if(participantA->getId() == participantB->getId()) {
    //     return;
    // }

    // // We only need to check the engagements for one of the participants
    // if(participantA->hasEngagement(participantB->getId())) {
    //     if(participantA->getEntities().empty() || participantB->getEntities().empty()) {
    //         disengage(participantA->getId(), participantB->getId());
    //     }

    //     return;
    // }

    // if(!participantA->isHostile(participantB) || !participantB->isHostile(participantA)) {
    //     return;
    // }

    // // Exit early if we find an engagement
    // for(auto entityToCheck : participantA->getEntities()) {
    //     if(hasEntityEngagement(entityToCheck, participantB)) {
    //         engage(participantA->getId(), participantB->getId());
    //         return;
    //     }
    // }

    // Same participant - irrelevant
    if(participantA->getId() == participantB->getId()) {
        return;
    }

    // Not hostile - irrelevant
    if(!participantA->isHostile(participantB) || !participantB->isHostile(participantA)) {
        return;
    }

    // Check if an engagment between these participants already exists
    if(participantA->getEngagement() != nullptr && participantA->hasEngagement(participantB)) {
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
        mergeEngagements(
            engagementController->getEngagement(participantA->getEngagement()->getId()), 
            engagementController->getEngagement(participantB->getEngagement()->getId())
        );
    }

    // std::map<int, uint32_t> currentEngagements;
    // for(auto const& [_, engagement] : getEngagements()) {
    //     for(auto participant : engagement.participants) {
    //         currentEngagements[participant]
    //     }
    //     // if(containsAll(engagement.participants, { participantA->getId(), participantB->getId() })) {
    //     //     return;
    //     // }
    // }
}

void ServerTurnController::mergeEngagements(Engagement* engagementA, Engagement* engagementB) {
    std::vector<Participant*> participantsToMerge;

    if(engagementA->getParticipants().size() == engagementB->getParticipants().size()) {
        float engagementAAverageSpeed = 0.0f;
        float engagementBAverageSpeed = 0.0f;

        for(auto participant : engagementA->getParticipants()) {
            engagementAAverageSpeed += participant->getAverageEntitySpeed();
        }
        for(auto participant : engagementB->getParticipants()) {
            engagementBAverageSpeed += participant->getAverageEntitySpeed();
        }

        // TODO: If the same, consider another heuristic - probably just random and have some kind of announcement on screen
        if(engagementAAverageSpeed >= engagementBAverageSpeed) {
            insertAll(participantsToMerge, engagementA->getParticipants());
            insertAll(participantsToMerge, engagementB->getParticipants());
        }
        else {
            insertAll(participantsToMerge, engagementB->getParticipants());
            insertAll(participantsToMerge, engagementA->getParticipants());
        }
    } else if(engagementA->getParticipants().size() > engagementB->getParticipants().size()) {
        insertAll(participantsToMerge, engagementA->getParticipants());
        insertAll(participantsToMerge, engagementB->getParticipants());
    }
    else {
        insertAll(participantsToMerge, engagementB->getParticipants());
        insertAll(participantsToMerge, engagementA->getParticipants());
    }

    engagementController->removeEngagement(engagementA->getId());
    engagementController->removeEngagement(engagementB->getId());

    engagementController->createEngagement(participantsToMerge);
}

bool ServerTurnController::hasEntityEngagement(Entity* target, Participant* participant) {
    for(auto entity : participant->getEntities()) {
        if(context->getVisibilityController()->isVisible(entity, target)) {
            return true;
        }
    }

    return false;
}

void ServerTurnController::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}