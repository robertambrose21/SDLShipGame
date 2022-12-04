#include "gameserver.h"

GameServer::GameServer(const yojimbo::Address& address) :
    adapter(this),
    address(address),
    server(
        yojimbo::GetDefaultAllocator(), 
        DEFAULT_PRIVATE_KEY, 
        address, 
        connectionConfig = GameConnectionConfig(), 
        adapter, 
        0.0
    )
{
    game_assert(connectionConfig.numChannels == 2);

    server.Start(64);
    if(!server.IsRunning()) {
        throw std::runtime_error("Could not start server at port " + std::to_string(address.GetPort()));
    }

    char buffer[256];
    server.GetAddress().ToString(buffer, sizeof(buffer));
    std::cout << "Server address is " << buffer << std::endl;
}

void GameServer::setReceiver(std::shared_ptr<ServerMessagesReceiver> receiver) {
    game_assert(receiver != nullptr);
    this->receiver = receiver;
}

void GameServer::setTransmitter(std::shared_ptr<ServerMessagesTransmitter> transmitter) {
    game_assert(transmitter != nullptr);
    this->transmitter = transmitter;
}

void GameServer::update(long timeSinceLastFrame) {
    server.AdvanceTime(server.GetTime() + ((double) timeSinceLastFrame) / 1000.0f);
    server.ReceivePackets();

    processMessages();

    // TODO: Send a ping every second or so: https://github.com/networkprotocol/yojimbo/issues/138 and https://github.com/networkprotocol/yojimbo/issues/146
    // Packets are intended to be sent pretty regulary - we can remove this when we're sending packets more regularly
    if(server.HasMessagesToSend(0, (int) GameChannel::RELIABLE)) {
        server.SendPackets();
    }
}

void GameServer::processMessages(void) {
    for(int i = 0; i < MaxClientConnections; i++) {
        if(server.IsClientConnected(i)) {
            for(int j = 0; j < connectionConfig.numChannels; j++) {
                yojimbo::Message* message = server.ReceiveMessage(i, j);
                while(message != NULL) {
                    processMessage(i, message);
                    server.ReleaseMessage(i, message);
                    message = server.ReceiveMessage(i, j);
                }
            }
        }
    }
}

void GameServer::processMessage(int clientIndex, yojimbo::Message* message) {
    receiver->receiveMessage(clientIndex, message);
}

void GameServer::clientConnected(int clientIndex) {
    game_assert(transmitter != nullptr);
    std::cout << "client " << clientIndex << " connected" << std::endl;
    transmitter->onClientConnected(clientIndex);
}

void GameServer::clientDisconnected(int clientIndex) {
    std::cout << "client " << clientIndex << " disconnected" << std::endl;
}

yojimbo::Message* GameServer::createMessage(int clientIndex, GameMessageType messageType) {
    game_assert((int) messageType < (int) GameMessageType::COUNT);
    return server.CreateMessage(clientIndex, (int) messageType);
}

// TODO: Choose the channel
void GameServer::sendMessage(int clientIndex, yojimbo::Message* message) {
    server.SendMessage(clientIndex, (int) GameChannel::RELIABLE, message);
}

yojimbo::Address GameServer::getAddress(void) const {
    return address;
}

GameAdapter& GameServer::getAdapter(void) {
    return adapter;
}