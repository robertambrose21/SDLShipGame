#pragma once

#include <memory>
#include <iostream>

#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"
#include "servermessagesreceiver.h"

class GameServer {
private:
    yojimbo::Server server;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    std::shared_ptr<ServerMessagesReceiver> receiver;

    void processMessages(void);
    void processMessage(int clientIndex, yojimbo::Message* message);

public:
    GameServer(std::shared_ptr<ServerMessagesReceiver> receiver, const yojimbo::Address& address);

    void update(long timeSinceLastFrame);

    void clientConnected(int clientIndex);
    void clientDisconnected(int clientIndex);

    yojimbo::Address getAddress(void) const;
    GameAdapter& getAdapter(void);
};