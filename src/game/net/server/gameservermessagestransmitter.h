#pragma once

#include "game/net/messages.h"
#include "core/net/server/gameserver.h"
#include "core/net/server/servermessagestransmitter.h"
#include "game/application/turncontroller.h"
#include "game/application/application.h"

class GameServerMessagesTransmitter : public ServerMessagesTransmitter {
private:
    std::shared_ptr<GameServer> server;
    std::shared_ptr<TurnController> turnController;

    std::function<void(int)> onClientConnectFunc;

    void onClientConnected(int clientIndex) override;

public:
    GameServerMessagesTransmitter(
        std::shared_ptr<GameServer> server, 
        std::function<void(int)> onClientConnectFunc = [](int) { }
    );

    void sendSetParticipant(int clientIndex, std::shared_ptr<TurnController::Participant> participant);
    void sendGameStateUpdate(int clientIndex, GameStateUpdate update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendFindPath(
        int clientIndex, 
        const uint32_t& entityId, 
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void sendAttackEntity(
        int clientIndex,
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
};
