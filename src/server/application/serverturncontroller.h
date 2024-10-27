#pragma once

#include "game/application/turncontroller.h"
#include "net/gameservermessagestransmitter.h"

class ServerTurnController : public TurnController {
private:
    std::map<int, int> participantToClient;

    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(int participantId) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit);
    
    void checkForItems(void);
    void assignEngagements(int participantIdToCheck);
    void compareAndEngageParticipants(Participant* participantA, Participant* participantB);
    bool hasEntityEngagement(Entity* target, Participant* participant);

public:
    ServerTurnController();
    
    void setTransmitter(GameServerMessagesTransmitter* transmitter);
    void attachParticipantToClient(int participantId, int clientIndex);

    int getAttachedClient(int participantId) const;
    int getAttachedParticipantId(int clientIndex) const;
    const std::map<int, int>& getAllAttachedClients(void) const;
};