#include "gameclient.h"

GameClient::GameClient(
    std::unique_ptr<MessageLogger> messageLogger,
    ClientMessagesReceiver& receiver,
    const yojimbo::Address& serverAddress
) :
    messageLogger(std::move(messageLogger)),
    receiver(receiver),
    adapter(ClientGameAdapter()),
    client(
        yojimbo::GetDefaultAllocator(), 
        yojimbo::Address("0.0.0.0"), 
        connectionConfig = ClientGameConnectionConfig(), 
        adapter, 
        0.0
    )
{
    game_assert(connectionConfig.numChannels == 2);
    uint64_t clientId;
    yojimbo::random_bytes((uint8_t*)&clientId, 8);
    client.InsecureConnect(DEFAULT_PRIVATE_KEY_CLIENT, clientId, serverAddress);
}

void GameClient::update(int64_t timeSinceLastFrame) {
    client.AdvanceTime(client.GetTime() + ((double) timeSinceLastFrame) / 1000.0f);
    client.ReceivePackets();

    if(client.IsConnected()) {
        processMessages();

        while(!messageQueue.empty()) {
            client.SendMessage((int) ClientGameChannel::RELIABLE, messageQueue.front());
            messageLogger->logMessage(messageQueue.front(), false);
            messageQueue.pop();
        }
    }

    // TODO: See message in gameserver.cpp
    if(client.HasMessagesToSend((int) ClientGameChannel::RELIABLE)) {
        client.SendPackets();
    }
}

yojimbo::Message* GameClient::createMessage(const GameMessageType& messageType) {
    game_assert((int) messageType < (int) GameMessageType::COUNT);
    return client.CreateMessage((int) messageType);
}

void GameClient::sendMessage(yojimbo::Message* message) {
    messageQueue.push(message);
}

void GameClient::processMessages(void) {
    for(int i = 0; i < connectionConfig.numChannels; i++) {
        yojimbo::Message* message = client.ReceiveMessage(i);
        while(message != NULL) {
            processMessage(message);
            client.ReleaseMessage(message);
            message = client.ReceiveMessage(i);
        }
    }
}

void GameClient::processMessage(yojimbo::Message* message) {
    receiver.receiveMessage(message);
    messageLogger->logMessage(message, true);
}