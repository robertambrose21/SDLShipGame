#pragma once

#include <memory>
#include <iostream>

#include "core/net/yojimboimport.h"
#include "core/net/gameadapter.h"
#include "servermessagesreceiver.h"
#include "servermessagestransmitter.h"
#include "core/util/gameassert.h"

class GameServer {
private:
    yojimbo::Server server;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    std::shared_ptr<ServerMessagesReceiver> receiver;
    std::shared_ptr<ServerMessagesTransmitter> transmitter;

    void processMessages(void);
    void processMessage(int clientIndex, yojimbo::Message* message);

public:
    const int MaxClientConnections = 64;

    GameServer(const yojimbo::Address& address);

    void setReceiver(const std::shared_ptr<ServerMessagesReceiver>& receiver);
    void setTransmitter(const std::shared_ptr<ServerMessagesTransmitter>& transmitter);

    void update(uint32_t timeSinceLastFrame);

    void clientConnected(int clientIndex);
    void clientDisconnected(int clientIndex);

    yojimbo::Message* createMessage(int clientIndex, const GameMessageType& messageType);
    void sendMessage(int clientIndex, yojimbo::Message* message);

    yojimbo::Address getAddress(void) const;
    GameAdapter& getAdapter(void);
};