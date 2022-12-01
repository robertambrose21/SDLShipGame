#include "gameservermessagesreceiver.h"

GameServerMessagesReceiver::GameServerMessagesReceiver(std::shared_ptr<ApplicationContext> context) :
    context(context)
{ }

void GameServerMessagesReceiver::receiveMessage(const int& participantId, yojimbo::Message* message) {
    switch(message->GetType()) {
        case (int) GameMessageType::FIND_PATH: {
            FindPathMessage* findPathMessage = (FindPathMessage*) message;
            receiveFindPathMessage(
                participantId, 
                findPathMessage->entityId, 
                { findPathMessage->x, findPathMessage->y },
                findPathMessage->shortStopSteps
            );
            break;
        }

        case (int) GameMessageType::SELECT_ENTITY: {
            SelectEntityMessage* selectEntityMessage = (SelectEntityMessage*) message;
            receiveSelectEntityMessage(participantId, selectEntityMessage->id);
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackEntityMessage* attackEntityMessage = (AttackEntityMessage*) message;
            receieveAttackEntityMessage(
                participantId, 
                attackEntityMessage->entityId, 
                attackEntityMessage->targetId, 
                attackEntityMessage->weaponId
            );
            break;
        }

        case (int) GameMessageType::PASS_PARTICIPANT_TURN: {
            PassParticipantTurnMessage* passParticipantTurnMessage = (PassParticipantTurnMessage*) message;
            receivePassParticipantTurnMessage(participantId, passParticipantTurnMessage->participantId);
            break;
        }

        default:
            break;
    }
}

void GameServerMessagesReceiver::receiveFindPathMessage(
    const int& participantId,
    const uint32_t& entityId,
    const glm::ivec2& position,
    const int& shortStopSteps
) {
    auto entities = context->getTurnController()->getParticipant(participantId)->entities;

    for(auto entity : entities) {
        if(entity->getId() == entityId) {
            entity->findPath(position, shortStopSteps);
        }
    }
}

void GameServerMessagesReceiver::receiveSelectEntityMessage(const int& participantId, const uint32_t& entityId) {
    auto entity = context->getEntityPool()->getEntity(entityId);

    if(entity->getParticipantId() != participantId) {
        return;
    }

    entity->setSelected(!entity->isSelected());
}

void GameServerMessagesReceiver::receieveAttackEntityMessage(
    const int& participantId, 
    const uint32_t& entityId, 
    const uint32_t& targetId, 
    const uint32_t& weaponId
) {
    auto entity = context->getEntityPool()->getEntity(entityId);

    if(entity->getParticipantId() != participantId) {
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
    const int& participantId,
    const int& receivedParticipantId
) {
    if(participantId != receivedParticipantId) {
        std::cout << "Could not pass participant turn, ids do not match" << std::endl;
        return;
    }

    context->getTurnController()->passParticipant(participantId);
}