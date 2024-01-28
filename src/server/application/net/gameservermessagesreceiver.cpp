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
                findPathMessage->shortStopSteps
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
                attackMessage->weaponId
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

        default:
            break;
    }
}

void GameServerMessagesReceiver::receiveFindPathMessage(
    int clientIndex,
    uint32_t entityId,
    const glm::ivec2& position,
    int shortStopSteps
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

    auto const& entities = turnController->getParticipant(participantId)->entities;

    for(auto const& entity : entities) {
        if(entity->getId() == entityId) {
            turnController->queueAction(std::make_unique<MoveAction>(turnController->getTurnNumber(), entity, position));
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
    uint32_t weaponId
) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto const& entity = context.getEntityPool()->getEntity(entityId);

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->queueAction(std::make_unique<AttackAction>(
                context.getTurnController()->getTurnNumber(), entity, weapon, glm::ivec2(x, y)));
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

    context.getTurnController()->passParticipant(receivedParticipantId);
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
    if(slot >= Equipment::Slot::COUNT) {
        return;
    }
    
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    if(!context.getItemController()->hasItem(itemId)) {
        return;
    }

    auto turnController = context.getTurnController();
    auto item = context.getItemController()->getItem(itemId);
    auto entity = context.getEntityPool()->getEntity(entityId);

    turnController->queueAction(std::make_unique<EquipItemAction>(
        turnController->getTurnNumber(), 
        entity, 
        item,
        (Equipment::Slot) slot,
        isUnequip
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
        if(!loaded.contains(participant->id)) {
            return false;
        }
    }

    return true;
}