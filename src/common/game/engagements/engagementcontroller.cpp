#include "engagementcontroller.h"

EngagementController::EngagementController(ApplicationContext* context) :
    context(context)
{ }

// TODO: Set engagements on participant (check remove too)
uint32_t EngagementController::createEngagement(const std::vector<Participant*>& orderedParticipants) {
    game_assert(!orderedParticipants.empty());

    auto engagement = std::make_unique<Engagement>(orderedParticipants);

    spdlog::trace(
        "Adding engagement {} with {} participants", 
        engagement->getId(), 
        orderedParticipants.size()
    );
    engagements[engagement->getId()] = std::move(engagement);

    for(auto participant : orderedParticipants) {
        participant->setEngagement(engagement.get());
    }

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
        participant->setEngagement(nullptr);
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
    participant->setEngagement(engagements[engagementId].get());
}

void EngagementController::disengage(uint32_t engagementId, Participant* participant) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot disengage participant from non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace("Removing participant {} from engagement {}", participant->getId(), engagementId);
    engagements[engagementId]->removeParticipant(participant->getId());

    participant->setEngagement(nullptr);

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