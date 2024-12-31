#pragma once

#include <memory>
#include <iostream>
#include <map>

#include "spdlog/spdlog.h"

#include "core/net/yojimboimport.h"
#include "core/net/gameadapter.h"
#include "servermessagesreceiver.h"
#include "servermessagestransmitter.h"
#include "core/util/gameassert.h"
#include "core/net/messagelogger.h"

class GameServer {
public:
    const int MaxClientConnections = 64;

    GameServer(std::unique_ptr<MessageLogger> messageLogger, const yojimbo::Address& address);

    void setReceiver(ServerMessagesReceiver* receiver);
    void setTransmitter(ServerMessagesTransmitter* transmitter);

    void update(int64_t timeSinceLastFrame);

    void clientConnected(int clientIndex);
    void clientDisconnected(int clientIndex);

    yojimbo::Message* createMessage(int clientIndex, const GameMessageType& messageType);
    void sendMessage(int clientIndex, yojimbo::Message* message);

    yojimbo::Address getAddress(void) const;
    GameAdapter& getAdapter(void);

    bool hasReconnected(int clientIndex);
    uint64_t getClientId(int clientIndex) const;

private:
    enum ConnectionState {
        CONNECTED = 0,
        RECONNECTED,
        DISCONNECTED
    };

    yojimbo::Server server;
    GameAdapter adapter;
    yojimbo::Address address;
    GameConnectionConfig connectionConfig;

    ServerMessagesReceiver* receiver;
    ServerMessagesTransmitter* transmitter;

    std::unique_ptr<MessageLogger> messageLogger;

    std::map<uint64_t, ConnectionState> clientIds;

    void processMessages(void);
    void processMessage(int clientIndex, yojimbo::Message* message);
};