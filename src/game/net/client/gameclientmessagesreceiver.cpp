#include "gameclientmessagesreceiver.h"

GameClientMessagesReceiver::GameClientMessagesReceiver(std::shared_ptr<ApplicationContext> context) :
    context(context)
{ }

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
            receiveSetParticipant(setParticipantMessage->participantId);
            break;
        }

        case (int) GameMessageType::LOAD_MAP: {
            LoadMapMessage* loadMapMessage = (LoadMapMessage*) message;
            receiveLoadMap(loadMapMessage->mapBlock);
            break;
        }

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdate update) {
    std::cout << "Got game state update: " << std::endl;

    for(int i = 0; i < update.numEntities; i++) {
        std::cout << "Entity " << update.entities[i].name << "#" 
            << update.entities[i].id << "(" << update.entities[i].currentHP << "/" 
            << update.entities[i].totalHP << "):" << std::endl;
        std::cout << "\tTexture ID: " << (unsigned int) update.entities[i].textureId << std::endl;
        std::cout << "\tPosition: (" << update.entities[i].x << ", " <<  update.entities[i].y << ")" << std::endl;
        std::cout << "\tMoves per turn: " << update.entities[i].movesPerTurn << std::endl;
        std::cout << "\tMoves left: " << update.entities[i].movesLeft << std::endl;
    }

    context->getEntityPool()->synchronize({ update });
}

void GameClientMessagesReceiver::receiveTestMessage(int data) {
    std::cout << "Received test data " << data << std::endl;
}

void GameClientMessagesReceiver::receiveSetParticipant(int participantId) {
    // std::cout << "Received set participant message " << participantId << std::endl;
    // TODO: Proper check
    if(playerController) {
        playerController->setParticipant(context->getTurnController()->getParticipant(participantId));
    }
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
