#pragma once

#include "game/application/turncontroller.h"
#include "net/gameservermessagestransmitter.h"

class ServerTurnController : public TurnController {
private:
    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(int participantId) override;

public:
    ServerTurnController();
    
    void update(int64_t timeSinceLastFrame, bool& quit) override;
    void setTransmitter(GameServerMessagesTransmitter* transmitter);
};