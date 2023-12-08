#pragma once

#include "game/application/turncontroller.h"
#include "net/gameservermessagestransmitter.h"

class ServerTurnController : public TurnController {
private:
    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(int participantId) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit);

public:
    ServerTurnController();
    
    void setTransmitter(GameServerMessagesTransmitter* transmitter);
};