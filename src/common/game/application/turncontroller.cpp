#include "turncontroller.h"

// This will effectively become a game controller
TurnController::TurnController() :
    initialised(false),
    turnNumber(0),
    currentParticipantId(0)
{ }

void TurnController::initialise(ApplicationContext& context) {
    this->context = &context;
    engagementController = std::make_unique<EngagementController>(context);
    initialised = true;
}

void TurnController::update(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    // // We need at least two participants to start a game
    // if(participants.size() < 2) {
    //     return;
    // }

    // processEngagements();
    // additionalUpdate(timeSinceLastFrame, quit);
    // executeActions(currentParticipantId);

    // for(auto const& [participantId, participant] : participants) {
    //     if(participantId != currentParticipantId && !participant->getEngagementId().has_value()) {
    //         executeActions(participantId);
    //     }
    // }

    // if(canProgressToNextTurn(currentParticipantId)) {
    //     endCurrentParticipantTurn();
    //     nextParticipantTurn();
    // }
    
    // for(auto const& [_, engagement] : engagements) {
    //     // execute actions
    //     // check if we can progress to next turn
    //     // Go to next turn if we can
    // }

    engagementController->update(timeSinceLastFrame);

    // Do free roam stuff
}

void TurnController::processEngagements() {
    // if (engagementsQueue[turnNumber].empty()) {
    //     return; 
    // }

    // for (auto& engagement : engagementsQueue[turnNumber]) {
    //     if (engagement.isDisengage) {
    //         disengage(engagement.participantIdA, engagement.participantIdB);
    //     } else {
    //         engage(engagement.participantIdA, engagement.participantIdB);
    //     }
    // }

    // engagementsQueue[turnNumber].clear();
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
        spdlog::error(
            "Could not add entity {} to participant with id {} participant does not exist", 
            entity->toString(),
            participantId
        );
        return;
    }

    participants[participantId]->addEntity(entity);

    // if(!participants[participantId]->getEngagements().empty()) {
    //     entity->engage();
    // }
}

Participant* TurnController::getParticipant(int id) {
    game_assert(initialised);
    game_assert(participants.contains(id));
    return participants[id].get();
}

// TODO: Replace me with less dumb
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

bool TurnController::hasParticipant(int id) {
    return participants.contains(id);
}

void TurnController::reset(void) {
    game_assert(initialised);

    for(auto& [participantId, participant] : participants) {
        for(auto entity : participant->getEntities()) {
            entity->nextTurn();
        }
    }
}

// void TurnController::engage(int participantIdA, int participantIdB) {
//     auto& participantA = participants[participantIdA];
//     auto& participantB = participants[participantIdB];

//     if(participantA == nullptr) {
//         spdlog::critical("Cannot engage participant with id {} participant does not exist", participantIdA);
//         return;
//     }
//     if(participantB == nullptr) {
//         spdlog::critical("Cannot engage participant with id {} participant does not exist", participantIdB);
//         return;
//     }
//     if(!participantA->isHostile(participantB.get())) {
//         spdlog::warn("Cannot engage participant {}, not hostile to participant {}", participantIdA, participantIdB);
//         return;
//     }
//     if(!participantB->isHostile(participantA.get())) {
//         spdlog::warn("Cannot engage participant {}, not hostile to participant {}", participantIdB, participantIdA);
//         return;
//     }

//     participantA->engage(participantIdB, turnNumber);
//     participantB->engage(participantIdA, turnNumber);

//     publish<EngagementEventData>({ participantIdA, participantIdB, EngagementType::ENGAGED });
// }

// void TurnController::queueEngagement(int turnNumber, const Engagement& engagement) {
//     engagementsQueue[turnNumber].push_back(engagement);
// }

// void TurnController::disengage(int participantIdA, int participantIdB) {
//     auto& participantA = participants[participantIdA];
//     auto& participantB = participants[participantIdB];

//     participantA->disengage(participantIdB);
//     participantB->disengage(participantIdA);

//     publish<EngagementEventData>({ participantIdA, participantIdB, EngagementType::DISENGAGED });
// }

void TurnController::endCurrentParticipantTurn(void) {
    auto& participant = participants[currentParticipantId];
    participant->endTurn();
    onParticipantTurnEnd(currentParticipantId);
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
    currentParticipantId = id;
}

int TurnController::getCurrentParticipantId(void) const {
    game_assert(initialised);
    return currentParticipantId;
}

void TurnController::executeActions(int participantId) {
    game_assert(initialised);

    for(auto entity : participants[participantId]->getEntities()) {
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
    bool skipValidation = turnNumber != action->getTurnNumber(); // TODO: Should this be <= ?

    if(skipValidation) {
        spdlog::trace(
            "[{}, {}] Skipping validation, action is for turn {}",
            turnNumber,
            action->typeToString(),
            action->getTurnNumber()
        );
    }

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
            auto equipItemAction = dynamic_cast<EquipGearAction&>(action);
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

void TurnController::setTurnNumber(int turnNumber) {
    this->turnNumber = turnNumber;
}