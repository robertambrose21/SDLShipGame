#include "gameclientmessagesreceiver.h"

GameClientMessagesReceiver::GameClientMessagesReceiver(std::shared_ptr<ApplicationContext> context) :
    context(context)
{ }

void GameClientMessagesReceiver::setTransmitter(std::shared_ptr<GameClientMessagesTransmitter> transmitter) {
    this->transmitter = transmitter;
}

void GameClientMessagesReceiver::setPlayerController(std::shared_ptr<PlayerController> playerController) {
    this->playerController = playerController;
}

void GameClientMessagesReceiver::receiveMessage(yojimbo::Message* message) {
    switch(message->GetType()) {
        case (int) GameMessageType::GAME_STATE_UPDATE: {
            GameStateUpdateMessage* updateMessage = (GameStateUpdateMessage*) message;
            receiveGameStateUpdate(updateMessage->gameStateUpdate);
            break;
        }

        case (int) GameMessageType::TEST_MESSAGE: {
            GameTestMessage* testMessage = (GameTestMessage*) message;
            receiveTestMessage(testMessage->data);
            break;
        }

        case (int) GameMessageType::SET_PARTICIPANT: {
            SetParticipantMessage* setParticipantMessage = (SetParticipantMessage*) message;
            receiveSetParticipant(
                setParticipantMessage->participantId,
                setParticipantMessage->isPlayer
            );
            break;
        }

        case (int) GameMessageType::LOAD_MAP: {
            LoadMapMessage* loadMapMessage = (LoadMapMessage*) message;
            receiveLoadMap(loadMapMessage->mapBlock);
            break;
        }

        case (int) GameMessageType::FIND_PATH: {
            FindPathMessage* findPathMessage = (FindPathMessage*) message;
            receiveFindPath(
                findPathMessage->entityId,
                { findPathMessage->x, findPathMessage->y },
                findPathMessage->shortStopSteps
            );
            break;
        }

        case (int) GameMessageType::ATTACK_ENTITY: {
            AttackEntityMessage* attackEntityMessage = (AttackEntityMessage*) message;
            receiveAttackEntity(
                attackEntityMessage->entityId,
                attackEntityMessage->targetId,
                attackEntityMessage->weaponId
            );
            break;
        }

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdate update) {
    std::cout << "Got game state update " << update.currentParticipant << std::endl;

    context->getTurnController()->setCurrentParticipant(update.currentParticipant);
    context->getEntityPool()->addGameStateUpdate(update);
}

void GameClientMessagesReceiver::receiveTestMessage(int data) {
    std::cout << "Received test data " << data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(int participantId, bool isPlayer) {
    std::cout << "Received set participant " << participantId << std::endl;

    auto participant = context->getTurnController()->addParticipant(participantId, { }, isPlayer);

    if(isPlayer) {
        playerController->setParticipant(participant);
    }

    transmitter->sendSetParticipantAckMessage(participantId);
}

void GameClientMessagesReceiver::receiveLoadMap(MapBlock block) {
    // TODO: Sequencing
    auto grid = context->getGrid();

    for(int i = 0; i < block.blockSize; i++) {
        auto x = i / block.width;
        auto y = i % block.width;
        auto id = block.data[i];

        grid->setTile(x, y, { id, id == 1 });
    }
}

void GameClientMessagesReceiver::receiveFindPath(
    const uint32_t& entityId, 
    const glm::ivec2& position,
    const int& shortStopSteps
) {
    if(!context->getEntityPool()->hasEntity(entityId)) {
        return;
    }

    auto entity = context->getEntityPool()->getEntity(entityId);

    entity->findPath(position, shortStopSteps);
}

void GameClientMessagesReceiver::receiveAttackEntity(
    const uint32_t& entityId, 
    const uint32_t& targetId, 
    const uint32_t& weaponId
) {
    auto entityPool = context->getEntityPool();

    if(!entityPool->hasEntity(entityId) || !entityPool->hasEntity(targetId)) {
        return;
    }

    auto entity = entityPool->getEntity(entityId);
    auto target = entityPool->getEntity(targetId);

    for(auto [_, weapon] : entity->getWeapons()) {
        if(weapon->getId() == weaponId) {
            entity->attack(target, weapon);
        }
    }
}
