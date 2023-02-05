#include "turncontroller.h"

TurnController::TurnController() :
    turnNumber(0),
    currentParticipant(0)
{ }

void TurnController::update(uint32_t timeSinceLastFrame, bool& quit) {
    if(participants.size() <= 0) {
        return;
    }

    auto bs = participants[currentParticipant]->behaviourStrategy;

    if(bs != nullptr) {
        bs->onUpdate(timeSinceLastFrame, quit);
    }

    bool nextTurn = true;
    int numEntities = 0;

    for(auto entity : participants[currentParticipant]->entities) {
        nextTurn = nextTurn && canProgressToNextTurn(entity);
        numEntities++;
    }

    nextTurn = nextTurn || (bs == nullptr ? false : bs->endTurnCondition());

    if(nextTurn && numEntities > 0) {
        nextParticipantTurn((currentParticipant + 1) % participants.size());
    }
}

std::shared_ptr<TurnController::Participant> TurnController::addParticipant(
    int id,
    bool isPlayer,
    const std::set<std::shared_ptr<Entity>>& entities, 
    const std::shared_ptr<BehaviourStrategy>& behaviourStrategy
) {
    Participant participant;
    participant.id = id;
    participant.entities = entities;
    participant.isPlayer = isPlayer;
    participant.passNextTurn = false;
    participant.actions = { };
    participant.hasRolledForActions = false;
    participant.behaviourStrategy = behaviourStrategy;

    for(auto const& entity : entities) {
        entity->setParticipantId(participant.id);
    }

    auto participantPtr = std::make_shared<Participant>(participant);

    participants[id] = participantPtr;

    return participantPtr;
}

void TurnController::addEntityToParticipant(int participantId, const std::shared_ptr<Entity>& entity) {
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

std::shared_ptr<TurnController::Participant> TurnController::getParticipant(int id) {
    game_assert(participants.contains(id));
    return participants[id];
}

const std::map<int, std::shared_ptr<TurnController::Participant>>& TurnController::getParticipants(void) const {
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

// TODO: This is doing way too much - offload to some helper classes/functions
void TurnController::nextParticipantTurn(int id) {
    if(id == currentParticipant) {
        return;
    }

    participants[currentParticipant]->passNextTurn = false;
    participants[currentParticipant]->hasRolledForActions = false;

    currentParticipant = id;

    if(currentParticipant == 0) {
        turnNumber++;
        std::cout << "Turn number: [" << turnNumber << "]" << std::endl;
    }

    auto& entities = participants[currentParticipant]->entities;

    std::set<std::shared_ptr<Entity>> entitiesForDeletion;

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
    const std::shared_ptr<Entity>& entity, 
    const glm::ivec2& position,
    int shortStopSteps
) {
    if(participants[currentParticipant]->actions[Action::Move] <= 0) {
        return false;
    }

    auto steps = entity->findPath(position, shortStopSteps);

    // Bug - if entity doesn't finish all it's steps then another entity from the same participant
    // can 'steal' the move action
    if(steps >= entity->getMovesLeft()) {
        participants[currentParticipant]->actions[Action::Move]--;
        entity->setMovesLeft(entity->getCurrentStats().movesPerTurn);
    }

    return true;
}

bool TurnController::performAttackAction(
    const std::shared_ptr<Entity>& entity,
    const std::shared_ptr<Weapon>& weapon,
    const std::shared_ptr<Entity>& target
) {
    if(participants[currentParticipant]->actions[Action::Attack] <= 0) {
        return false;
    }

    // Bug - if entity doesn't finish all it's attacks then another entity from the same participant
    // can 'steal' the attack action
    entity->attack(target, weapon);

    if(weapon->getUsesLeft() <= 0) {
        participants[currentParticipant]->actions[Action::Attack]--;
        weapon->reset();
    }

    return true;
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

bool TurnController::canProgressToNextTurn(const std::shared_ptr<Entity>& entity) {
    // if(!participants[currentParticipant]->hasRolledForActions) {
    //     return false;
    // }

    return 
        !entity->isTurnInProgress() || 
        participants[currentParticipant]->passNextTurn;
}