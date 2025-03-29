#pragma once

#include <queue>
#include <map>

#include "game/application/gamecontroller.h"

// TODO: Handle turn/participant # desync
class ClientGameController : public GameController {
public:
    ClientGameController();

    void receiveSetNextTurnFlag(int participantId, uint32_t engagementId, int receivedTurnNumber);

private:
    std::map<uint32_t, std::queue<int>> nextTurnFlags;

    bool canProgressToNextTurn(Engagement* engagement) override;
    void onParticipantTurnEnd(Engagement* engagement) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) override;
};