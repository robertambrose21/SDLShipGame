#include "gameservermessagesreceiver.h"

GameServerMessagesReceiver::GameServerMessagesReceiver(const std::shared_ptr<ApplicationContext>& context) :
    context(context)
{ }

void GameServerMessagesReceiver::setTransmitter(const std::shared_ptr<GameServerMessagesTransmitter>& transmitter) {
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

        case (int) GameMessageType::ACTIONS_ROLL: {
            ActionsRollMessage* actionsRollMessage = (ActionsRollMessage*) message;
            receiveActionsRollMessage(clientIndex, actionsRollMessage->participantId);
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
    if(!context->getEntityPool()->hasEntity(entityId)) {
        return;
    }

    // TODO: Get real participant
    auto const& entities = context->getTurnController()->getParticipant(0)->entities;

    for(auto const& entity : entities) {
        if(entity->getId() == entityId) {
            context->getTurnController()->performMoveAction(entity, position);
            // entity->findPath(position, shortStopSteps);
        }
    }
}

void GameServerMessagesReceiver::receiveSelectEntityMessage(int clientIndex, uint32_t entityId) {
    if(!context->getEntityPool()->hasEntity(entityId)) {
        return;
    }
    
    auto const& entity = context->getEntityPool()->getEntity(entityId);

    if(entity->getParticipantId() != 0) {
        return;
    }

    entity->setSelected(!entity->isSelected());
}

void GameServerMessagesReceiver::receieveAttackEntityMessage(
    int clientIndex, 
    uint32_t entityId, 
    uint32_t targetId, 
    uint32_t weaponId
) {
    if(!context->getEntityPool()->hasEntity(entityId) || !context->getEntityPool()->hasEntity(targetId)) {
        return;
    }

    auto const& entity = context->getEntityPool()->getEntity(entityId);

    if(entity->getParticipantId() != 0) {
        return;
    }

    auto const& target = context->getEntityPool()->getEntity(targetId);

    for(auto [_, weapon] : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            // entity->attack(target, weapon);
            context->getTurnController()->performAttackAction(entity, weapon, target);
        }
    }
}

void GameServerMessagesReceiver::receivePassParticipantTurnMessage(
    int clientIndex,
    int receivedParticipantId
) {
    if(receivedParticipantId != 0) {
        std::cout << "Could not pass participant turn, ids do not match" << std::endl;
        return;
    }

    context->getTurnController()->passParticipant(clientIndex);
}

void GameServerMessagesReceiver::receiveSetParticipantAckMessage(int clientIndex, int participantId) {
    clientParticipantsLoaded[clientIndex].insert(participantId);

    std::cout << "Got participant ACK " << participantId << std::endl;
}

void GameServerMessagesReceiver::receiveActionsRollMessage(int clientIndex, int participantId) {
    std::cout << "Got actions roll message" << std::endl;

    auto actions = std::vector<int>();

    for(auto [action, num] : context->getTurnController()->rollActions(participantId)) {
        for(int i = 0; i < num; i++) {
            actions.push_back(action);
        }
    }

    auto rollNumber = actions.size();

    game_assert(rollNumber >= 1 && rollNumber <= 6);

    transmitter->sendActionsRollResponse(clientIndex, participantId, rollNumber, &actions[0]);
}

bool GameServerMessagesReceiver::areParticipantsLoadedForClient(int clientIndex) {
    if(!clientParticipantsLoaded.contains(clientIndex)) {
        return false;
    }

    auto const& participants = context->getTurnController()->getParticipants();
    auto const& loaded = clientParticipantsLoaded[clientIndex];

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