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
    std::erase_if(participantToClient, [&](const auto& item) {
        auto const& [participantId, client] = item;

        if(client.index == clientIndex) {
            spdlog::debug("Detached client [{}] from participant [{}]", clientIndex, participantId);
            return true;
        }

        return false;
    });
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

std::map<int, int> ServerTurnController::getAllAttachedClients(void) {
    std::map<int, int> participantToClientIndexes;

    for(auto& [participantId, client] : participantToClient) {
        participantToClientIndexes[participantId] = client.index;
    }

    return participantToClientIndexes;
}

void ServerTurnController::additionalUpdate(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [participantId, _] : participants) {
        // TODO: This should be called on end of turn, not every update tick
        assignEngagements(participantId);
    }

    auto participant = participants.at(currentParticipantId).get();

    if(participant->getBehaviourStrategy() != nullptr) {
        participant->getBehaviourStrategy()->onUpdate(participant->getId(), timeSinceLastFrame, quit);
    }

    checkForItems();
}

bool ServerTurnController::canProgressToNextTurn(int participantId) {
    game_assert(transmitter != nullptr);

    auto& participant = participants[participantId];

    if(!participant->getIsReady()) {
        return false;
    }

    // TODO: Need to add a check to see if any fully dead participants still have projectiles/animations in progress
    if(participant->getEntities().empty()) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
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
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
        return true;
    }

    auto nextTurn = haveEntitiesTurnsFinished || participant->isPassingNextTurn();

    if(nextTurn) {
        transmitter->sendNextTurn(0, currentParticipantId, turnNumber);
    }

    return nextTurn;
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
                queueAction(std::make_unique<TakeItemAction>(turnNumber, entity, items));
            }
        }
    }
}

void ServerTurnController::assignEngagements(int participantIdToCheck) {
    auto behaviour = participants[participantIdToCheck]->getBehaviourStrategy();

    for(auto& [participantId, participant] : participants) {
        compareAndEngageParticipants(participant.get(), participants[participantIdToCheck].get());
    }
}

void ServerTurnController::compareAndEngageParticipants(Participant* participantA, Participant* participantB) {
    if(participantA->getId() == participantB->getId()) {
        return;
    }

    // We only need to check the engagements for one of the participants
    if(participantA->getEngagements().contains(participantB->getId())) {
        if(participantA->getEntities().empty() || participantB->getEntities().empty()) {
            disengage(participantA->getId(), participantB->getId());
        }

        return;
    }

    // Exit early if we find an engagement
    for(auto entityToCheck : participantA->getEntities()) {
        if(hasEntityEngagement(entityToCheck, participantB)) {
            engage(participantA->getId(), participantB->getId());
            return;
        }
    }
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