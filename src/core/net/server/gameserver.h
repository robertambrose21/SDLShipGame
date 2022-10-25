#pragma once

#include <memory>
#include <iostream>

#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"

class GameServer {
private:
    yojimbo::Server server;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    void processMessages(void);
    void processMessage(int clientIndex, yojimbo::Message* message);

public:
    GameServer(const yojimbo::Address& address);

    void update(long timeSinceLastFrame);

    void clientConnected(int clientIndex);
    void clientDisconnected(int clientIndex);

    yojimbo::Address getAddress(void) const;
    GameAdapter& getAdapter(void);
};