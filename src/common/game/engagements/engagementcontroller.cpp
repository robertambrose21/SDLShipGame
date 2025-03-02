#include "engagementcontroller.h"

EngagementController::EngagementController(ApplicationContext* context) :
    context(context)
{ }

uint32_t EngagementController::createEngagement(
    const std::vector<Participant*>& orderedParticipants,
    bool canPublish
) {
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

    if(canPublish) {
        std::vector<int> participantIds;
        for(auto participant : orderedParticipants) {
            participantIds.push_back(participant->getId());
        }

        publish<CreateEngagementEventData>({ engagement->getId(), participantIds });
    }

    return engagement->getId();
}

void EngagementController::removeEngagement(uint32_t engagementId, bool canPublish) {
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

    if(canPublish) {
        publish<RemoveEngagementEventData>({ engagementId });
    }
}

void EngagementController::addToEngagement(uint32_t engagementId, Participant* participant, bool canPublish) {
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

    if(canPublish) {
        publish<AddToEngagementEventData>({ engagementId, participant->getId() });
    }
}

void EngagementController::disengage(uint32_t engagementId, Participant* participant, bool canPublish) {
    if(!engagements.contains(engagementId)) {
        spdlog::warn("Cannot disengage participant from non-existant engagement with id {}", engagementId);
        return;
    }

    spdlog::trace("Removing participant {} from engagement {}", participant->getId(), engagementId);
    engagements[engagementId]->removeParticipant(participant->getId());

    participant->setEngagement(nullptr);

    if(canPublish) {
        publish<DisengageEventData>({ engagementId, participant->getId() });
    }

    if(engagements[engagementId]->getParticipants().empty()) {
        removeEngagement(engagementId);
    }
}

uint32_t EngagementController::merge(uint32_t engagementIdA, uint32_t engagementIdB, bool canPublish) {
    std::vector<Participant*> participantsToMerge;
    auto engagementA = getEngagement(engagementIdA);
    auto engagementB = getEngagement(engagementIdB);

    if(engagementA->getParticipants().size() == engagementB->getParticipants().size()) {
        float engagementAAverageSpeed = 0.0f;
        float engagementBAverageSpeed = 0.0f;

        for(auto participant : engagementA->getParticipants()) {
            engagementAAverageSpeed += participant->getAverageEntitySpeed();
        }
        for(auto participant : engagementB->getParticipants()) {
            engagementBAverageSpeed += participant->getAverageEntitySpeed();
        }

        // TODO: If the same, consider another heuristic - probably just random and have some kind of announcement on screen
        if(engagementAAverageSpeed >= engagementBAverageSpeed) {
            insertAll(participantsToMerge, engagementA->getParticipants());
            insertAll(participantsToMerge, engagementB->getParticipants());
        }
        else {
            insertAll(participantsToMerge, engagementB->getParticipants());
            insertAll(participantsToMerge, engagementA->getParticipants());
        }
    } else if(engagementA->getParticipants().size() > engagementB->getParticipants().size()) {
        insertAll(participantsToMerge, engagementA->getParticipants());
        insertAll(participantsToMerge, engagementB->getParticipants());
    }
    else {
        insertAll(participantsToMerge, engagementB->getParticipants());
        insertAll(participantsToMerge, engagementA->getParticipants());
    }

    removeEngagement(engagementA->getId(), false);
    removeEngagement(engagementB->getId(), false);

    auto newEngagementId = createEngagement(participantsToMerge, false);

    if(canPublish) {
        std::vector<int> participantIds;
        for(auto participant : participantsToMerge) {
            participantIds.push_back(participant->getId());
        }

        publish<MergeEngagementEventData>({ newEngagementId, engagementIdA, engagementIdB, participantIds });
    }

    return newEngagementId;
}

const std::map<uint32_t, std::unique_ptr<Engagement>>& EngagementController::getEngagements(void) const {
    return engagements;
}

Engagement* EngagementController::getEngagement(uint32_t id) {
    return engagements.at(id).get();
}