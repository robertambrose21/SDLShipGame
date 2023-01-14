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

    void sendSetParticipant(int clientIndex, const std::shared_ptr<TurnController::Participant>& participant);
    void sendGameStateUpdate(int clientIndex, const GameStateUpdate& update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendFindPath(
        int clientIndex, 
        uint32_t entityId, 
        const glm::ivec2& position,
        int shortStopSteps
    );
    void sendAttackEntity(
        int clientIndex,
        uint32_t entityId, 
        uint32_t targetId, 
        uint32_t weaponId
    );
    void sendActionsRollResponse(int clientIndex, int participantId, int rollNumber, int actions[6]);
};
