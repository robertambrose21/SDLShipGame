#include "gameservermessagesreceiver.h"

GameServerMessagesReceiver::GameServerMessagesReceiver(std::shared_ptr<ApplicationContext> context) :
    context(context)
{ }

void GameServerMessagesReceiver::receiveMessage(const int& clientIndex, yojimbo::Message* message) {
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
            AttackEntityMessage* attackEntityMessage = (AttackEntityMessage*) message;
            receieveAttackEntityMessage(
                clientIndex, 
                attackEntityMessage->entityId, 
                attackEntityMessage->targetId, 
                attackEntityMessage->weaponId
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

        default:
            break;
    }
}

void GameServerMessagesReceiver::receiveFindPathMessage(
    const int& clientIndex,
    const uint32_t& entityId,
    const glm::ivec2& position,
    const int& shortStopSteps
) {
    // TODO: Get real participant
    auto entities = context->getTurnController()->getParticipant(0)->entities;

    for(auto entity : entities) {
        if(entity->getId() == entityId) {
            entity->findPath(position, shortStopSteps);
        }
    }
}

void GameServerMessagesReceiver::receiveSelectEntityMessage(const int& clientIndex, const uint32_t& entityId) {
    auto entity = context->getEntityPool()->getEntity(entityId);

    if(entity == nullptr) {
        return;
    }

    if(entity->getParticipantId() != 0) {
        return;
    }

    entity->setSelected(!entity->isSelected());
}

void GameServerMessagesReceiver::receieveAttackEntityMessage(
    const int& clientIndex, 
    const uint32_t& entityId, 
    const uint32_t& targetId, 
    const uint32_t& weaponId
) {
    auto entity = context->getEntityPool()->getEntity(entityId);

    if(entity == nullptr) {
        return;
    }

    if(entity->getParticipantId() != 0) {
        return;
    }

    auto target = context->getEntityPool()->getEntity(targetId);

    for(auto [_, weapon] : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            entity->attack(target, weapon);
        }
    }
}

void GameServerMessagesReceiver::receivePassParticipantTurnMessage(
    const int& clientIndex,
    const int& receivedParticipantId
) {
    if(receivedParticipantId != 0) {
        std::cout << "Could not pass participant turn, ids do not match" << std::endl;
        return;
    }

    context->getTurnController()->passParticipant(clientIndex);
}

void GameServerMessagesReceiver::receiveSetParticipantAckMessage(const int& clientIndex, const int& participantId) {
    clientParticipantsLoaded[clientIndex].insert(participantId);

    std::cout << "Got participant ACK " << participantId << std::endl;
}

bool GameServerMessagesReceiver::areParticipantsLoadedForClient(int clientIndex) {
    if(!clientParticipantsLoaded.contains(clientIndex)) {
        return false;
    }

    auto participants = context->getTurnController()->getParticipants();
    auto loaded = clientParticipantsLoaded[clientIndex];

    if(participants.size() != loaded.size()) {
        return false;
    }

    for(auto [participantId, _] : participants) {
        if(!loaded.contains(participantId)) {
            return false;
        }
    }

    return true;
}