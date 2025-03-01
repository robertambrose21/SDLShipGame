#include "engagementcontroller.h"

EngagementController::EngagementController(ApplicationContext* context) :
    context(context)
{ }

void EngagementController::update(int64_t timeSinceLastFrame) {
    for(auto& [_, engagement] : engagements) {
        // execute actions
        // check if we can progress to next turn
        // Go to next turn if we can

        executeActions(engagement.get());

        if(true /* Can progress to next turn*/) {
            endCurrentParticipantTurn(engagement.get());
            nextParticipantTurn(engagement.get());
        }
    }
}

void EngagementController::endCurrentParticipantTurn(Engagement* engagement) {
    engagement->getCurrentParticipant()->endTurn();
    // onParticipantTurnEnd(currentParticipantId);
}

void EngagementController::nextParticipantTurn(Engagement* engagement) {
    engagement->nextTurn();
}

void EngagementController::executeActions(Engagement* engagement) {
    for(auto entity : engagement->getCurrentParticipant()->getEntities()) {
        executeEntityActions(engagement, entity);
    }
}

void EngagementController::executeEntityActions(Engagement* engagement, Entity* entity) {
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

uint32_t EngagementController::createEngagement(const std::vector<Participant*>& orderedParticipants) {
    game_assert(!orderedParticipants.empty());

    auto engagement = std::make_unique<Engagement>(orderedParticipants);

    spdlog::trace(
        "Adding engagement {} with {} participants", 
        engagement->getId(), 
        orderedParticipants.size()
    );
    engagements[engagement->getId()] = std::move(engagement);

    return engagement->getId();
}

void EngagementController::removeEngagement(uint32_t engagementId) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot remove non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace(
        "Removing engagement {} with {} participants", 
        engagementId, 
        engagements[engagementId]->getParticipants().size()
    );

    for(auto participant : engagements[engagementId]->getParticipants()) {
        participant->setEngagementId(std::nullopt);
    }

    engagements.erase(engagementId);
}

void EngagementController::addToEngagement(uint32_t engagementId, Participant* participant) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot add participant to non-existant engagement with id {}", engagementId);
        return;
    }

    if(contains(engagements[engagementId]->getParticipants(), participant)) {
        spdlog::warn(
            "Cannot add participant {} to engagement {}, participant already part of this engagement",
            participant->getId(),
            engagementId
        );
        return;
    }
    spdlog::trace("Adding participant {} to engagement {}", participant->getId(), engagementId);
    engagements[engagementId]->addParticipant(participant);
}

void EngagementController::disengage(uint32_t engagementId, Participant* participant) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot disengage participant from non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace("Removing participant {} from engagement {}", participant->getId(), engagementId);
    engagements[engagementId]->removeParticipant(participant->getId());

    participant->setEngagementId(std::nullopt);

    if(engagements[engagementId]->getParticipants().empty()) {
        removeEngagement(engagementId);
    }
}

const std::map<uint32_t, std::unique_ptr<Engagement>>& EngagementController::getEngagements(void) const {
    return engagements;
}

Engagement* EngagementController::getEngagement(uint32_t id) {
    return engagements.at(id).get();
}