#include "turncontroller.h"

TurnController::TurnController() :
    initialised(false),
    turnNumber(0),
    currentParticipant(0)
{ }

void TurnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void TurnController::update(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    if(participants.size() <= 0) {
        return;
    }

    if(canProgressToNextTurn(currentParticipant)) {
        for(auto const& entity : participants[currentParticipant]->entities) {
            entity->endTurn();
        }
        participants[currentParticipant]->actions.clear();

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
    game_assert(initialised);

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
    game_assert(initialised);
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
    game_assert(initialised);
    game_assert(participants.contains(id));
    return participants[id].get();
}

std::vector<TurnController::Participant*> TurnController::getParticipants(void) {
    game_assert(initialised);

    std::vector<Participant*> vParticipants;
    
    for(auto& [_, participant] : participants) {
        if(participant != nullptr) {
            vParticipants.push_back(participant.get());
        }
    }

    return vParticipants;
}

void TurnController::reset(void) {
    game_assert(initialised);

    for(auto& [participantId, participant] : participants) {
        for(auto entity : participant->entities) {
            entity->nextTurn();
        }
    }
}

// TODO: This is doing way too much - offload to some helper classes/functions
void TurnController::nextParticipantTurn(int id) {
    game_assert(initialised);

    if(id == currentParticipant) {
        return;
    }

    participants[currentParticipant]->passNextTurn = false;
    participants[currentParticipant]->hasRolledForActions = false;

    currentParticipant = id;

    // TODO: Offload to some kind of global turn controller
    if(currentParticipant == 0) {
        turnNumber++;
        context->getGrid()->nextTurn();
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
    game_assert(initialised);
    game_assert(participants.contains(id));
    participants[id]->passNextTurn = true;

    std::cout << "Passing participant " << id << std::endl;
}

void TurnController::setCurrentParticipant(int id) {
    game_assert(initialised);
    game_assert(participants.contains(id));
    nextParticipantTurn(id);
}

int TurnController::getCurrentParticipant(void) const {
    game_assert(initialised);
    return currentParticipant;
}

void TurnController::setActions(int participantId, const std::map<Action, int>& actions) {
    game_assert(initialised);

    if(participants[participantId]->hasRolledForActions) {
        return;
    }

    participants[participantId]->actions = actions;
    participants[participantId]->hasRolledForActions = true;
}

std::map<TurnController::Action, int> TurnController::rollActions(int participantId) {
    game_assert(initialised);

    auto rollNumber = randomD6();
    auto actions = std::map<Action, int>();

    for(int i = 0; i < rollNumber; i++) {
        auto action = randomRange(0, Action::Count - 1);

        actions[(Action) action]++;
    }

    std::cout 
        << "Actions: ["
        << "MOVE/"
        << actions[Action::Move]
        << " ATTACK/"
        << actions[Action::Attack]
        << "]"
        << std::endl;

    setActions(participantId, std::move(actions));

    return participants[participantId]->actions;
}

bool TurnController::performMoveAction(
    Entity* entity, 
    const glm::ivec2& position,
    int shortStopSteps
) {
    game_assert(initialised);

    if(entity == nullptr) {
        return false;
    }

    auto participantId = entity->getParticipantId();

    if(participantId != currentParticipant) {
        return false;
    }

    auto numMoveActions = participants[participantId]->actions[Action::Move];

    if(numMoveActions > 0 && entity->getMovesLeft() <= 0) {
        participants[participantId]->actions[Action::Move]--;
        entity->setMovesLeft(entity->getCurrentStats().movesPerTurn);
    }
    
    if(entity->getMovesLeft() > 0) {
        bool success = entity->findPath(position, shortStopSteps) != 0;

        // if(success) {
        //     std::cout
        //         << "Participant #"
        //         << participantId
        //         << ", "
        //         << entity->getName() 
        //         << "#"
        //         << entity->getId()
        //         << " moving from ("
        //         << entity->getPosition().x << ", " << entity->getPosition().y
        //         << ") to ("
        //         << position.x << ", " << position.y
        //         << ") and now has "
        //         << participants[participantId]->actions[Action::Move]
        //         << "/"
        //         << entity->getMovesLeft()
        //         << " moves/steps left"
        //         << std::endl;
        // }

        return success;
    }

    return false;
}

bool TurnController::performAttackAction(
    Entity* entity,
    Weapon* weapon,
    const glm::ivec2& target
) {
    game_assert(initialised);

    if(weapon == nullptr || entity == nullptr) {
        return false;
    }

    auto participantId = entity->getParticipantId();

    if(participantId != currentParticipant) {
        return false;
    }

    auto numAttackActions = participants[participantId]->actions[Action::Attack];

    if(numAttackActions > 0 && weapon->getUsesLeft() <= 0) {
        participants[participantId]->actions[Action::Attack]--;
        weapon->reset();
    }

    int usesLeft = weapon->getUsesLeft();
    entity->attack(target, weapon->getId());

    // if(usesLeft != weapon->getUsesLeft()) {
    //     std::cout
    //         << "Participant #"
    //         << participantId
    //         << ", "
    //         << entity->getName() 
    //         << "#"
    //         << entity->getId()
    //         << " attacked target ("
    //         << target.x
    //         << ", "
    //         << target.y
    //         << ") with a "
    //         << weapon->getName()
    //         << " and now has "
    //         << participants[participantId]->actions[Action::Attack]
    //         << "/"
    //         << weapon->getUsesLeft()
    //         << " attack/weapon uses left"
    //         << std::endl;
    // }

    return usesLeft > 0;
}

void TurnController::addOnNextTurnFunction(std::function<void(int, int)> onNextTurnFunc) {
    game_assert(initialised);
    onNextTurnWorkers.push_back(onNextTurnFunc);
}

void TurnController::setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet) {
    game_assert(initialised);
    this->onAllParticipantsSet = onAllParticipantsSet;
}

void TurnController::allParticipantsSet(void) {
    game_assert(initialised);
    onAllParticipantsSet();
    for(auto& [_, participant] : participants) {
        participant->isReady = true;
    }
}

int TurnController::getTurnNumber(void) const {
    game_assert(initialised);
    return turnNumber;
}