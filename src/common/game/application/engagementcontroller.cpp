#include "engagementcontroller.h"

EngagementController::EngagementController(ApplicationContext* context) :
    context(context)
{ }

void EngagementController::update(int64_t timeSinceLastFrame) {
    for(auto& [_, engagement] : engagements) {
        // execute actions
        // check if we can progress to next turn
        // Go to next turn if we can

        executeActions(engagement);

        if(true /* Can progress to next turn*/) {
            endCurrentParticipantTurn(engagement);
            nextParticipantTurn(engagement);
        }
    }
}

void EngagementController::endCurrentParticipantTurn(const Engagement& engagement) {
    engagement.getCurrentParticipant()->endTurn();
    // onParticipantTurnEnd(currentParticipantId);
}

void EngagementController::nextParticipantTurn(Engagement& engagement) {
    engagement.nextTurn();
}

void EngagementController::executeActions(const Engagement& engagement) {
    for(auto entity : engagement.getCurrentParticipant()->getEntities()) {
        executeEntityActions(engagement, entity);
    }
}

void EngagementController::executeEntityActions(const Engagement& engagement, Entity* entity) {
    bool moreActionsToProcess = !entity->getActionsChain(engagement.turnNumber).empty();

    while(moreActionsToProcess) {
        auto action = entity->getActionsChain(engagement.turnNumber).front();

        if(action->isFinished()) {
            entity->popAction(engagement.turnNumber);
            moreActionsToProcess = !entity->getActionsChain(engagement.turnNumber).empty();
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

uint32_t EngagementController::createEngagement(const std::vector<Participant*>& orderedParticipants) {
    game_assert(!orderedParticipants.empty());

    auto id = getNewId();

    spdlog::trace(
        "Adding engagement {} with {} participants", 
        id, 
        engagements[id].participants.size()
    );
    engagements[id] = { id, orderedParticipants, 0, 0 };
    return id;
}

void EngagementController::removeEngagement(uint32_t engagementId) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot remove non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace(
        "Removing engagement {} with {} participants", 
        engagementId, 
        engagements[engagementId].participants.size()
    );

    for(auto participant : engagements[engagementId].participants) {
        participant->setEngagementId(std::nullopt);
    }

    engagements.erase(engagementId);
}

void EngagementController::addToEngagement(uint32_t engagementId, Participant* participant) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot add participant to non-existant engagement with id {}", engagementId);
        return;
    }

    if(contains(engagements[engagementId].participants, participant)) {
        spdlog::warn(
            "Cannot add participant {} to engagement {}, participant already part of this engagement",
            participant->getId(),
            engagementId
        );
        return;
    }
    spdlog::trace("Adding participant {} to engagement {}", participant->getId(), engagementId);
    engagements[engagementId].participants.push_back(participant);
}

void EngagementController::disengage(uint32_t engagementId, Participant* participant) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot disengage participant from non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace("Removing participant {} from engagement {}", participant->getId(), engagementId);
    std::erase_if(engagements[engagementId].participants, [&](const auto& item) {
        return participant == item;
    });

    participant->setEngagementId(std::nullopt);

    if(engagements[engagementId].participants.empty()) {
        removeEngagement(engagementId);
    }
}

const std::map<uint32_t, EngagementController::Engagement>& EngagementController::getEngagements(void) const {
    return engagements;
}

EngagementController::Engagement EngagementController::getEngagement(uint32_t id) const {
    return engagements.at(id);
}