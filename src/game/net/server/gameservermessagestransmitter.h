#pragma once

#include "game/net/messages.h"
#include "core/net/server/gameserver.h"
#include "core/net/server/servermessagestransmitter.h"
#include "game/application/turncontroller.h"
#include "game/application/application.h"

class GameServerMessagesTransmitter : public ServerMessagesTransmitter {
private:
    std::shared_ptr<GameServer> server;

    std::function<void(int)> onClientConnectFunc;

    void onClientConnected(int clientIndex) override;

public:
    GameServerMessagesTransmitter(
        std::shared_ptr<GameServer> server, 
        std::function<void(int)> onClientConnectFunc = [](int) { }
    );

    void sendSetParticipant(int clientIndex, int participantId);
    void sendGameStateUpdate(int clientIndex, GameStateUpdate update);
    void sendLoadMap(int clientIndex, const MapBlock& block);
};
