#include "gamecontroller.h"

GameController::GameController() :
    initialised(false)
{ }

void GameController::initialise(ApplicationContext& context) {
    this->context = &context;
    engagementController = std::make_unique<EngagementController>(&context);
    factionController = std::make_unique<FactionController>();
    initialised = true;
}

void GameController::update(int64_t timeSinceLastFrame, bool& quit) {
    game_assert(initialised);

    additionalUpdate(timeSinceLastFrame, quit);
    
    for(auto& [engagementId, engagement] : engagementController->getEngagements()) {
        executeActions(engagementId);

        if(canProgressToNextTurn(engagement.get())) {
            endCurrentParticipantTurn(engagementId);
            nextParticipantTurn(engagementId);
        }
    }

    engagementController->update(timeSinceLastFrame);
}

void GameController::endCurrentParticipantTurn(uint32_t engagementId) {
    if(!engagementController->hasEngagement(engagementId)) {
        spdlog::warn("Cannot find engagement {}, cannot end turn for current participant", engagementId);
        return;
    }

    auto engagement = engagementController->getEngagement(engagementId);
    engagement->getCurrentParticipant()->endTurn();
    onParticipantTurnEnd(engagement);
}

void GameController::nextParticipantTurn(uint32_t engagementId) {
    if(!engagementController->hasEngagement(engagementId)) {
        spdlog::trace("Engagement {} no longer exists, not executing nextParticipantTurn", engagementId);
        return;
    }

    auto engagement = engagementController->getEngagement(engagementId);

    if(engagement->getIsFinished()) {
        spdlog::trace("Engagement {} is finished, not executing nextParticipantTurn", engagementId);
        return;
    }

    engagement->nextTurn();
}

void GameController::executeActions(uint32_t engagementId) {
    if(!engagementController->hasEngagement(engagementId)) {
        spdlog::warn("Engagement {} no longer exists, cannot execute entity actions", engagementId);
        return;
    }

    auto engagement = engagementController->getEngagement(engagementId);

    if(engagement->getIsFinished()) {
        spdlog::trace("Engagement {} is finished, not executing executeActions", engagementId);
        return;
    }

    for(auto entity : engagement->getCurrentParticipant()->getEntities()) {
        executeEntityActions(engagement, entity);
    }
}

void GameController::executeEntityActions(Engagement* engagement, Entity* entity) {
    bool moreActionsToProcess = !entity->getActionsChain(engagement->getTurnNumber()).empty();

    while(moreActionsToProcess) {
        auto action = entity->getActionsChain(engagement->getTurnNumber()).front();

        if(action->isFinished()) {
            entity->popAction(engagement->getTurnNumber());
            moreActionsToProcess = !entity->getActionsChain(engagement->getTurnNumber()).empty();
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

Participant* GameController::addParticipant(
    bool isPlayer,
    const std::vector<Entity*>& entities, 
    std::unique_ptr<BehaviourStrategy> behaviourStrategy,
    bool isReady
) {
    game_assert(initialised);

    static int id = 0;

    return addParticipant(id++, isPlayer, entities, std::move(behaviourStrategy), isReady);
}

Participant* GameController::addParticipant(
    int id,
    bool isPlayer,
    const std::vector<Entity*>& entities, 
    std::unique_ptr<BehaviourStrategy> behaviourStrategy,
    bool isReady
) {
    game_assert(initialised);

    Participant participant(id, factionController->getUnalignedFactionId());
    participant.setIsReady(isReady);
    participant.setIsPlayer(isPlayer);
    participant.setBehaviourStrategy(std::move(behaviourStrategy));
    participant.addEntities(entities);

    participants[id] = std::make_unique<Participant>(std::move(participant));

    return participants[id].get();
}

void GameController::addEntityToParticipant(int participantId, Entity* entity) {
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
}

Participant* GameController::getParticipant(int id) {
    game_assert(initialised);
    game_assert(participants.contains(id));
    return participants[id].get();
}

// TODO: Replace me with less dumb
std::vector<Participant*> GameController::getParticipants(void) {
    game_assert(initialised);

    std::vector<Participant*> vParticipants;
    
    for(auto& [_, participant] : participants) {
        if(participant != nullptr) {
            vParticipants.push_back(participant.get());
        }
    }

    return vParticipants;
}

bool GameController::hasParticipant(int id) {
    return participants.contains(id);
}

void GameController::reset(void) {
    game_assert(initialised);

    for(auto& [participantId, participant] : participants) {
        for(auto entity : participant->getEntities()) {
            entity->nextTurn();
        }
    }
}

bool GameController::executeActionImmediately(std::unique_ptr<Action> action) {
    if(action->getTurnNumber().has_value()) {
        spdlog::error(
            "Execute [{}]: Cannot execute immediately - turn number set {}",
            action->typeToString(),
            action->getTurnNumber().value()
        );
        return false;
    }

    if(action->getParticipant()->getEngagement() != nullptr) {
        spdlog::error(
            "Execute [{}]: Cannot execute immediately - participant {} has engagement {}",
            action->typeToString(),
            action->getParticipant()->getId(),
            action->getParticipant()->getEngagement()->getId()
        );
        return false;
    }

    if(action->isExecuted()) {
        spdlog::warn("Execute [{}]: Cannot execute immediately - action already executed", action->typeToString());
        return false;
    }

    if(!action->passesPrecondition()) {
        spdlog::warn("Execute [{}]: Cannot execute immediately - action fails precondition", action->typeToString());
        return false;
    }
    
    action->execute(context);
    publishAction(action.get());

    return true;
}

void GameController::publishAction(Action* action) {
    auto publishData = action->getPublishData();

    std::visit([&](auto&& eventData) {
        publish(eventData);
    }, publishData);
}

bool GameController::queueAction(std::unique_ptr<Action> action) {
    if(!action->getTurnNumber().has_value()) {
        spdlog::error("Queue [{}]: Cannot queue action - no turn number is set", action->typeToString());
        return false;
    }

    auto engagement = action->getParticipant()->getEngagement();
    bool skipValidation = engagement != nullptr && engagement->getTurnNumber() != action->getTurnNumber().value();

    if(skipValidation) {
        spdlog::trace(
            "[{}, {}] Skipping validation, action is for turn {}",
            engagement->getTurnNumber(),
            action->typeToString(),
            action->getTurnNumber().value()
        );
    }

    return action->getEntity()->queueAction(
        context,
        std::move(action),
        [&](auto& action) { publishAction(&action); },
        skipValidation
    );
}

void GameController::setOnAllParticipantsSetFunction(std::function<void()> onAllParticipantsSet) {
    this->onAllParticipantsSet = onAllParticipantsSet;
}

void GameController::allParticipantsSet(void) {
    onAllParticipantsSet();
    for(auto& [_, participant] : participants) {
        participant->setIsReady(true);
    }
}

EngagementController* GameController::getEngagementController(void) {
    return engagementController.get();
}

FactionController* GameController::getFactionController(void) {
    return factionController.get();
}