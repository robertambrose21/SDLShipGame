#pragma once

#include "game/application/turncontroller.h"
#include "net/gameservermessagestransmitter.h"

class ServerTurnController : public TurnController {
public:
    typedef struct _client {
        uint64_t id;
        int index;
    } Client;

    ServerTurnController();
    
    void setTransmitter(GameServerMessagesTransmitter* transmitter);
    void attachParticipantToClient(int participantId, int clientIndex, uint64_t clientId);
    void detachParticipantFromClient(int clientIndex);

    int getAttachedClient(int participantId) const;
    int getAttachedParticipantId(int clientIndex) const;
    std::map<int, int> getAllAttachedClients(void);

private:
    std::map<int, Client> participantToClient;

    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(int participantId) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit);
    
    void checkForItems(void);
    void assignEngagements(int participantIdToCheck);
    void compareAndEngageParticipants(Participant* participantA, Participant* participantB);
    bool hasEntityEngagement(Entity* target, Participant* participant);
};