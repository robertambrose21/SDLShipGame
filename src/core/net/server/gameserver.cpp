#include "gameserver.h"

GameServer::GameServer(std::shared_ptr<ServerMessagesReceiver> receiver, const yojimbo::Address& address) :
    receiver(receiver),
    adapter(this),
    address(address),
    server(yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY, address, connectionConfig = GameConnectionConfig(), adapter, 0.0)
{
    server.Start(64);
    if(!server.IsRunning()) {
        throw std::runtime_error("Could not start server at port " + std::to_string(address.GetPort()));
    }

    char buffer[256];
    server.GetAddress().ToString(buffer, sizeof(buffer));
    std::cout << "Server address is " << buffer << std::endl;
}

void GameServer::update(long timeSinceLastFrame) {
    server.AdvanceTime(server.GetTime() + ((double) timeSinceLastFrame) / 1000.0f);
    server.ReceivePackets();

    processMessages();

    server.SendPackets();
}

void GameServer::processMessages(void) {
    for(int i = 0; i < 64; i++) {
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
    std::cout << "client " << clientIndex << " connected" << std::endl;
}

void GameServer::clientDisconnected(int clientIndex) {
    std::cout << "client " << clientIndex << " disconnected" << std::endl;
}

yojimbo::Message* GameServer::createMessage(int clientIndex, GameMessageType messageType) {
    return server.CreateMessage(clientIndex, (int) messageType);
}

void GameServer::sendMessage(int clientIndex, yojimbo::Message* message) {
    server.SendMessage(clientIndex, (int) GameChannel::RELIABLE, message);
}

yojimbo::Address GameServer::getAddress(void) const {
    return address;
}

GameAdapter& GameServer::getAdapter(void) {
    return adapter;
}