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

        default:
            break;
    }
}

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdate update) {
    std::cout << "Got game state update: " << std::endl;

    for(int i = 0; i < update.numEntities; i++) {
        std::cout << "Entity " << update.entities[i].id << ":" << std::endl;
        std::cout << "\tHP: " << update.entities[i].currentHP << std::endl;
        std::cout << "\tPosition: (" << update.entities[i].x << ", " <<  update.entities[i].y << ")" << std::endl;
    }
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
