#pragma once

#include "core/glmimport.h"
#include "core/net/message/messages.h"
#include "core/net/client/gameclient.h"

class GameClientMessagesTransmitter {
private:
    std::shared_ptr<GameClient> client;

public:
    GameClientMessagesTransmitter(const std::shared_ptr<GameClient>& client);

    void sendFindPathMessage(
        const uint32_t& entityId, 
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void sendSelectEntityMessage(const uint32_t& entityId);
    void sendAttackEntityMessage(
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
    void sendPassParticipantTurnMessage(const int& participantId);
    void sendSetParticipantAckMessage(const int& participantId);
};