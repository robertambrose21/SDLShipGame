#include "gameservermessagestransmitter.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(std::shared_ptr<GameServer> server) :
    server(server)
{ }

void GameServerMessagesTransmitter::sendGameStateUpdate(GameStateUpdate update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server->createMessage(0, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server->sendMessage(0, message);
}

void GameServerMessagesTransmitter::onClientConnected(int clientIndex) {
    SetParticipantMessage* message = (SetParticipantMessage*) server->createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = 0;

    server->sendMessage(clientIndex, message);
}