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
        nextParticipantTurn((currentParticipant + 1) % participants.size());
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
    participant.passNextTurn = false;

    for(auto entity : entities) {
        entity->setParticipantId(participant.id);
    }

    auto participantPtr = std::make_shared<Participant>(participant);

    participants[id] = participantPtr;

    return participantPtr;
}

void TurnController::addEntityToParticipant(const int& participantId, std::shared_ptr<Entity> entity) {
    game_assert(entity != nullptr);

    if(!participants.contains(participantId)) {
        throw std::runtime_error(
            "Could not add entity to participant with id " + 
            std::to_string(participantId) + 
            " participant does not exist"
        );
    }

    participants[participantId]->entities.insert(entity);
}

std::shared_ptr<TurnController::Participant> TurnController::getParticipant(const int& id) {
    game_assert(participants.contains(id));
    return participants[id];
}

std::map<int, std::shared_ptr<TurnController::Participant>> TurnController::getParticipants(void) {
    return participants;
}

void TurnController::reset(void) {
    for(auto [participantId, participant] : participants) {
        for(auto entity : participant->entities) {
            // Why is this different for participant 0???
            if(participantId > 0) {
                entity->reset();
            }
            else {
                entity->nextTurn();
            }
        }
    }
}

void TurnController::nextParticipantTurn(const int& id) {
    if(id == currentParticipant) {
        return;
    }

    participants[currentParticipant]->passNextTurn = false;

    currentParticipant = id;

    if(currentParticipant == 0) {
        turnNumber++;
        std::cout << "Turn number: [" << turnNumber << "]" << std::endl;
    }

    auto& entities = participants[currentParticipant]->entities;

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

void TurnController::passParticipant(const int& id) {
    game_assert(participants.contains(id));
    participants[id]->passNextTurn = true;
}

void TurnController::setCurrentParticipant(const int& id) {
    game_assert(participants.contains(id));
    nextParticipantTurn(id);
}

int TurnController::getCurrentParticipant(void) const {
    return currentParticipant;
}

void TurnController::addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc) {
    onNextTurnWorkers.push_back(onNextTurnFunc);
}

void TurnController::setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet) {
    this->onAllParticipantsSet = onAllParticipantsSet;
}

void TurnController::allParticipantsSet(void) {
    onAllParticipantsSet();
}

int TurnController::getTurnNumber(void) const {
    return turnNumber;
}

bool TurnController::canProgressToNextTurn(std::shared_ptr<Entity> entity) {
    return 
        !entity->isTurnInProgress() || 
        entity->endTurnCondition() || 
        participants[currentParticipant]->passNextTurn;
}