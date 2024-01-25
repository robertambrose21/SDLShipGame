#pragma once

#include "core/glmimport.h"
#include "core/net/message/messages.h"
#include "core/net/gameclient.h"

class GameClientMessagesTransmitter {
private:
    GameClient& client;

public:
    GameClientMessagesTransmitter(GameClient& client);

    void sendActionsRollMessage(int participantId);
    void sendFindPathMessage(
        uint32_t entityId, 
        const glm::ivec2& position,
        int shortStopSteps
    );
    void sendSelectEntityMessage(uint32_t entityId);
    void sendAttackMessage(
        uint32_t entityId, 
        const glm::ivec2& target, 
        uint32_t weaponId
    );
    void sendPassParticipantTurnMessage(int participantId);
    void sendSetParticipantAckMessage(int participantId);
    void sendEquipItemMessage(uint32_t itemId, uint32_t entityId, Equipment::Slot slot);
};