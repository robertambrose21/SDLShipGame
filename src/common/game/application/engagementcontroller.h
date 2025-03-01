#pragma once

#include <vector>
#include <cstdint>

#include "core/util/vectorutils.h"
#include "core/util/gameassert.h"
#include "game/participant/participant.h"

class EngagementController {
public:
    typedef struct _engagement {
        uint32_t id;
        std::vector<Participant*> participants;
        int turnNumber;
        int currentParticipantIndex;

        bool operator<(const _engagement& other) const {
            return id < other.id;
        }
    } Engagement;

    uint32_t createEngagement(const std::vector<Participant*>& orderedParticipants);
    void removeEngagement(uint32_t engagementId);
    void addToEngagement(uint32_t engagementId, Participant* participant);
    void disengage(uint32_t engagementId, Participant* participant);
    const std::map<uint32_t, Engagement>& getEngagements(void) const;
    Engagement getEngagement(uint32_t id) const;

private:
    std::map<uint32_t, Engagement> engagements;
};