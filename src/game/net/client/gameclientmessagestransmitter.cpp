#include "gameclientmessagestransmitter.h"

GameClientMessagesTransmitter::GameClientMessagesTransmitter(const std::shared_ptr<GameClient>& client) :
    client(client)
{ }

void GameClientMessagesTransmitter::sendFindPathMessage(
    uint32_t entityId,
    const glm::ivec2& position,
    int shortStopSteps
) {
    FindPathMessage* message = (FindPathMessage*) client->createMessage(GameMessageType::FIND_PATH);

    message->entityId = entityId;
    message->x = position.x;
    message->y = position.y;
    message->shortStopSteps = shortStopSteps;

    client->sendMessage(message);
}

void GameClientMessagesTransmitter::sendSelectEntityMessage(uint32_t entityId) {
    SelectEntityMessage* message = (SelectEntityMessage*) client->createMessage(GameMessageType::SELECT_ENTITY);

    message->id = entityId;

    client->sendMessage(message);
}

void GameClientMessagesTransmitter::sendAttackEntityMessage(
    uint32_t entityId, 
    uint32_t targetId, 
    uint32_t weaponId
) {
    AttackEntityMessage* message = (AttackEntityMessage*) client->createMessage(GameMessageType::ATTACK_ENTITY);
    
    message->entityId = entityId;
    message->targetId = targetId;
    message->weaponId = weaponId;

    client->sendMessage(message);
}

void GameClientMessagesTransmitter::sendPassParticipantTurnMessage(int participantId) {
    PassParticipantTurnMessage* message = (PassParticipantTurnMessage*) client->createMessage(GameMessageType::PASS_PARTICIPANT_TURN);

    message->participantId = participantId;

    client->sendMessage(message);
}

void GameClientMessagesTransmitter::sendSetParticipantAckMessage(int participantId) {
    SetParticipantAckMessage* message = (SetParticipantAckMessage*) client->createMessage(GameMessageType::SET_PARTICIPANT_ACK);

    message->participantId = participantId;

    client->sendMessage(message);
}