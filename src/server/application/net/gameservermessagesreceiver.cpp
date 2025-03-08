#include "gameservermessagesreceiver.h"
#include "application/serverturncontroller.h"

GameServerMessagesReceiver::GameServerMessagesReceiver(ApplicationContext& context) :
    context(context)
{ }

void GameServerMessagesReceiver::setTransmitter(GameServerMessagesTransmitter* transmitter) {
    this->transmitter = transmitter;
}

void GameServerMessagesReceiver::receiveMessage(int clientIndex, yojimbo::Message* message) {
    switch(message->GetType()) {
        case (int) GameMessageType::FIND_PATH: {
            FindPathMessage* findPathMessage = (FindPathMessage*) message;
            receiveFindPathMessage(
                clientIndex, 
                findPathMessage->entityId, 
                { findPathMessage->x, findPathMessage->y },
                findPathMessage->shortStopSteps,
                findPathMessage->turnNumber
            );
            break;
        }

        case (int) GameMessageType::SELECT_ENTITY: {
            SelectEntityMessage* selectEntityMessage = (SelectEntityMessage*) message;
            receiveSelectEntityMessage(clientIndex, selectEntityMessage->id);
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackMessage* attackMessage = (AttackMessage*) message;
            receieveAttackMessage(
                clientIndex, 
                attackMessage->entityId, 
                attackMessage->x, 
                attackMessage->y,
                attackMessage->weaponIdBytes
            );
            break;
        }

        case (int) GameMessageType::PASS_PARTICIPANT_TURN: {
            PassParticipantTurnMessage* passParticipantTurnMessage = (PassParticipantTurnMessage*) message;
            receivePassParticipantTurnMessage(clientIndex, passParticipantTurnMessage->participantId);
            break;
        }

        case (int) GameMessageType::SET_PARTICIPANT_ACK: {
            SetParticipantAckMessage* setParticipantAckMessage = (SetParticipantAckMessage*) message;
            receiveSetParticipantAckMessage(clientIndex, setParticipantAckMessage->participantId);
            break;
        }

        case (int) GameMessageType::EQUIP_ITEM: {
            EquipItemMessage* equipItemMessage = (EquipItemMessage*) message;
            receiveEquipItemMessage(
                clientIndex, 
                equipItemMessage->itemId, 
                equipItemMessage->entityId, 
                equipItemMessage->slot,
                equipItemMessage->isUnequip
            );
            break;
        }

        case (int) GameMessageType::EQUIP_WEAPON: {
            EquipWeaponMessage* equipWeaponMessage = (EquipWeaponMessage*) message;
            receiveEquipWeaponMessage(
                clientIndex,
                equipWeaponMessage->itemId,
                equipWeaponMessage->entityId,
                equipWeaponMessage->weaponIdBytes,
                equipWeaponMessage->isUnequip
            );
            break;
        }

        default:
            break;
    }
}

void GameServerMessagesReceiver::receiveFindPathMessage(
    int clientIndex,
    uint32_t entityId,
    const glm::ivec2& position,
    int shortStopSteps,
    int turnNumber
) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto turnController = dynamic_cast<ServerTurnController*>(context.getTurnController());
    auto participantId = turnController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    auto participant = turnController->getParticipant(participantId);
    auto const& entities = participant->getEntities();

    for(auto const& entity : entities) {
        if(entity->getId() != entityId) {
            continue;
        }

        if(turnNumber == -1) {
            turnController->executeActionImmediately(std::make_unique<MoveAction>(participant, entity, position));
        }
        else {
            turnController->queueAction(std::make_unique<MoveAction>(participant, entity, turnNumber, position));
        }
        
    }
}

void GameServerMessagesReceiver::receiveSelectEntityMessage(int clientIndex, uint32_t entityId) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }
    
    auto const& entity = context.getEntityPool()->getEntity(entityId);

    entity->setSelected(!entity->isSelected());
}

void GameServerMessagesReceiver::receieveAttackMessage(
    int clientIndex, 
    uint32_t entityId, 
    int x,
    int y,
    uint8_t weaponIdBytes[16]
) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto turnController = dynamic_cast<ServerTurnController*>(context.getTurnController());
    auto participantId = turnController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    auto weaponId = UUID::fromBytes(weaponIdBytes);
    auto const& entity = context.getEntityPool()->getEntity(entityId);
    auto participant = turnController->getParticipant(participantId);

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->queueAction(
                std::make_unique<AttackAction>(
                    participant, 
                    entity, 
                    weapon, 
                    glm::ivec2(x, y)
                )
            );
        }
    }
}

void GameServerMessagesReceiver::receivePassParticipantTurnMessage(
    int clientIndex,
    int receivedParticipantId
) {
    if(!clientParticipantsLoaded[clientIndex].contains(receivedParticipantId)) {
        std::cout << "Could not pass participant turn, ids do not match" << std::endl;
        return;
    }

    // context.getTurnController()->passParticipant(receivedParticipantId); // TODO
}

void GameServerMessagesReceiver::receiveSetParticipantAckMessage(int clientIndex, int participantId) {
    // TODO: This could be spoofed - need to check that this participant does actually belong to this client
    clientParticipantsLoaded[clientIndex].insert(participantId);

    // std::cout << "Got participant ACK " << participantId << std::endl;
}

void GameServerMessagesReceiver::receiveEquipItemMessage(
    int clientIndex, 
    uint32_t itemId, 
    uint32_t entityId, 
    uint8_t slot, 
    bool isUnequip
) {
    auto turnController = dynamic_cast<ServerTurnController*>(context.getTurnController());
    auto participantId = turnController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    if(!contains(Gear::VALID_SLOTS, (Equippable<Stats::GearStats>::Slot) slot)) {
        std::cout << std::format("Warning: received equip gear message with invalid slot {}", 
                Equippable<Stats::GearStats>::SLOT_NAMES[slot]) << std::endl;
        return;
    }
    
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    if(!context.getItemController()->hasItem(itemId)) {
        return;
    }

    auto participant = turnController->getParticipant(participantId);
    auto item = context.getItemController()->getItem(itemId);
    auto entity = context.getEntityPool()->getEntity(entityId);

    turnController->queueAction(std::make_unique<EquipGearAction>(
        participant, 
        entity, 
        item,
        (Equippable<Stats::GearStats>::Slot) slot,
        isUnequip
    ));
}

void GameServerMessagesReceiver::receiveEquipWeaponMessage(
    int clientIndex, 
    uint32_t itemId, 
    uint32_t entityId, 
    uint8_t weaponIdBytes[16],
    bool isUnequip
) {
    auto turnController = dynamic_cast<ServerTurnController*>(context.getTurnController());
    auto participantId = turnController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    if(!context.getItemController()->hasItem(itemId)) {
        return;
    }

    auto participant = turnController->getParticipant(participantId);
    auto weaponId = UUID::fromBytes(weaponIdBytes);
    auto item = context.getItemController()->getItem(itemId);
    auto entity = context.getEntityPool()->getEntity(entityId);

    turnController->queueAction(std::make_unique<EquipWeaponAction>(
        participant, 
        entity, 
        item,
        weaponId
    ));
}

bool GameServerMessagesReceiver::areParticipantsLoadedForClient(int clientIndex) {
    if(!clientParticipantsLoaded.contains(clientIndex)) {
        return false;
    }

    auto const& participants = context.getTurnController()->getParticipants();
    auto const& loaded = clientParticipantsLoaded[clientIndex];

    if(participants.size() != loaded.size()) {
        return false;
    }

    for(auto& participant : participants) {
        if(!loaded.contains(participant->getId())) {
            return false;
        }
    }

    return true;
}