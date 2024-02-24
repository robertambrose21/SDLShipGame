#include "gameclientmessagestransmitter.h"

GameClientMessagesTransmitter::GameClientMessagesTransmitter(GameClient& client) :
    client(client)
{ }

void GameClientMessagesTransmitter::sendActionsRollMessage(int participantId) {
    ActionsRollMessage* message = (ActionsRollMessage*) client.createMessage(GameMessageType::ACTIONS_ROLL);

    message->participantId = participantId;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendFindPathMessage(
    uint32_t entityId,
    const glm::ivec2& position,
    int shortStopSteps
) {
    FindPathMessage* message = (FindPathMessage*) client.createMessage(GameMessageType::FIND_PATH);

    message->entityId = entityId;
    message->x = position.x;
    message->y = position.y;
    message->shortStopSteps = shortStopSteps;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendSelectEntityMessage(uint32_t entityId) {
    SelectEntityMessage* message = (SelectEntityMessage*) client.createMessage(GameMessageType::SELECT_ENTITY);

    message->id = entityId;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendAttackMessage(
    uint32_t entityId, 
    const glm::ivec2& target,
    UUID weaponId
) {
    AttackMessage* message = (AttackMessage*) client.createMessage(GameMessageType::ATTACK_ENTITY);
    
    message->entityId = entityId;
    message->x = target.x;
    message->y = target.y;
    memcpy(message->weaponIdBytes, &weaponId.getBytes()[0], 16);

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendPassParticipantTurnMessage(int participantId) {
    PassParticipantTurnMessage* message = (PassParticipantTurnMessage*) client.createMessage(GameMessageType::PASS_PARTICIPANT_TURN);

    message->participantId = participantId;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendSetParticipantAckMessage(int participantId) {
    SetParticipantAckMessage* message = (SetParticipantAckMessage*) client.createMessage(GameMessageType::SET_PARTICIPANT_ACK);

    message->participantId = participantId;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendEquipItemMessage(uint32_t itemId, uint32_t entityId, Equipment::Slot slot, bool isUnequip) {
    EquipItemMessage* message = (EquipItemMessage*) client.createMessage(GameMessageType::EQUIP_ITEM);

    message->itemId = itemId;
    message->entityId = entityId;
    message->slot = slot;
    message->isUnequip = isUnequip;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendEquipWeaponMessage(uint32_t itemId, uint32_t entityId, UUID weaponId, bool isUnequip) {
    EquipWeaponMessage* message = (EquipWeaponMessage*) client.createMessage(GameMessageType::EQUIP_WEAPON);

    message->itemId = itemId;
    message->entityId = entityId;
    memcpy(message->weaponIdBytes, &weaponId.getBytes()[0], 16);
    message->isUnequip = isUnequip;

    client.sendMessage(message);
}
