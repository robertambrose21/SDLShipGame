#pragma once

#include <iostream>
#include <map>
#include <set>

#include "core/glmimport.h"
#include "core/net/yojimboimport.h"
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
        int shortStopSteps,
        int turnNumber
    );
    void receiveSelectEntityMessage(int clientIndex, uint32_t entityId);
    void receieveAttackMessage(
        int clientIndex, 
        uint32_t entityId, 
        int x,
        int y,
        uint8_t weaponIdBytes[16]
    );
    void receivePassParticipantTurnMessage(int clientIndex, int participantId);
    void receiveSetParticipantAckMessage(int clientIndex, int participantId);
    void receiveEquipItemMessage(int clientIndex, uint32_t itemId, uint32_t entityId, uint8_t slot, bool isUnequip);
    void receiveEquipWeaponMessage(
        int clientIndex, 
        uint32_t itemId, 
        uint32_t entityId, 
        uint8_t weaponIdBytes[16], 
        bool isUnequip
    );

public:
    GameServerMessagesReceiver(ApplicationContext& context);

    void setTransmitter(GameServerMessagesTransmitter* transmitter);

    void receiveMessage(int clientIndex, yojimbo::Message* message) override;
    bool areParticipantsLoadedForClient(int clientIndex);
};