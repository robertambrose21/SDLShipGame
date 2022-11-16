#include "gameservermessagestransmitter.h"

GameServerMessagesTransmitter::GameServerMessagesTransmitter(
    std::shared_ptr<GameServer> server,
    std::function<void(int)> onClientConnectFunc
) :
    server(server),
    onClientConnectFunc(onClientConnectFunc)
{ }

void GameServerMessagesTransmitter::onClientConnected(int clientIndex) {
    onClientConnectFunc(clientIndex);
}

void GameServerMessagesTransmitter::sendSetParticipant(int clientIndex, int participantId) {
    SetParticipantMessage* message = (SetParticipantMessage*) server->createMessage(clientIndex, GameMessageType::SET_PARTICIPANT);

    message->participantId = participantId;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendGameStateUpdate(int clientIndex, GameStateUpdate update) {
    GameStateUpdateMessage* message = (GameStateUpdateMessage*) server->createMessage(clientIndex, GameMessageType::GAME_STATE_UPDATE);

    message->gameStateUpdate = update;

    server->sendMessage(clientIndex, message);
}

void GameServerMessagesTransmitter::sendLoadMap(int clientIndex, const MapBlock& block) {
    LoadMapMessage* message = (LoadMapMessage*) server->createMessage(clientIndex, GameMessageType::LOAD_MAP);

    message->mapBlock = block;

    server->sendMessage(clientIndex, message);
}