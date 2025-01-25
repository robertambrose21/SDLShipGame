#pragma once

#include <iostream>
#include <queue>

#include "core/net/yojimboimport.h"
#include "core/net/clientgameadapter.h"
#include "clientmessagesreceiver.h"
#include "core/util/gameassert.h"
#include "core/net/messagelogger.h"

class GameClient {
private:
    yojimbo::Client client;
    uint64_t clientId;
    ClientGameAdapter adapter;
    yojimbo::Address address;
    ClientGameConnectionConfig connectionConfig;

    ClientMessagesReceiver& receiver;

    std::queue<yojimbo::Message*> messageQueue;

    std::unique_ptr<MessageLogger> messageLogger;

    void processMessages(void);
    void processMessage(yojimbo::Message* message);

public:
    GameClient(
        std::unique_ptr<MessageLogger> messageLogger,
        uint64_t clientId,
        ClientMessagesReceiver& receiver,
        const yojimbo::Address& serverAddress
    );
    ~GameClient();

    void update(int64_t timeSinceLastFrame);
    yojimbo::Message* createMessage(const GameMessageType& messageType);
    void sendMessage(yojimbo::Message* message);

    uint64_t getClientId(void) const;
};