#include "gameclient.h"

GameClient::GameClient(const yojimbo::Address& serverAddress) :
    adapter(GameAdapter()),
    client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), connectionConfig = GameConnectionConfig(), adapter, 0.0)
{
    uint64_t clientId;
    yojimbo::random_bytes((uint8_t*)&clientId, 8);
    client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, serverAddress);
}

void GameClient::update(long timeSinceLastFrame) {
    client.AdvanceTime(client.GetTime() + ((double) timeSinceLastFrame) / 1000.0f);
    client.ReceivePackets();

    if(client.IsConnected()) {
        processMessages();

        while(!messageQueue.empty()) {
            client.SendMessage((int) GameChannel::RELIABLE, messageQueue.front());
            messageQueue.pop();
        }
    }

    client.SendPackets();
}

yojimbo::Message* GameClient::createMessage(GameMessageType messageType) {
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
    switch(message->GetType()) {
        // case (int) GameMessageType::TEST
        //     std::cout << "Client receieved a test message from server with data " << ((TestMessage2*) message)->m_data << std::endl;
        //     break;

        default:
            break;
    }
}