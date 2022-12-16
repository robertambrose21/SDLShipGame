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

    std::function<void(const int&)> onClientConnectFunc;

    void onClientConnected(const int& clientIndex) override;

public:
    GameServerMessagesTransmitter(
        std::shared_ptr<GameServer> server, 
        std::function<void(const int&)> onClientConnectFunc = [](const int&) { }
    );

    void sendSetParticipant(const int& clientIndex, const std::shared_ptr<TurnController::Participant>& participant);
    void sendGameStateUpdate(const int& clientIndex, const GameStateUpdate& update);
    void sendLoadMap(const int& clientIndex, const MapBlock& block);
    void sendFindPath(
        const int& clientIndex, 
        const uint32_t& entityId, 
        const glm::ivec2& position,
        const int& shortStopSteps
    );
    void sendAttackEntity(
        const int& clientIndex,
        const uint32_t& entityId, 
        const uint32_t& targetId, 
        const uint32_t& weaponId
    );
};
