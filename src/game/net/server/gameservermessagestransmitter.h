#pragma once

#include "game/net/gamestateupdate.h"
#include "core/net/server/gameserver.h"

class GameServerMessagesTransmitter {
private:
    std::shared_ptr<GameServer> server;

public:
    GameServerMessagesTransmitter(std::shared_ptr<GameServer> server);

    void sendGameStateUpdate(GameStateUpdate update);
};
