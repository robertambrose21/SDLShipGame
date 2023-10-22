#pragma once

#include "game/net/messages.h"
#include "core/net/gameserver.h"
#include "core/net/servermessagestransmitter.h"
#include "game/application/turncontroller.h"
#include "game/application/application.h"

class GameServerMessagesTransmitter : public ServerMessagesTransmitter {
private:
    GameServer& server;
    TurnController* turnController;

    std::function<void(int)> onClientConnectFunc;

    void onClientConnected(int clientIndex) override;

public:
    GameServerMessagesTransmitter(
        GameServer& server,
        TurnController* turnController,
        std::function<void(int)> onClientConnectFunc = [](int) { }
    );

    void sendSetParticipant(int clientIndex, TurnController::Participant* participant);
    void sendGameStateUpdate(int clientIndex, const GameStateUpdate& update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
    void sendFindPath(
        int clientIndex, 
        uint32_t entityId, 
        const glm::ivec2& position,
        int shortStopSteps
    );
    void sendAttack(
        int clientIndex,
        uint32_t entityId, 
        const glm::ivec2& target,
        uint32_t weaponId
    );
    void sendActionsRollResponse(int clientIndex, int participantId, const std::vector<DiceActionResult>& dice);
    void sendNextTurn(int clientIndex, int participantId, int turnNumber);
};
