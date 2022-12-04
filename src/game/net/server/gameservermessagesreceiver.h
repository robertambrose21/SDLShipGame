#pragma once

#include <iostream>
#include <map>
#include <set>

#include "core/glmimport.h"
#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"
#include "core/net/server/servermessagesreceiver.h"
#include "game/application/applicationcontext.h"

class GameServerMessagesReceiver : public ServerMessagesReceiver {
private:
    std::shared_ptr<ApplicationContext> context;
    std::map<int, std::set<int>> clientParticipantsLoaded;

    void receiveFindPathMessage(
        const int& clientIndex, 
        const uint32_t& entityId, 
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void receiveSelectEntityMessage(const int& clientIndex, const uint32_t& entityId);
    void receieveAttackEntityMessage(
        const int& clientIndex, 
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
    void receivePassParticipantTurnMessage(const int& clientIndex, const int& participantId);
    void receiveSetParticipantAckMessage(const int& clientIndex, const int& participantId);

public:
    GameServerMessagesReceiver(std::shared_ptr<ApplicationContext> context);

    void receiveMessage(const int& clientIndex, yojimbo::Message* message) override;
    bool areParticipantsLoadedForClient(int clientIndex);
};