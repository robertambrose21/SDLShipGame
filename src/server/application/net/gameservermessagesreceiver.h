#pragma once

#include <iostream>
#include <map>
#include <set>

#include "core/glmimport.h"
#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"
#include "core/net/servermessagesreceiver.h"
#include "game/application/applicationcontext.h"
#include "gameservermessagestransmitter.h"

// TODO: Ensure participants are validated
class GameServerMessagesReceiver : public ServerMessagesReceiver {
private:
    ApplicationContext& context;
    GameServerMessagesTransmitter* transmitter;

    std::map<int, std::set<int>> clientParticipantsLoaded;

    void receiveFindPathMessage(
        int clientIndex, 
        uint32_t entityId, 
        const glm::ivec2& position,
        int shortStopSteps
    );
    void receiveSelectEntityMessage(int clientIndex, uint32_t entityId);
    void receieveAttackMessage(
        int clientIndex, 
        uint32_t entityId, 
        int x,
        int y,
        uint32_t weaponId
    );
    void receivePassParticipantTurnMessage(int clientIndex, int participantId);
    void receiveSetParticipantAckMessage(int clientIndex, int participantId);

public:
    GameServerMessagesReceiver(ApplicationContext& context);

    void setTransmitter(GameServerMessagesTransmitter* transmitter);

    void receiveMessage(int clientIndex, yojimbo::Message* message) override;
    bool areParticipantsLoadedForClient(int clientIndex);
};