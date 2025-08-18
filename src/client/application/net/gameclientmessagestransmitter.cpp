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
    uint32_t actorId,
    const glm::ivec2& position,
    int shortStopSteps,
    int turnNumber
) {
    FindPathMessage* message = (FindPathMessage*) client.createMessage(GameMessageType::FIND_PATH);

    message->actorId = actorId;
    message->x = position.x;
    message->y = position.y;
    message->shortStopSteps = shortStopSteps;
    message->turnNumber = turnNumber;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendSelectActorMessage(uint32_t actorId) {
    SelectActorMessage* message = (SelectActorMessage*) client.createMessage(GameMessageType::SELECT_ENTITY);

    message->id = actorId;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendAttackMessage(
    uint32_t actorId, 
    const glm::ivec2& target,
    UUID weaponId,
    int turnNumber
) {
    AttackMessage* message = (AttackMessage*) client.createMessage(GameMessageType::ATTACK_ENTITY);
    
    message->actorId = actorId;
    message->x = target.x;
    message->y = target.y;
    memcpy(message->weaponIdBytes, &weaponId.getBytes()[0], 16);
    message->turnNumber = turnNumber;

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

void GameClientMessagesTransmitter::sendEquipItemMessage(uint32_t itemId, uint32_t actorId, Equippable<Stats::GearStats>::Slot slot, bool isUnequip) {
    EquipItemMessage* message = (EquipItemMessage*) client.createMessage(GameMessageType::EQUIP_ITEM);

    message->itemId = itemId;
    message->actorId = actorId;
    message->slot = slot;
    message->isUnequip = isUnequip;

    client.sendMessage(message);
}

void GameClientMessagesTransmitter::sendEquipWeaponMessage(uint32_t itemId, uint32_t actorId, UUID weaponId, bool isUnequip) {
    EquipWeaponMessage* message = (EquipWeaponMessage*) client.createMessage(GameMessageType::EQUIP_WEAPON);

    message->itemId = itemId;
    message->actorId = actorId;
    memcpy(message->weaponIdBytes, &weaponId.getBytes()[0], 16);
    message->isUnequip = isUnequip;

    client.sendMessage(message);
}
