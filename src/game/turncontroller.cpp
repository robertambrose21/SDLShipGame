#include "turncontroller.h"

TurnController::TurnController() :
    turnNumber(0),
    currentParticipant(0)
{ }

void TurnController::update(Uint32 timeSinceLastFrame) {
    if(participants.size() <= 0) {
        return;
    }

    bool nextTurn = true;

    for(auto entity : participants[currentParticipant].entities) {
        nextTurn = nextTurn && canProgressToNextTurn(entity);
    }

    if(nextTurn) {
        nextParticipantTurn();
    }
}

void TurnController::addParticipant(std::set<std::shared_ptr<Entity>> entities, bool isPlayer) {
    Participant participant;
    participant.order = participants.size();
    participant.entities = entities;
    participant.isPlayer = isPlayer;

    participants.push_back(participant);
}

void TurnController::reset(void) {
    for(auto participant : participants) {
        for(auto entity : participant.entities) {
            if(participant.order > 0) {
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

    auto& entities = participants[currentParticipant].entities;

    if(entities.size() == 0) {
        nextParticipantTurn();
        return;
    }

    std::set<std::shared_ptr<Entity>> entitiesForDeletion;

    for(auto entity : participants[currentParticipant].entities) {
        entity->nextTurn();

        if(entity->getCurrentHP() <= 0) {
            entitiesForDeletion.insert(entity);
        }
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
    }
    
    entitiesForDeletion.clear();
}

void TurnController::passCurrentParticipant(void) {
    nextParticipantTurn();
}

int TurnController::getTurnNumber(void) const {
    return turnNumber;
}

bool TurnController::canProgressToNextTurn(std::shared_ptr<Entity> entity) {
    return !entity->isTurnInProgress() || entity->endTurnCondition();
}