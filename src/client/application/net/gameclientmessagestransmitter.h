#pragma once

#include "core/glmimport.h"
#include "game/message/messages.h"
#include "core/net/gameclient.h"

class GameClientMessagesTransmitter {
private:
    GameClient& client;

public:
    GameClientMessagesTransmitter(GameClient& client);

    void sendActionsRollMessage(int participantId);
    void sendFindPathMessage(
        uint32_t actorId, 
        const glm::ivec2& position,
        int shortStopSteps,
        int turnNumber = -1
    );
    void sendSelectActorMessage(uint32_t actorId);
    void sendAttackMessage(
        uint32_t actorId, 
        const glm::ivec2& target, 
        UUID weaponId,
        int turnNumber = -1
    );
    void sendPassParticipantTurnMessage(int participantId);
    void sendSetParticipantAckMessage(int participantId);
    void sendEquipItemMessage(uint32_t itemId, uint32_t actorId, Equippable<Stats::GearStats>::Slot slot, bool isUnequip);
    void sendEquipWeaponMessage(uint32_t itemId, uint32_t actorId, UUID weaponId, bool isUnequip);
};