#include "gameservermessagesreceiver.h"
#include "application/servergamecontroller.h"

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
                findPathMessage->actorId, 
                { findPathMessage->x, findPathMessage->y },
                findPathMessage->shortStopSteps,
                findPathMessage->turnNumber
            );
            break;
        }

        case (int) GameMessageType::SELECT_ENTITY: {
            SelectActorMessage* selectActorMessage = (SelectActorMessage*) message;
            receiveSelectActorMessage(clientIndex, selectActorMessage->id);
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackMessage* attackMessage = (AttackMessage*) message;
            receieveAttackMessage(
                clientIndex, 
                attackMessage->actorId, 
                attackMessage->x, 
                attackMessage->y,
                attackMessage->weaponIdBytes,
                attackMessage->turnNumber
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
                equipItemMessage->actorId, 
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
                equipWeaponMessage->actorId,
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
    uint32_t actorId,
    const glm::ivec2& position,
    int shortStopSteps,
    int turnNumber
) {
    if(!context.getActorPool()->getByExternalId(actorId).has_value()) {
        return;
    }

    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());
    auto participantId = gameController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    auto participant = gameController->getParticipant(participantId);
    auto const& actors = participant->getActors();

    for(auto entity : actors) {
        auto const& externalId = context.getEntityRegistry().get<ExternalId>(entity);

        if(externalId != actorId) {
            continue;
        }

        if(turnNumber == -1) {
            gameController->executeActionImmediately(std::make_unique<MoveAction>(participant, entity, position));
        }
        else {
            gameController->queueAction(std::make_unique<MoveAction>(participant, entity, turnNumber, position));
        }
        
    }
}

// TODO: This should not be a toggle but take a bool from the message
void GameServerMessagesReceiver::receiveSelectActorMessage(int clientIndex, uint32_t actorId) {
    auto entity = context.getActorPool()->getByExternalId(actorId);

    if(!entity.has_value()) {
        return;
    }
    
    auto& actor = context.getEntityRegistry().get<Actor>(entity.value());

    actor.setSelected(!actor.isSelected());
}

void GameServerMessagesReceiver::receieveAttackMessage(
    int clientIndex, 
    uint32_t actorId, 
    int x,
    int y,
    uint8_t weaponIdBytes[16],
    int turnNumber
) {
    auto entity = context.getActorPool()->getByExternalId(actorId);

    if(!entity.has_value()) {
        return;
    }

    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());
    auto participantId = gameController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    auto weaponId = UUID::fromBytes(weaponIdBytes);
    auto& actor = context.getEntityRegistry().get<Actor>(entity.value());
    auto participant = gameController->getParticipant(participantId);

    for(auto weapon : actor.getWeapons()) {
        // if(weapon->getId() != weaponId) {
        //     continue;
        // }
        
        if(turnNumber != -1) {
            context.getGameController()->queueAction(
                std::make_unique<AttackAction>(
                    participant, 
                    entity.value(),
                    turnNumber,
                    weapon, 
                    glm::ivec2(x, y)
                )
            );
        }
        else {
            context.getGameController()->executeActionImmediately(
                std::make_unique<AttackAction>(
                    participant, 
                    entity.value(),
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

    auto participant = context.getGameController()->getParticipant(receivedParticipantId);

    if(!participant->getEngagement()) {
        spdlog::warn("Participant {} does not have any engagement, cannot pass turn", receivedParticipantId);
        return;
    }

    participant->passTurn();
}

void GameServerMessagesReceiver::receiveSetParticipantAckMessage(int clientIndex, int participantId) {
    // TODO: This could be spoofed - need to check that this participant does actually belong to this client
    clientParticipantsLoaded[clientIndex].insert(participantId);

    // std::cout << "Got participant ACK " << participantId << std::endl;
}

void GameServerMessagesReceiver::receiveEquipItemMessage(
    int clientIndex, 
    uint32_t itemId, 
    uint32_t actorId, 
    uint8_t slot, 
    bool isUnequip
) {
    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());
    auto participantId = gameController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    if(!contains(Gear::VALID_SLOTS, (Equippable<Stats::GearStats>::Slot) slot)) {
        std::cout << std::format("Warning: received equip gear message with invalid slot {}", 
                Equippable<Stats::GearStats>::SLOT_NAMES[slot]) << std::endl;
        return;
    }

    if(!context.getItemController()->hasItem(itemId)) {
        return;
    }

    auto entity = context.getActorPool()->getByExternalId(actorId);

    if(!entity.has_value()) {
        return;
    }

    auto participant = gameController->getParticipant(participantId);
    auto item = context.getItemController()->getItem(itemId);

    gameController->executeActionImmediately(std::make_unique<EquipGearAction>(
        participant, 
        entity.value(), 
        item,
        (Equippable<Stats::GearStats>::Slot) slot,
        isUnequip
    ));
}

void GameServerMessagesReceiver::receiveEquipWeaponMessage(
    int clientIndex, 
    uint32_t itemId, 
    uint32_t actorId, 
    uint8_t weaponIdBytes[16],
    bool isUnequip
) {
    auto gameController = dynamic_cast<ServerGameController*>(context.getGameController());
    auto participantId = gameController->getAttachedParticipantId(clientIndex);

    if(participantId == -1) {
        std::cout << "Something went wrong: participant not found for client " << clientIndex << std::endl;
        return;
    }

    if(!context.getItemController()->hasItem(itemId)) {
        return;
    }

    auto entity = context.getActorPool()->getByExternalId(actorId);

    if(!entity.has_value()) {
        return;
    }

    auto participant = gameController->getParticipant(participantId);
    auto weaponId = UUID::fromBytes(weaponIdBytes);
    auto item = context.getItemController()->getItem(itemId);

    spdlog::trace(
        "Player {} weapon {} from actor {}", 
        isUnequip ? "unequipping" : "equipping",
        weaponId.getString(), 
        actorId
    );

    gameController->executeActionImmediately(std::make_unique<EquipWeaponAction>(
        participant, 
        entity.value(), 
        item,
        weaponId,
        isUnequip
    ));
}

bool GameServerMessagesReceiver::areParticipantsLoadedForClient(int clientIndex) {
    if(!clientParticipantsLoaded.contains(clientIndex)) {
        return false;
    }

    auto const& participants = context.getGameController()->getParticipants();
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