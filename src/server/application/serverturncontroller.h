#pragma once

#include "game/application/turncontroller.h"
#include "net/gameservermessagestransmitter.h"

class ServerTurnController : public TurnController {
private:
    std::map<int, int> participantToClient;

    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(int participantId) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit);

public:
    ServerTurnController();
    
    void setTransmitter(GameServerMessagesTransmitter* transmitter);
    void attachParticipantToClient(int participantId, int clientIndex);

    int getAttachedClient(int participantId) const;
    int getAttachedParticipantId(int clientIndex) const;
    const std::map<int, int>& getAllAttachedClients(void) const;
};