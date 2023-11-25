#include "gameservermessagesreceiver.h"

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
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }

    // TODO: Get real participant
    auto const& entities = context.getTurnController()->getParticipant(0)->entities;

    for(auto const& entity : entities) {
        if(entity->getId() == entityId) {
            context.getTurnController()->queueAction(std::make_unique<MoveAction>(entity, position));
        }
    }
}

void GameServerMessagesReceiver::receiveSelectEntityMessage(int clientIndex, uint32_t entityId) {
    if(!context.getEntityPool()->hasEntity(entityId)) {
        return;
    }
    
    auto const& entity = context.getEntityPool()->getEntity(entityId);

    if(entity->getParticipantId() != 0) {
        return;
    }

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

    if(entity->getParticipantId() != 0) { // TODO: Why is this here??
        return;
    }

    for(auto weapon : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            context.getTurnController()->queueAction(std::make_unique<AttackAction>(entity, weapon, glm::ivec2(x, y)));
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

    context.getTurnController()->passParticipant(clientIndex);
}

void GameServerMessagesReceiver::receiveSetParticipantAckMessage(int clientIndex, int participantId) {
    clientParticipantsLoaded[clientIndex].insert(participantId);

    // std::cout << "Got participant ACK " << participantId << std::endl;
}

void GameServerMessagesReceiver::receiveActionsRollMessage(int clientIndex, int participantId) {
    DiceActionResult dice;

    for(auto [action, num] : context.getTurnController()->rollActions(participantId)) {
        for(int i = 0; i < num; i++) {
            dice.actions[i] = action;
            dice.rollNumber++;
        }
    }

    game_assert(dice.rollNumber >= 1 && dice.rollNumber <= 6);

    transmitter->sendActionsRollResponse(clientIndex, participantId, { dice });
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