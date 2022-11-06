#include "gameclientmessagesreceiver.h"

GameClientMessagesReceiver::GameClientMessagesReceiver()
{ }

void GameClientMessagesReceiver::receiveGameStateUpdate(GameStateUpdate update) {
    std::cout << "Got game state update with entities" << std::endl;
}

void GameClientMessagesReceiver::receiveTestMessage(int data) {
    std::cout << "Received test data " << data << std::endl;
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
        }

        default:
            break;
    }
}