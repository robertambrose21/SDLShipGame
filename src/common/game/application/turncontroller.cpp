#include "turncontroller.h"

TurnController::TurnController() :
    initialised(false),
    turnNumber(0),
    currentParticipantId(0)
{ }

void TurnController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void TurnController::update(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    // We need at least two participants to start a game
    if(participants.size() < 2) {
        return;
    }

    processEngagements();
    additionalUpdate(timeSinceLastFrame, quit);
    executeActions(currentParticipantId);

    if(canProgressToNextTurn(currentParticipantId)) {
        endCurrentParticipantTurn();
        nextParticipantTurn();
    }
}

void TurnController::processEngagements() {
    if (engagementsQueue[turnNumber].empty()) {
        return; 
    }

    for (auto& engagement : engagementsQueue[turnNumber]) {
        if (engagement.isDisengage) {
            disengage(engagement.participantIdA, engagement.participantIdB);
        } else {
            engage(engagement.participantIdA, engagement.participantIdB);
        }
    }

    engagementsQueue[turnNumber].clear();
}

Participant* TurnController::addParticipant(
    bool isPlayer,
    const std::vector<Entity*>& entities, 
    std::unique_ptr<BehaviourStrategy> behaviourStrategy,
    bool isReady
) {
    game_assert(initialised);

    static int id = 0;

    return addParticipant(id++, isPlayer, entities, std::move(behaviourStrategy), isReady);
}

Participant* TurnController::addParticipant(
    int id,
    bool isPlayer,
    const std::vector<Entity*>& entities, 
    std::unique_ptr<BehaviourStrategy> behaviourStrategy,
    bool isReady
) {
    game_assert(initialised);

    Participant participant(id);
    participant.setIsReady(isReady);
    participant.setIsPlayer(isPlayer);
    participant.setBehaviourStrategy(std::move(behaviourStrategy));
    participant.addEntities(entities);

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

    participants[participantId]->addEntity(entity);

    if(!participants[participantId]->getEngagements().empty()) {
        entity->engage();
    }
}

Participant* TurnController::getParticipant(int id) {
    game_assert(initialised);
    game_assert(participants.contains(id));
    return participants[id].get();
}

std::vector<Participant*> TurnController::getParticipants(void) {
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
        for(auto entity : participant->getEntities()) {
            entity->nextTurn();
        }
    }
}

void TurnController::engage(int participantIdA, int participantIdB) {
    auto& participantA = participants[participantIdA];
    auto& participantB = participants[participantIdB];

    participantA->engage(participantIdB);
    participantB->engage(participantIdA);

    publish<EngagementEventData>({ participantIdA, participantIdB, EngagementEventData::Type::ENGAGED });
}

void TurnController::queueEngagement(int turnNumber, const Engagement& engagement) {
    engagementsQueue[turnNumber].push_back(engagement);
}

void TurnController::disengage(int participantIdA, int participantIdB) {
    auto& participantA = participants[participantIdA];
    auto& participantB = participants[participantIdB];

    participantA->disengage(participantIdB);
    participantB->disengage(participantIdA);

    publish<EngagementEventData>({ participantIdA, participantIdB, EngagementEventData::Type::DISENGAGED });
}

void TurnController::endCurrentParticipantTurn(void) {
    auto& participant = participants[currentParticipantId];
    participant->endTurn();
}

void TurnController::nextParticipantTurn(void) {
    game_assert(initialised);

    currentParticipantId = (currentParticipantId + 1) % participants.size();

    participants[currentParticipantId]->nextTurn();

    for(auto const& onNextTurnFunc : onNextTurnWorkers) {
        onNextTurnFunc(currentParticipantId, turnNumber);
    }

    incrementTurn();

    context->getEffectController()->onNextTurn();
}

void TurnController::incrementTurn(void) {
    turnNumber++;
    context->getGrid()->nextTurn();
    publish<TurnEventData>({ turnNumber, currentParticipantId });
}

void TurnController::passParticipant(int id) {
    game_assert(participants.contains(id));
    participants[id]->passTurn();
}

void TurnController::setCurrentParticipant(int id) {
    throw std::runtime_error("not implemented");
}

int TurnController::getCurrentParticipant(void) const {
    game_assert(initialised);
    return currentParticipantId;
}

void TurnController::executeActions(int participantId) {
    game_assert(initialised);

    for(auto entity : participants[currentParticipantId]->getEntities()) {
        executeEntityActions(entity);
    }
}

void TurnController::executeEntityActions(Entity* entity) {
    bool moreActionsToProcess = !entity->getActionsChain(turnNumber).empty();

    while(moreActionsToProcess) {
        auto action = entity->getActionsChain(turnNumber).front();

        if(action->isFinished()) {
            entity->popAction(turnNumber);
            moreActionsToProcess = !entity->getActionsChain(turnNumber).empty();
        }
        // TODO: If precondition fails - just drop?
        else if(action->passesPrecondition() && !action->isExecuted()) {
            action->execute(context);
            moreActionsToProcess = false;
        }
        else {
            moreActionsToProcess = false;
        }
    }
}

bool TurnController::queueAction(std::unique_ptr<Action> action) {
    bool skipValidation = turnNumber != action->getTurnNumber();

    return action->getEntity()->queueAction(
        context,
        std::move(action),
        [&](auto& action) { publishAction(action); },
        skipValidation
    );
}

// TODO: Gross
void TurnController::publishAction(Action& action) {
    switch(action.getType()) {
        case Action::Move: {
            auto moveAction = dynamic_cast<MoveAction&>(action);
            publish<MoveActionEventData>({ 
                turnNumber, 
                moveAction.getEntity(), 
                moveAction.getPosition(), 
                moveAction.getShortStopSteps() 
            });
            break;
        }

        case Action::Attack: {
            auto attackAction = dynamic_cast<AttackAction&>(action);
            publish<AttackActionEventData>({
                turnNumber,
                attackAction.getEntity(),
                attackAction.getTarget(),
                attackAction.getWeapon()
            });
            break;
        }

        case Action::TakeItem: {
            auto takeItemAction = dynamic_cast<TakeItemAction&>(action);
            publish<TakeItemActionEventData>({
                turnNumber,
                takeItemAction.getEntity(),
                takeItemAction.getItems()
            });
            break;
        }

        case Action::EquipItem: {
            auto equipItemAction = dynamic_cast<EquipItemAction&>(action);
            publish<EquipItemActionEventData>({
                turnNumber,
                equipItemAction.getEntity(),
                equipItemAction.getItem(),
                equipItemAction.getSlot(),
                equipItemAction.getIsUnequip()
            });
            break;
        }

        case Action::EquipWeaponItem: {
            auto equipWeaponAction = dynamic_cast<EquipWeaponAction&>(action);
            publish<EquipWeaponActionEventData>({
                turnNumber,
                equipWeaponAction.getEntity(),
                equipWeaponAction.getItem(),
                equipWeaponAction.getWeaponId()
            });
            break;
        }

        default:
            break;
    }
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
        participant->setIsReady(true);
    }
}

int TurnController::getTurnNumber(void) const {
    return turnNumber;
}