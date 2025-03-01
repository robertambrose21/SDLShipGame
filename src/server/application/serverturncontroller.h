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
    int getParticipantByClientId(uint64_t clientId) const;
    std::map<int, int> getAllAttachedClients(void);

private:
    std::map<int, Client> participantToClient;

    GameServerMessagesTransmitter* transmitter;

    bool canProgressToNextTurn(Engagement* engagement) override;
    void onParticipantTurnEnd(Engagement* engagement) override;
    void additionalUpdate(int64_t timeSinceLastFrame, bool& quit) override;
    
    void checkForItems(void);
    void assignEngagements(int participantIdToCheck);
    void compareAndEngageParticipants(Participant* participantA, Participant* participantB);
    void mergeEngagements(Engagement* engagementA, Engagement* engagementB);
    bool hasEntityEngagement(Entity* target, Participant* participant);
};