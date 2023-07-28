#include "turncontroller.h"

TurnController::TurnController(Grid& grid) :
    grid(grid),
    turnNumber(0),
    currentParticipant(0)
{ }

void TurnController::update(uint32_t timeSinceLastFrame, bool& quit) {
    if(participants.size() <= 0) {
        return;
    }

    auto& bs = participants[currentParticipant]->behaviourStrategy;

    if(bs != nullptr) {
        bs->onUpdate(timeSinceLastFrame, quit);
    }

    if(canProgressToNextTurn(currentParticipant)) {
        nextParticipantTurn((currentParticipant + 1) % participants.size());
    }
}

TurnController::Participant* TurnController::addParticipant(
    int id,
    bool isPlayer,
    const std::vector<Entity*>& entities, 
    std::unique_ptr<BehaviourStrategy> behaviourStrategy,
    bool isReady
) {
    Participant participant;
    participant.id = id;
    participant.isReady = isReady;
    participant.entities = entities;
    participant.isPlayer = isPlayer;
    participant.passNextTurn = false;
    participant.actions = { };
    participant.hasRolledForActions = false;
    participant.behaviourStrategy = std::move(behaviourStrategy);

    for(auto const& entity : entities) {
        entity->setParticipantId(participant.id);
    }

    participants[id] = std::make_unique<Participant>(std::move(participant));

    return participants[id].get();
}

void TurnController::addEntityToParticipant(int participantId, Entity* entity) {
    game_assert(entity != nullptr);

    if(!participants.contains(participantId)) {
        throw std::runtime_error(
            "Could not add entity to participant with id " + 
            std::to_string(participantId) + 
            " participant does not exist"
        );
    }

    participants[participantId]->entities.push_back(entity);
}

TurnController::Participant* TurnController::getParticipant(int id) {
    game_assert(participants.contains(id));
    return participants[id].get();
}

std::vector<TurnController::Participant*> TurnController::getParticipants(void) {
    std::vector<Participant*> vParticipants;
    
    for(auto& [_, participant] : participants) {
        if(participant != nullptr) {
            vParticipants.push_back(participant.get());
        }
    }

    return vParticipants;
}

void TurnController::reset(void) {
    for(auto& [participantId, participant] : participants) {
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

// TODO: This is doing way too much - offload to some helper classes/functions
void TurnController::nextParticipantTurn(int id) {
    if(id == currentParticipant) {
        return;
    }

    participants[currentParticipant]->passNextTurn = false;
    participants[currentParticipant]->hasRolledForActions = false;

    currentParticipant = id;

    // TODO: Offload to some kind of global turn controller
    if(currentParticipant == 0) {
        turnNumber++;
        grid.nextTurn();
        publish({ turnNumber, currentParticipant });
    }

    auto& entities = participants[currentParticipant]->entities;

    std::set<Entity*> entitiesForDeletion;

    for(auto const& entity : participants[currentParticipant]->entities) {
        entity->nextTurn();

        if(entity->getCurrentHP() <= 0) {
            entitiesForDeletion.insert(entity);
        }
    }

    for(auto const& entity : entitiesForDeletion) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
    }
    
    entitiesForDeletion.clear();

    if(participants[currentParticipant]->behaviourStrategy != nullptr) {
        participants[currentParticipant]->behaviourStrategy->onNextTurn();
    }

    for(auto const& onNextTurnFunc : onNextTurnWorkers) {
        onNextTurnFunc(currentParticipant, turnNumber);
    }
}

void TurnController::passParticipant(int id) {
    game_assert(participants.contains(id));
    participants[id]->passNextTurn = true;
}

void TurnController::setCurrentParticipant(int id) {
    game_assert(participants.contains(id));
    nextParticipantTurn(id);
}

int TurnController::getCurrentParticipant(void) const {
    return currentParticipant;
}

void TurnController::setActions(int participantId, const std::map<Action, int>& actions) {
    if(participants[participantId]->hasRolledForActions) {
        return;
    }

    participants[participantId]->actions = actions;
    participants[participantId]->hasRolledForActions = true;
}

std::map<TurnController::Action, int> TurnController::rollActions(int participantId) {
    auto rollNumber = randomD6();
    auto actions = std::map<Action, int>();

    for(int i = 0; i < rollNumber; i++) {
        auto action = randomRange(0, Action::Count - 1);

        actions[(Action) action]++;
    }

    setActions(participantId, std::move(actions));

    return participants[participantId]->actions;
}

bool TurnController::performMoveAction(
    Entity* entity, 
    const glm::ivec2& position,
    int shortStopSteps
) {
    auto numMoveActions = participants[currentParticipant]->actions[Action::Move];

    if(entity == nullptr) {
        return false;
    }

    if(numMoveActions > 0 && entity->getMovesLeft() <= 0) {
        participants[currentParticipant]->actions[Action::Move]--;
        entity->setMovesLeft(entity->getCurrentStats().movesPerTurn);
    }
    
    if(entity->getMovesLeft() > 0) {
        entity->findPath(position, shortStopSteps);
        return true;
    }

    return false;
}

bool TurnController::performAttackAction(
    Entity* entity,
    Weapon* weapon,
    const glm::ivec2& target
) {
    auto numAttackActions = participants[currentParticipant]->actions[Action::Attack];

    if(weapon == nullptr || entity == nullptr) {
        return false;
    }

    if(numAttackActions > 0 && weapon->getUsesLeft() <= 0) {
        participants[currentParticipant]->actions[Action::Attack]--;
        weapon->reset();
    }

    int usesLeft = weapon->getUsesLeft();
    entity->attack(target, weapon->getId());

    return usesLeft > 0;
}

void TurnController::addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc) {
    onNextTurnWorkers.push_back(onNextTurnFunc);
}

void TurnController::setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet) {
    this->onAllParticipantsSet = onAllParticipantsSet;
}

void TurnController::allParticipantsSet(void) {
    onAllParticipantsSet();
    for(auto& [_, participant] : participants) {
        participant->isReady = true;
    }
}

int TurnController::getTurnNumber(void) const {
    return turnNumber;
}

bool TurnController::canProgressToNextTurn(int participantId) {
    auto& participant = participants[participantId];

    if(!participant->isReady) {
        return false;
    }

    if(participant->entities.empty()) {
        return false;
    }

    if(participant->behaviourStrategy != nullptr && participant->behaviourStrategy->endTurnCondition()) {
        return true;
    }

    bool haveEntitiesTurnsFinished = true;
    for(auto entity : participant->entities) {
        haveEntitiesTurnsFinished = haveEntitiesTurnsFinished && !entity->isTurnInProgress();
    }

    if(!haveEntitiesTurnsFinished) {
        return false;
    }

    bool haveActionsCompleted = participant->hasRolledForActions;

    for(auto [_, numActionsLeft] : participant->actions) {
        haveActionsCompleted = haveActionsCompleted && numActionsLeft == 0;
    }

    return haveActionsCompleted || participant->passNextTurn;
}