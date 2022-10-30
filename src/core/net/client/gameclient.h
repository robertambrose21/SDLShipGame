#pragma once

#include <iostream>
#include <queue>

#include "yojimbo/yojimbo.h"
#include "core/net/gameadapter.h"

class GameClient {
private:
    yojimbo::Client client;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    std::queue<yojimbo::Message*> messageQueue;

    void processMessages(void);
    void processMessage(yojimbo::Message* message);

public:
    GameClient(const yojimbo::Address& serverAddress);

    void update(long timeSinceLastFrame);
    yojimbo::Message* createMessage(GameMessageType messageType);
    void sendMessage(yojimbo::Message* message);
};