#pragma once

#include <vector>
#include <cstdint>
#include <ranges>

#include "core/util/vectorutils.h"
#include "core/util/gameassert.h"
#include "game/participant/participant.h"
#include "game/application/applicationcontext.h"
#include "core/event/eventpublisher.h"
#include "core/event/event.h"
#include "engagement.h"

class EngagementController : 
    public EventPublisher<
        CreateEngagementEventData,
        AddToEngagementEventData,
        DisengageEventData,
        RemoveEngagementEventData,
        MergeEngagementEventData
    > 
{
public:
    EngagementController(ApplicationContext* context);

    uint32_t createEngagement(const std::vector<Participant*>& orderedParticipants, bool canPublish = true);
    void removeEngagement(uint32_t engagementId, bool canPublish = true);
    void addToEngagement(uint32_t engagementId, Participant* participant, bool canPublish = true);
    void disengage(uint32_t engagementId, Participant* participant, bool canPublish = true);
    uint32_t merge(uint32_t engagementIdA, uint32_t engagementIdB, bool canPublish = true);

    const std::map<uint32_t,  std::unique_ptr<Engagement>>& getEngagements(void) const;
    Engagement* getEngagement(uint32_t id);

private:
    ApplicationContext* context;
    std::map<uint32_t, std::unique_ptr<Engagement>> engagements;
};
