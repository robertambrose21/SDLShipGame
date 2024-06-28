#pragma once

#include <queue>

#include "game/application/turncontroller.h"

// TODO: Handle turn/participant # desync
class ClientTurnController : public TurnController {
private:
    std::queue<int> nextTurnFlags;

    bool canProgressToNextTurn(int participantId);
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit);

public:
    ClientTurnController();

    void receiveSetNextTurnFlag(int participantId, int receivedTurnNumber);
};