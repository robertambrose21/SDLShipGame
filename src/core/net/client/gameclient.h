#pragma once

#include <iostream>
#include <queue>

#include "core/net/yojimboimport.h"
#include "core/net/gameadapter.h"
#include "clientmessagesreceiver.h"
#include "core/util/gameassert.h"

class GameClient {
private:
    yojimbo::Client client;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    std::shared_ptr<ClientMessagesReceiver> receiver;

    std::queue<yojimbo::Message*> messageQueue;

    void processMessages(void);
    void processMessage(yojimbo::Message* message);

public:
    GameClient(
        const std::shared_ptr<ClientMessagesReceiver>& receiver,
        const yojimbo::Address& serverAddress
    );

    void update(uint32_t timeSinceLastFrame);
    yojimbo::Message* createMessage(const GameMessageType& messageType);
    void sendMessage(yojimbo::Message* message);
};