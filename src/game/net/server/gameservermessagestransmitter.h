#pragma once

#include "game/net/gamestateupdate.h"
#include "core/net/server/gameserver.h"
#include "core/net/server/servermessagestransmitter.h"

class GameServerMessagesTransmitter : public ServerMessagesTransmitter {
private:
    std::shared_ptr<GameServer> server;

public:
    GameServerMessagesTransmitter(std::shared_ptr<GameServer> server);

    void sendGameStateUpdate(GameStateUpdate update);
    void onClientConnected(int clientIndex) override;
};
