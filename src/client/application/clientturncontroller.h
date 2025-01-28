#pragma once

#include <queue>

#include "game/application/turncontroller.h"

// TODO: Handle turn/participant # desync
class ClientTurnController : public TurnController {
public:
    ClientTurnController();

    void receiveSetNextTurnFlag(int participantId, int receivedTurnNumber);

private:
    std::queue<int> nextTurnFlags;

    bool canProgressToNextTurn(int participantId) override;
    void onParticipantTurnEnd(int participantId) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) override;
};