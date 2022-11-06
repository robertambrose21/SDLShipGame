#pragma once

#include "core/net/server/gameserver.h"
#include "game/net/server/gameservermessagesreceiver.h"
#include "application.h"

class ServerApplication {
private:
    std::shared_ptr<GameServer> server;
    std::shared_ptr<GameServerMessagesReceiver> receiver;

public:
    ServerApplication();
    ~ServerApplication();

    void initialise(void);
    void run(void);
};