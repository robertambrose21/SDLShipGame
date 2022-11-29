#include "turncontroller.h"

TurnController::TurnController() :
    turnNumber(0),
    currentParticipant(0)
{ }

void TurnController::update(const uint32_t& timeSinceLastFrame) {
    if(participants.size() <= 0) {
        return;
    }

    bool nextTurn = true;

    for(auto entity : participants[currentParticipant]->entities) {
        nextTurn = nextTurn && canProgressToNextTurn(entity);
    }

    if(nextTurn) {
        nextParticipantTurn();
    }
}

std::shared_ptr<TurnController::Participant> TurnController::addParticipant(
    const int& id,
    std::set<std::shared_ptr<Entity>> entities, 
    bool isPlayer
) {
    Participant participant;
    participant.id = id;
    participant.entities = entities;
    participant.isPlayer = isPlayer;

    for(auto entity : entities) {
        entity->setParticipantId(participant.id);
    }

    auto participantPtr = std::make_shared<Participant>(participant);

    participants[id] = participantPtr;

    return participantPtr;
}

void TurnController::addEntityToParticipant(const int& participantId, std::shared_ptr<Entity> entity) {
    participants[participantId]->entities.insert(entity);
}

std::shared_ptr<TurnController::Participant> TurnController::getParticipant(const int& id) {
    if(participants.contains(id)) {
        return participants[id];
    }

    return nullptr;
}

std::map<int, std::shared_ptr<TurnController::Participant>> TurnController::getParticipants(void) {
    return participants;
}

void TurnController::reset(void) {
    for(auto [participantId, participant] : participants) {
        for(auto entity : participant->entities) {
            if(participantId > 0) {
                entity->reset();
            }
            else {
                entity->nextTurn();
            }
        }
    }
}

void TurnController::nextParticipantTurn(void) {
    currentParticipant = (currentParticipant + 1) % participants.size();

    if(currentParticipant == 0) {
        turnNumber++;
        std::cout << "Turn number: [" << turnNumber << "]" << std::endl;
    }

    auto& entities = participants[currentParticipant]->entities;

    if(entities.size() == 0) {
        nextParticipantTurn();
        return;
    }

    std::set<std::shared_ptr<Entity>> entitiesForDeletion;

    for(auto entity : participants[currentParticipant]->entities) {
        entity->nextTurn();

        if(entity->getCurrentHP() <= 0) {
            entitiesForDeletion.insert(entity);
        }
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
    }
    
    entitiesForDeletion.clear();

    for(auto onNextTurnFunc : onNextTurnWorkers) {
        onNextTurnFunc(currentParticipant, turnNumber);
    }
}

void TurnController::passCurrentParticipant(void) {
    nextParticipantTurn();
}

void TurnController::addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc) {
    onNextTurnWorkers.push_back(onNextTurnFunc);
}

int TurnController::getTurnNumber(void) const {
    return turnNumber;
}

bool TurnController::canProgressToNextTurn(std::shared_ptr<Entity> entity) {
    return !entity->isTurnInProgress() || entity->endTurnCondition();
}