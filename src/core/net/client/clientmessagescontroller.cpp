#include "clientmessagescontroller.h"

ClientMessagesController::ClientMessagesController(std::shared_ptr<GameClient> client) :
    client(client)
{ }

void ClientMessagesController::sendFindPathMessage(const glm::ivec2& position) {
    FindPathMessage* message = (FindPathMessage*) client->createMessage(GameMessageType::FIND_PATH);
    message->x = position.x;
    message->y = position.y;

    client->sendMessage(message);
}