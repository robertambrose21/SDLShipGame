#pragma once

#include <iostream>
#include <map>
#include <set>

#include "core/glmimport.h"
#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"
#include "core/net/server/servermessagesreceiver.h"
#include "game/application/applicationcontext.h"
#include "game/net/server/gameservermessagestransmitter.h"

// TODO: Ensure participants are validated
class GameServerMessagesReceiver : public ServerMessagesReceiver {
private:
    std::shared_ptr<ApplicationContext> context;
    std::shared_ptr<GameServerMessagesTransmitter> transmitter;

    std::map<int, std::set<int>> clientParticipantsLoaded;

    void receiveFindPathMessage(
        int clientIndex, 
        uint32_t entityId, 
        const glm::ivec2& position,
        int shortStopSteps
    );
    void receiveSelectEntityMessage(int clientIndex, uint32_t entityId);
    void receieveAttackEntityMessage(
        int clientIndex, 
        uint32_t entityId, 
        uint32_t targetId, 
        uint32_t weaponId
    );
    void receivePassParticipantTurnMessage(int clientIndex, int participantId);
    void receiveSetParticipantAckMessage(int clientIndex, int participantId);
    void receiveActionsRollMessage(int clientIndex, int participantId);

public:
    GameServerMessagesReceiver(const std::shared_ptr<ApplicationContext>& context);

    void setTransmitter(const std::shared_ptr<GameServerMessagesTransmitter>& transmitter);

    void receiveMessage(int clientIndex, yojimbo::Message* message) override;
    bool areParticipantsLoadedForClient(int clientIndex);
};