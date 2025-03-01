#pragma once

#include <vector>
#include <cstdint>

#include "core/util/vectorutils.h"
#include "core/util/gameassert.h"
#include "game/participant/participant.h"
#include "game/application/applicationcontext.h"

class EngagementController {
public:
    // TODO: Make this a class
    typedef struct _engagement {
        uint32_t id;
        std::vector<Participant*> participants;
        int currentParticipantIndex;
        int turnNumber;

        bool operator<(const _engagement& other) const {
            return id < other.id;
        }

        Participant* getCurrentParticipant(void) const {
            return participants[currentParticipantIndex];
        }

        void nextTurn(void) {
            currentParticipantIndex = (currentParticipantIndex + 1) % participants.size();
            getCurrentParticipant()->nextTurn();

            // for(auto const& onNextTurnFunc : onNextTurnWorkers) {
            //     onNextTurnFunc(currentParticipantId, turnNumber);
            // }
            // context->getEffectController()->onNextTurn();

            turnNumber++;
            // context->getGrid()->nextTurn();
            // publish<TurnEventData>({ turnNumber, currentParticipantId });
        }

    } Engagement;

    EngagementController(ApplicationContext* context);

    void update(int64_t timeSinceLastFrame);

    uint32_t createEngagement(const std::vector<Participant*>& orderedParticipants);
    void removeEngagement(uint32_t engagementId);
    void addToEngagement(uint32_t engagementId, Participant* participant);
    void disengage(uint32_t engagementId, Participant* participant);
    const std::map<uint32_t, Engagement>& getEngagements(void) const;
    Engagement getEngagement(uint32_t id) const;

private:
    // TODO: The following -> new TurnController
    // --------------------------------------------------------------------------------
    void executeActions(const Engagement& engagement);
    void executeEntityActions(const Engagement& engagement, Entity* entity);
    void endCurrentParticipantTurn(const Engagement& engagement);
    void nextParticipantTurn(Engagement& engagement);
    // --------------------------------------------------------------------------------

    ApplicationContext* context;
    std::map<uint32_t, Engagement> engagements;
};
