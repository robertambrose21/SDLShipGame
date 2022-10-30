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

void ClientMessagesController::sendSelectEntityMessage(const uint32_t& entityId) {
    SelectEntityMessage* message = (SelectEntityMessage*) client->createMessage(GameMessageType::SELECT_ENTITY);

    message->id = entityId;

    client->sendMessage(message);
}

void ClientMessagesController::sendAttackEntityMessage(
    const uint32_t& entityId, 
    const uint32_t& targetId, 
    const uint32_t& weaponId
) {
    AttackEntityMessage* message = (AttackEntityMessage*) client->createMessage(GameMessageType::ATTACK_ENTITY);
    
    message->entityId = entityId;
    message->targetId = targetId;
    message->weaponId = weaponId;

    client->sendMessage(message);
}
