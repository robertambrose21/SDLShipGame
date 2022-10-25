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
            TestMessage2* message = (TestMessage2*) client.CreateMessage((int) GameMessageType::TEST);
            message->m_data = messageQueue.front();
            messageQueue.pop();
            client.SendMessage((int) GameChannel::RELIABLE, message);
        }
    }

    client.SendPackets();
}

void GameClient::sendMessage(void) {
    messageQueue.push(42);
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
        case (int) GameMessageType::TEST:
            std::cout << "Client receieved a test message from server with data " << ((TestMessage2*) message)->m_data << std::endl;
            break;

        default:
            break;
    }
}