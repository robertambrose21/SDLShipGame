#pragma once

#include <vector>
#include <cstdint>

#include "core/util/vectorutils.h"
#include "core/util/gameassert.h"
#include "game/participant/participant.h"
#include "game/application/applicationcontext.h"
#include "engagement.h"

class EngagementController {
public:
    EngagementController(ApplicationContext* context);

    void update(int64_t timeSinceLastFrame);

    uint32_t createEngagement(const std::vector<Participant*>& orderedParticipants);
    void removeEngagement(uint32_t engagementId);
    void addToEngagement(uint32_t engagementId, Participant* participant);
    void disengage(uint32_t engagementId, Participant* participant);
    const std::map<uint32_t,  std::unique_ptr<Engagement>>& getEngagements(void) const;
    Engagement* getEngagement(uint32_t id);

private:
    // TODO: The following -> new TurnController
    // --------------------------------------------------------------------------------
    void executeActions(Engagement* engagement);
    void executeEntityActions(Engagement* engagement, Entity* entity);
    void endCurrentParticipantTurn(Engagement* engagement);
    void nextParticipantTurn(Engagement* engagement);
    // --------------------------------------------------------------------------------

    ApplicationContext* context;
    std::map<uint32_t, std::unique_ptr<Engagement>> engagements;
};
