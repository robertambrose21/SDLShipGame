#pragma once

#include "game/application/turncontroller.h"

// TODO: Handle turn/participant # desync
class ClientTurnController : public TurnController {
private:
    bool receivedValidNextTurnFlag;

    bool canProgressToNextTurn(int participantId);

public:
    ClientTurnController();

    void receiveSetNextTurnFlag(int participantId, int turnNumber);
};