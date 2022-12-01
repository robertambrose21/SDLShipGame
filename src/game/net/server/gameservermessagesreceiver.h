#pragma once

#include <iostream>

#include "core/glmimport.h"
#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"
#include "core/net/server/servermessagesreceiver.h"
#include "game/application/applicationcontext.h"

class GameServerMessagesReceiver : public ServerMessagesReceiver {
private:
    std::shared_ptr<ApplicationContext> context;

    void receiveFindPathMessage(
        const int& participantId, 
        const uint32_t& entityId, 
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void receiveSelectEntityMessage(const int& participantId, const uint32_t& entityId);
    void receieveAttackEntityMessage(
        const int& participantId, 
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
    void receivePassParticipantTurnMessage(const int& participantId, const int& receivedParticipantId);

public:
    GameServerMessagesReceiver(std::shared_ptr<ApplicationContext> context);

    void receiveMessage(const int& participantId, yojimbo::Message* message) override;
};